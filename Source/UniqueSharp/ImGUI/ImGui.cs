using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    public static partial class ImGUI
    {
        static IntPtr ctx_;
        static IntPtr ctx
        {
            get
            {
                if(ctx_ == IntPtr.Zero)
                {
                    ctx_ = nk_ctx();
                }

                return ctx_;
            }

        }

        public static void SetStyle(Theme theme) => nk_set_style(ctx, theme);
        public static bool Begin(string title, nk_rect bounds, nk_panel_flags flags) => nk_begin(ctx, title, bounds, (uint)flags) != 0;
        public static int BeginTitled(string name, string title, nk_rect bounds, nk_panel_flags flags) => nk_begin_titled(ctx, name, title, bounds, (uint)flags);
        public static void End() => nk_end(ctx);

        public static bool Window(string name, string title, float x, float y, float w, float h, nk_panel_flags flags, Action a)
        {
            bool res = true;

            if(nk_begin_titled(ctx, name, title, new nk_rect(x, y, w, h), (uint)flags) != 0)
                a?.Invoke();
            else
                res = false;

            nk_end(ctx);
            return res;
        }

        public static bool Window(string title, float x, float y, float w, float h, nk_panel_flags flags, Action a) => Window(title, title, x, y, w, h, flags, a);
        public static IntPtr WindowFind(string name) => nk_window_find(ctx, name);
        public static nk_rect WindowGetBounds() => nk_window_get_bounds(ctx);        
        public static nk_vec2 WindowGetPosition() => nk_window_get_position(ctx);
        public static nk_vec2 WindowGetSize() => nk_window_get_size(ctx);
        public static float WindowGetWidth() => nk_window_get_width(ctx);
        public static float WindowGetHeight() => nk_window_get_height(ctx);
        public static IntPtr WindowGetPanel() => nk_window_get_panel(ctx);
        public static nk_rect WindowGetContentRegion() => nk_window_get_content_region(ctx);
        public static nk_vec2 WindowGetContentRegionMin() => nk_window_get_content_region_min(ctx);
        public static nk_vec2 WindowGetContentRegionMax() => nk_window_get_content_region_max(ctx);
        public static nk_vec2 WindowGetContentRegionSize() => nk_window_get_content_region_size(ctx);
        public static IntPtr WindowGetCanvas() => nk_window_get_canvas(ctx);
        public static bool WindowHasFocus() => nk_window_has_focus(ctx) != 0;
        public static bool WindowIsCollapsed(string name) => nk_window_is_collapsed(ctx, name) != 0;
        public static bool WindowIsClosed(string name) => nk_window_is_closed(ctx, name) != 0;
        public static bool WindowIsHidden(string name) => nk_window_is_hidden(ctx, name)!= 0;
        public static bool WindowIsActive(string name) => nk_window_is_active(ctx, name) != 0;
        public static bool WindowIsHoverd(string name) => nk_window_is_hovered(ctx) != 0;
        public static bool WindowIsAnyHovered(string name) => nk_window_is_any_hovered(ctx) != 0;
        public static bool WindowIsAnyActive(string name) => nk_item_is_any_active(ctx) != 0;
        public static void WindowSetBounds(string name, nk_rect bounds) => nk_window_set_bounds(ctx, name, bounds);
        public static void WindowSetPosition(string name, nk_vec2 pos) => nk_window_set_position(ctx, name, pos);
        public static void WindowSetSize(string name, nk_vec2 sz) => nk_window_set_size(ctx, name, sz);
        public static void WindowSetFocus(string name) => nk_window_set_focus(ctx, name);
        public static void WindowClose(string name) => nk_window_close(ctx, name);
        public static void WindowCollapse(string name, nk_collapse_states state) => nk_window_collapse(ctx, name, state);
        public static void WindowCollapseIf(string name, nk_collapse_states state, int cond) => nk_window_collapse_if(ctx, name, state, cond);
        public static void WindowShow(string name, nk_show_states state) => nk_window_show(ctx, name, state);
        public static void WindowShowIf(string name, nk_show_states state, int cond) => nk_window_show_if(ctx, name, state, cond);

        public static void LayoutSetMinRowHeight(float height) => nk_layout_set_min_row_height(ctx, height);
        public static void LayoutResetMinRowHeight(float height) => nk_layout_reset_min_row_height(ctx);
        public static nk_rect LayoutWidgetBounds() => nk_layout_widget_bounds(ctx);
        public static float LayoutRatioFromPixel(float pixel_width) => nk_layout_ratio_from_pixel(ctx, pixel_width);
        public static void LayoutRowDynamic(float height, int cols = 1) => nk_layout_row_dynamic(ctx, height, cols);
#if false
        public static void nk_layout_row_static(IntPtr ctx, float height, int item_width, int cols);
        public static void nk_layout_row_begin(IntPtr ctx, nk_layout_format fmt, float row_height, int cols);
        public static void nk_layout_row_push(IntPtr ctx, float value);
        public static void nk_layout_row_end(IntPtr ctx);
        public static void nk_layout_row(IntPtr ctx, nk_layout_format fmt, float height, int cols, IntPtr ratio);
        public static void nk_layout_row_template_begin(IntPtr ctx, float row_height);
        public static void nk_layout_row_template_push_dynamic(IntPtr ctx);
        public static void nk_layout_row_template_push_variable(IntPtr ctx, float min_width);
        public static void nk_layout_row_template_push_static(IntPtr ctx, float width);
        public static void nk_layout_row_template_end(IntPtr ctx);
        public static void nk_layout_space_begin(IntPtr ctx, nk_layout_format fmt, float height, int widget_count);
        public static void nk_layout_space_push(IntPtr ctx, nk_rect rect);
        public static void nk_layout_space_end(IntPtr ctx);
        public static nk_rect nk_layout_space_bounds(IntPtr ctx);
        public static nk_vec2 nk_layout_space_to_screen(IntPtr ctx, nk_vec2 pos);
        public static nk_vec2 nk_layout_space_to_local(IntPtr ctx, nk_vec2 pos);
        public static nk_rect nk_layout_space_rect_to_screen(IntPtr ctx, nk_rect rect);
        public static nk_rect nk_layout_space_rect_to_local(IntPtr ctx, nk_rect rect);
#endif

        public static bool ButtonLabel(string label) => nk_button_label(ctx, label) != 0;
        public static bool ButtonText(string text) => nk_button_text(ctx, text, text.Length) != 0;
        public static bool ButtonText(char c) => ButtonText(c.ToString());
        public static void LayoutRowStatic(float height, int itemWidth, int cols) => nk_layout_row_static(ctx, height, itemWidth, cols);
        //public static void LayoutRowDynamic(float height = 0, int cols = 1) => nk_layout_row_dynamic(ctx, height, cols);
        //public static nk_rect WindowGetBounds() => nk_window_get_bounds(ctx);        
        public static int Combo(string[] items, int selected, int item_height, nk_vec2 size) => nk_combo(ctx, items, items.Length, selected, item_height, size);

        

        public static void MenubarBegin() => nk_menubar_begin(ctx);
        public static void MenubarEnd() => nk_menubar_end(ctx);
        public static bool MenuBeginText(string title, int title_len, uint align, nk_vec2 size) => nk_menu_begin_text(ctx, title, title_len, align, size) != 0;
        public static bool MenuBeginLabel(string label, uint align, nk_vec2 size) => nk_menu_begin_label(ctx, label, align, size) != 0;
        public static bool MenuBeginImage(string label, nk_image image, nk_vec2 size) => nk_menu_begin_image(ctx, label, image, size) != 0;
        public static bool MenuBeginImageText(string label, int p, uint align, nk_image image, nk_vec2 size) => nk_menu_begin_image_text(ctx, label, p, align, image, size) != 0;
        /*
        public static int nk_menu_begin_image_label(IntPtr ctx, string label, uint align, nk_image image, nk_vec2 size);
        public static int nk_menu_begin_symbol(IntPtr ctx, string label, nk_symbol_type symbol, nk_vec2 size);
        public static int nk_menu_begin_symbol_text(IntPtr ctx, string text, int p1, uint align, nk_symbol_type symbol, nk_vec2 size);
        public static int nk_menu_begin_symbol_label(IntPtr ctx, string label, uint align, nk_symbol_type symbol, nk_vec2 size);
        */


        public static void MenuItemText(string text, int p1, uint align) => nk_menu_item_text(ctx, text, p1, align);
        /*
        public static int nk_menu_item_text(IntPtr ctx, string text, int p1, uint align);
        public static int nk_menu_item_label(IntPtr ctx, string label, uint alignment);
        public static int nk_menu_item_image_label(IntPtr ctx, nk_image image, string label, uint alignment);
        public static int nk_menu_item_image_text(IntPtr ctx, nk_image image, string text, int len, uint alignment);
        public static int nk_menu_item_symbol_text(IntPtr ctx, nk_symbol_type symol, string text, int p1, uint alignment);
        public static int nk_menu_item_symbol_label(IntPtr ctx, nk_symbol_type symol, string label, uint alignment);
        */
        public static void MenuClose() => nk_menu_close(ctx);
        public static void MenuEnd() => nk_menu_end(ctx);
















    }
}
