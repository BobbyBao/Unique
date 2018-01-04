#include "UniquePCH.h"
#include "ImGUI.h"
#include "Graphics/Geometry.h"

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

#define MAX_VERTEX_BUFFER 32 * 1024
#define MAX_INDEX_BUFFER 8 * 1024

	GUISystem::GUISystem() : impl_(*new NkImpl)
	{
		impl_.max_vertex_buffer = MAX_VERTEX_BUFFER;
		impl_.max_index_buffer = MAX_INDEX_BUFFER;

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

		nk_init_default(&impl_.ctx, 0);
		//impl_.ctx.clip.copy = nk_d3d11_clipbard_copy;
		//impl_.ctx.clip.paste = nk_d3d11_clipbard_paste;
		impl_.ctx.clip.userdata = nk_handle_ptr(0);

		nk_buffer_init_default(&impl_.cmds);

		/* Load Fonts: if none of these are loaded a default font will be used  */
		/* Load Cursor: if you uncomment cursor loading please hide the cursor */
		{
			struct nk_font_atlas *atlas;
			FontStashBegin(&atlas);
			/*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../extra_font/DroidSans.ttf", 14, 0);*/
			/*struct nk_font *robot = nk_font_atlas_add_from_file(atlas, "../../extra_font/Roboto-Regular.ttf", 14, 0);*/
			/*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
			/*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../extra_font/ProggyClean.ttf", 12, 0);*/
			/*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../extra_font/ProggyTiny.ttf", 10, 0);*/
			/*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../extra_font/Cousine-Regular.ttf", 13, 0);*/
			FontStashEnd();
			/*nk_style_load_all_cursors(ctx, atlas->cursors);*/
			/*nk_style_set_font(ctx, &droid->handle)*/;
		}

		Subscribe(&GUISystem::HandleBeginFrame);
	}

	GUISystem::~GUISystem()
	{
		nk_font_atlas_clear(&impl_.atlas);
		nk_buffer_free(&impl_.cmds);
		nk_free(&impl_.ctx);

		delete &impl_;
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
		/*
		{ID3D11Texture2D *font_texture;
		HRESULT hr;

		D3D11_TEXTURE2D_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.Width = (UINT)w;
		desc.Height = (UINT)h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		{D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = image;
		data.SysMemPitch = (UINT)(w * 4);
		data.SysMemSlicePitch = 0;
		hr = ID3D11Device_CreateTexture2D(d3d11.device, &desc, &data, &font_texture);
		assert(SUCCEEDED(hr)); }

		{D3D11_SHADER_RESOURCE_VIEW_DESC srv;
		memset(&srv, 0, sizeof(srv));
		srv.Format = desc.Format;
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels = 1;
		srv.Texture2D.MostDetailedMip = 0;
		hr = ID3D11Device_CreateShaderResourceView(d3d11.device, (ID3D11Resource *)font_texture, &srv, &d3d11.font_texture_view);
		assert(SUCCEEDED(hr)); }
		ID3D11Texture2D_Release(font_texture); }

		nk_font_atlas_end(&impl_.atlas, nk_handle_ptr(d3d11.font_texture_view), &impl_.null);
		if (impl_.atlas.default_font)
			nk_style_set_font(&impl_.ctx, &impl_.atlas.default_font->handle);
		*/
	}

	void GUISystem::HandleBeginFrame(const struct BeginFrame& eventData)
	{
		nk_context *ctx = &impl_.ctx;
		nk_color background = nk_rgb(28, 48, 62);

		nk_input_begin(ctx);

		nk_input_end(ctx);

		/* GUI */
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
		nk_end(ctx);
	}

	void GUISystem::HandlePostRenderUpdate(const struct PostRenderUpdate& eventData)
	{
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

			/* iterate over and execute each draw command */
			nk_draw_foreach(cmd, &impl_.ctx, &impl_.cmds)
			{
// 				D3D11_RECT scissor;
// 				ID3D11ShaderResourceView *texture_view = (ID3D11ShaderResourceView *)cmd->texture.ptr;
				if (!cmd->elem_count) continue;

				Diligent::Rect scissor;
				scissor.left = (int)cmd->clip_rect.x;
 				scissor.right = (int)(cmd->clip_rect.x + cmd->clip_rect.w);
 				scissor.top = (int)cmd->clip_rect.y;
 				scissor.bottom = (int)(cmd->clip_rect.y + cmd->clip_rect.h);

// 				ID3D11DeviceContext_PSSetShaderResources(context, 0, 1, &texture_view);
// 				ID3D11DeviceContext_RSSetScissorRects(context, 1, &scissor);
// 				ID3D11DeviceContext_DrawIndexed(context, (UINT)cmd->elem_count, offset, 0);
				offset += cmd->elem_count;
			}
			nk_clear(&impl_.ctx); }
	}
}
