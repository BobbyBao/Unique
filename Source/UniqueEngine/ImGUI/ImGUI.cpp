#include "UniquePCH.h"
#include "ImGUI.h"
#include "Graphics/Geometry.h"
#include "Graphics/PipelineState.h"
#include <SDL/SDL.h>
#include "../Input/Input.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION

#define NK_API UNIQUE_API

#include "nuklear.h"


namespace Unique
{
	struct nk_vertex
	{
		float position[2];
		float uv[2];
		nk_byte col[4];
	};

	struct NkImpl
	{
		nk_context ctx;
		nk_font_atlas atlas;
		nk_buffer cmds;
		nk_draw_null_texture null;
		unsigned int max_vertex_buffer;
		unsigned int max_index_buffer;
	};

	static void
		nk_sdl_clipbard_paste(nk_handle usr, struct nk_text_edit *edit)
	{
		const char *text = SDL_GetClipboardText();
		if (text) nk_textedit_paste(edit, text, nk_strlen(text));
		(void)usr;
	}

	static void
		nk_sdl_clipbard_copy(nk_handle usr, const char *text, int len)
	{
		char *str = 0;
		(void)usr;
		if (!len) return;
		str = (char*)malloc((size_t)len + 1);
		if (!str) return;
		memcpy(str, text, (size_t)len);
		str[len] = '\0';
		SDL_SetClipboardText(str);
		free(str);
	}

#define MAX_VERTEX_BUFFER 32 * 1024
#define MAX_INDEX_BUFFER 8 * 1024

	GUISystem::GUISystem() : impl_(*new NkImpl)
	{
		impl_.max_vertex_buffer = MAX_VERTEX_BUFFER;
		impl_.max_index_buffer = MAX_INDEX_BUFFER;

		Subscribe(&GUISystem::HandleStartup);
		Subscribe(&GUISystem::HandleInputBegin);
		Subscribe(&GUISystem::HandleInputEnd);
		Subscribe(&GUISystem::HandleSDLRawInput);
		Subscribe(&GUISystem::HandleBeginFrame);
		Subscribe(&GUISystem::HandlePostRenderUpdate);
	}

	GUISystem::~GUISystem()
	{
		nk_font_atlas_clear(&impl_.atlas);
		nk_buffer_free(&impl_.cmds);
		nk_free(&impl_.ctx);

		delete &impl_;
	}
	
	void GUISystem::HandleStartup(const struct Startup& eventData)
	{
		auto& cache = GetSubsystem<ResourceCache>();
		auto& graphics = GetSubsystem<Graphics>();

		vertexBuffer_ = new VertexBuffer();

		PODVector<VertexElement> elements
		{
			VertexElement(TYPE_VECTOR2, SEM_POSITION),
			VertexElement(TYPE_VECTOR2, SEM_TEXCOORD),
			VertexElement(TYPE_UBYTE4_NORM, SEM_COLOR)
		};

		vertexBuffer_->SetSize(MAX_VERTEX_BUFFER, elements, true);

		indexBuffer_ = new IndexBuffer();
		indexBuffer_->SetSize(MAX_INDEX_BUFFER, false, true);

		geometry_ = new Geometry();
		geometry_->SetVertexBuffer(0, vertexBuffer_);
		geometry_->SetIndexBuffer(indexBuffer_);
		geometry_->SetDrawRange(PrimitiveTopology::TRIANGLE_LIST, 0, indexBuffer_->GetIndexCount(), 0, vertexBuffer_->GetVertexCount());

		uiConstants_ = graphics.AddUniformBuffer<UIVS>();

		material_ = new Material();
		material_->SetShaderAttr(ResourceRef::Create<Shader>("shaders/UI.shader"));
		pipeline_ = material_->GetPipeline("base", "");
		pipeline_->SetLineAntialiased(NK_ANTI_ALIASING_ON);

		nk_init_default(&impl_.ctx, 0);
		impl_.ctx.clip.copy = nk_sdl_clipbard_copy;
		impl_.ctx.clip.paste = nk_sdl_clipbard_paste;
		impl_.ctx.clip.userdata = nk_handle_ptr(0);

		nk_buffer_init_default(&impl_.cmds);

		/* Load Fonts: if none of these are loaded a default font will be used  */
		/* Load Cursor: if you uncomment cursor loading please hide the cursor */
		{
			struct nk_font_atlas *atlas;
			FontStashBegin(&atlas);
			struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "CoreData/Fonts/arial.ttf", 14, 0);
			/*struct nk_font *robot = nk_font_atlas_add_from_file(atlas, "../../extra_font/Roboto-Regular.ttf", 14, 0);*/
			/*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
			/*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../extra_font/ProggyClean.ttf", 12, 0);*/
			/*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../extra_font/ProggyTiny.ttf", 10, 0);*/
			/*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../extra_font/Cousine-Regular.ttf", 13, 0);*/
			FontStashEnd();
			/*nk_style_load_all_cursors(ctx, atlas->cursors);*/
			nk_style_set_font(&impl_.ctx, &droid->handle);
		}
	}

	void GUISystem::FontStashBegin(struct nk_font_atlas **atlas)
	{
		nk_font_atlas_init_default(&impl_.atlas);
		nk_font_atlas_begin(&impl_.atlas);
		*atlas = &impl_.atlas;
	}

	void GUISystem::FontStashEnd(void)
	{
		const void *image; int w, h;

		image = nk_font_atlas_bake(&impl_.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
		
		Vector<Vector<byte>> mip(1);	
		Vector<byte>& subData = mip.back();
		subData.resize(w * h * 4);
		std::memcpy(subData.data(), image, subData.size());

		font_texture = new Texture();

		TextureDesc desc;
		desc.Type = Diligent::RESOURCE_DIM_TEX_2D;
		desc.Width = (uint)w;
		desc.Height = (uint)h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = Diligent::TEX_FORMAT_RGBA8_UNORM;
		desc.SampleCount = 1;
		desc.Usage = Diligent::USAGE_DEFAULT;
		desc.BindFlags = Diligent::BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		
		font_texture->Create(desc, std::move(mip));

		nk_font_atlas_end(&impl_.atlas, nk_handle_ptr(font_texture), &impl_.null);
		if (impl_.atlas.default_font)
			nk_style_set_font(&impl_.ctx, &impl_.atlas.default_font->handle);
	}
	
	int	nk_sdl_handle_event(struct nk_context *ctx, SDL_Event *evt)
	{
		if (evt->type == SDL_KEYUP || evt->type == SDL_KEYDOWN) {
			/* key events */
			int down = evt->type == SDL_KEYDOWN;
			const Uint8* state = SDL_GetKeyboardState(0);
			SDL_Keycode sym = evt->key.keysym.sym;
			if (sym == SDLK_RSHIFT || sym == SDLK_LSHIFT)
				nk_input_key(ctx, NK_KEY_SHIFT, down);
			else if (sym == SDLK_DELETE)
				nk_input_key(ctx, NK_KEY_DEL, down);
			else if (sym == SDLK_RETURN)
				nk_input_key(ctx, NK_KEY_ENTER, down);
			else if (sym == SDLK_TAB)
				nk_input_key(ctx, NK_KEY_TAB, down);
			else if (sym == SDLK_BACKSPACE)
				nk_input_key(ctx, NK_KEY_BACKSPACE, down);
			else if (sym == SDLK_HOME) {
				nk_input_key(ctx, NK_KEY_TEXT_START, down);
				nk_input_key(ctx, NK_KEY_SCROLL_START, down);
			}
			else if (sym == SDLK_END) {
				nk_input_key(ctx, NK_KEY_TEXT_END, down);
				nk_input_key(ctx, NK_KEY_SCROLL_END, down);
			}
			else if (sym == SDLK_PAGEDOWN) {
				nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
			}
			else if (sym == SDLK_PAGEUP) {
				nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
			}
			else if (sym == SDLK_z)
				nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
			else if (sym == SDLK_r)
				nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
			else if (sym == SDLK_c)
				nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
			else if (sym == SDLK_v)
				nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
			else if (sym == SDLK_x)
				nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
			else if (sym == SDLK_b)
				nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
			else if (sym == SDLK_e)
				nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
			else if (sym == SDLK_UP)
				nk_input_key(ctx, NK_KEY_UP, down);
			else if (sym == SDLK_DOWN)
				nk_input_key(ctx, NK_KEY_DOWN, down);
			else if (sym == SDLK_LEFT) {
				if (state[SDL_SCANCODE_LCTRL])
					nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
				else nk_input_key(ctx, NK_KEY_LEFT, down);
			}
			else if (sym == SDLK_RIGHT) {
				if (state[SDL_SCANCODE_LCTRL])
					nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
				else nk_input_key(ctx, NK_KEY_RIGHT, down);
			}
			else return 0;
			return 1;
		}
		else if (evt->type == SDL_MOUSEBUTTONDOWN || evt->type == SDL_MOUSEBUTTONUP) {
			/* mouse button */
			int down = evt->type == SDL_MOUSEBUTTONDOWN;
			const int x = evt->button.x, y = evt->button.y;
			if (evt->button.button == SDL_BUTTON_LEFT) {
				if (evt->button.clicks > 1)
					nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
				nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
			}
			else if (evt->button.button == SDL_BUTTON_MIDDLE)
				nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
			else if (evt->button.button == SDL_BUTTON_RIGHT)
				nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
			return 1;
		}
		else if (evt->type == SDL_MOUSEMOTION) {
			/* mouse motion */
			if (ctx->input.mouse.grabbed) {
				int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
				nk_input_motion(ctx, x + evt->motion.xrel, y + evt->motion.yrel);
			}
			else nk_input_motion(ctx, evt->motion.x, evt->motion.y);
			return 1;
		}
		else if (evt->type == SDL_TEXTINPUT) {
			/* text input */
			nk_glyph glyph;
			memcpy(glyph, evt->text.text, NK_UTF_SIZE);
			nk_input_glyph(ctx, glyph);
			return 1;
		}
		else if (evt->type == SDL_MOUSEWHEEL) {
			/* mouse wheel */
			nk_input_scroll(ctx, nk_vec2((float)evt->wheel.x, (float)evt->wheel.y));
			return 1;
		}
		return 0;
	}

	void GUISystem::HandleInputBegin(const struct InputBegin& eventData)
	{
		nk_input_begin(&impl_.ctx);
	}

	void GUISystem::HandleInputEnd(const struct InputEnd& eventData)
	{
		nk_input_end(&impl_.ctx);
	}

	void GUISystem::HandleSDLRawInput(const SDLRawInput& eventData)
	{	
		int ret = nk_sdl_handle_event(&impl_.ctx, eventData.sdlEvent_);
		const_cast<SDLRawInput&>(eventData).consumed_ = (ret != 0);
	}

	nk_color background = nk_rgb(28, 48, 62);
	void GUISystem::HandleBeginFrame(const BeginFrame& eventData)
	{
		SendEvent(GUI());
		/*
		nk_context *ctx = &impl_.ctx;

		if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
		{
			enum { EASY, HARD };
			static int op = EASY;
			static int property = 20;

			nk_layout_row_static(ctx, 30, 80, 1);
			if (nk_button_label(ctx, "button"))
				fprintf(stdout, "button pressed\n");
			nk_layout_row_dynamic(ctx, 30, 2);
			if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
			if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
			nk_layout_row_dynamic(ctx, 22, 1);
			nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

			nk_layout_row_dynamic(ctx, 20, 1);
			nk_label(ctx, "background:", NK_TEXT_LEFT);
			nk_layout_row_dynamic(ctx, 25, 1);
			if (nk_combo_begin_color(ctx, background, nk_vec2(nk_widget_width(ctx), 400))) {
				nk_layout_row_dynamic(ctx, 120, 1);
				background = nk_color_picker(ctx, background, NK_RGBA);
				nk_layout_row_dynamic(ctx, 25, 1);
				background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1, 1);
				background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1, 1);
				background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1, 1);
				background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1, 1);
				nk_combo_end(ctx);
			}
		}
		nk_end(ctx);*/
	}

	static void
		nk_get_projection_matrix(int width, int height, float *result)
	{
		const float L = 0.0f;
		const float R = (float)width;
		const float T = 0.0f;
		const float B = (float)height;
		float matrix[4][4] =
		{
			{ 2.0f / (R - L),	0.0f,				0.0f, 0.0f },
			{ 0.0f,				2.0f / (T - B),		0.0f, 0.0f },
			{ 0.0f,             0.0f,				0.5f, 0.0f },
			{ (R + L) / (L - R),(T + B) / (B - T),	0.5f, 1.0f },
		};
		memcpy(result, matrix, sizeof(matrix));
	}

	void GUISystem::HandlePostRenderUpdate(const struct PostRenderUpdate& eventData)
	{
		auto& renderer = GetSubsystem<Renderer>();
		auto& graphics = GetSubsystem<Graphics>();
		/* Convert from command queue into draw list and draw to screen */
		{/* load draw vertices & elements directly into vertex + element buffer */
			const struct nk_draw_command *cmd;
 			uint offset = 0;
			void* vertices = vertexBuffer_->Lock();
			void* indices = indexBuffer_->Lock();

			{/* fill converting configuration */
				struct nk_convert_config config;
				NK_STORAGE const struct nk_draw_vertex_layout_element vertex_layout[] = {
					{ NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_vertex, position) },
					{ NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_vertex, uv) },
					{ NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_vertex, col) },
					{ NK_VERTEX_LAYOUT_END }
				};
				memset(&config, 0, sizeof(config));
				config.vertex_layout = vertex_layout;
				config.vertex_size = sizeof(struct nk_vertex);
				config.vertex_alignment = NK_ALIGNOF(struct nk_vertex);
				config.global_alpha = 1.0f;
				config.shape_AA = NK_ANTI_ALIASING_ON;
				config.line_AA = NK_ANTI_ALIASING_ON;
				config.circle_segment_count = 22;
				config.curve_segment_count = 22;
				config.arc_segment_count = 22;
				config.null = impl_.null;

				{/* setup buffers to load vertices and elements */
					struct nk_buffer vbuf, ibuf;
					nk_buffer_init_fixed(&vbuf, vertices, (size_t)impl_.max_vertex_buffer);
					nk_buffer_init_fixed(&ibuf, indices, (size_t)impl_.max_index_buffer);
					nk_convert(&impl_.ctx, &impl_.cmds, &vbuf, &ibuf, &config); 
				}
			}
			vertexBuffer_->Unlock();
			indexBuffer_->Unlock();

			Batch batch(geometry_, material_, &Matrix3x4::IDENTITY);
			batch.SetDrawRange(PrimitiveTopology::TRIANGLE_LIST, 0, -1);
			batch.pipelineState_ = pipeline_;

			UIVS* ui = (UIVS*)uiConstants_->Lock();
			nk_get_projection_matrix(graphics.GetWidth(), graphics.GetHeight(), &ui->UIProj.m00_);
			uiConstants_->Unlock();
			/* iterate over and execute each draw command */
			nk_draw_foreach(cmd, &impl_.ctx, &impl_.cmds)
			{
				if (!cmd->elem_count)
					continue;

				material_->SetTexture("DiffMap", (Texture*)cmd->texture.ptr);

				batch.scissor_ = renderer.CacheScissor(
					(int)cmd->clip_rect.x, (int)cmd->clip_rect.y,
					(int)(cmd->clip_rect.x + cmd->clip_rect.w),					
					(int)(cmd->clip_rect.y + cmd->clip_rect.h));
				batch.indexOffset_ = offset;
				batch.indexCount_ = cmd->elem_count;
				renderer.AddBatch(batch);
				offset += cmd->elem_count;
			}

			nk_clear(&impl_.ctx); }
	}
}
