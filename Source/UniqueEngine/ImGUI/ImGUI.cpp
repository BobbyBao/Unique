#include "UniquePCH.h"
#include "ImGUI.h"

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


struct nk_vertex {
	float position[2];
	float uv[2];
	nk_byte col[4];
};

namespace Unique
{
	struct Impl
	{
		nk_context ctx;
		nk_font_atlas atlas;
		nk_buffer cmds;
		nk_draw_null_texture null;
		unsigned int max_vertex_buffer;
		unsigned int max_index_buffer;
	};


#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024

	GUISystem::GUISystem() : impl_(*new Impl)
	{
		impl_.max_vertex_buffer = MAX_VERTEX_BUFFER;
		impl_.max_index_buffer = MAX_INDEX_BUFFER;

		nk_init_default(&impl_.ctx, 0);
		//impl_.ctx.clip.copy = nk_d3d11_clipbard_copy;
		//impl_.ctx.clip.paste = nk_d3d11_clipbard_paste;
		impl_.ctx.clip.userdata = nk_handle_ptr(0);

		nk_buffer_init_default(&impl_.cmds);

		Subscribe(&GUISystem::HandleBeginFrame);
	}

	GUISystem::~GUISystem()
	{
		delete &impl_;
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
}
