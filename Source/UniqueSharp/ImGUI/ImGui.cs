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
        public static bool WindowIsClosed(string name) => nk_window_is_closed(ctx, name) != 0;
        public static bool WindowIsHidden(string name) => nk_window_is_hidden(ctx, name) != 0;
        public static bool WindowIsCollapsed(string name) => nk_window_is_collapsed(ctx, name) != 0;
        public static bool ButtonLabel(string label) => nk_button_label(ctx, label) != 0;
        public static bool ButtonText(string text) => nk_button_text(ctx, text, text.Length) != 0;
        public static bool ButtonText(char c) => ButtonText(c.ToString());
        public static void LayoutRowStatic(float height, int itemWidth, int cols) => nk_layout_row_static(ctx, height, itemWidth, cols);
        public static void LayoutRowDynamic(float height = 0, int cols = 1) => nk_layout_row_dynamic(ctx, height, cols);
        public static nk_rect WindowGetBounds() => nk_window_get_bounds(ctx);        
        public static int Combo(string[] items, int selected, int item_height, nk_vec2 size) => nk_combo(ctx, items, items.Length, selected, item_height, size);

    }
}
