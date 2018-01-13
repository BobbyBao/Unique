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

        public static void Begin(string title, nk_rect bounds, nk_panel_flags flags)
        {
            nk_begin(ctx, title, bounds, (uint)flags);
        }

        public static void End()
        {
            nk_end(ctx);
        }

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

        public static bool Window(string Title, float X, float Y, float W, float H, nk_panel_flags Flags, Action A) => Window(Title, Title, X, Y, W, H, Flags, A);

        public static bool WindowIsClosed(string Name) => nk_window_is_closed(ctx, Name) != 0;

        public static bool WindowIsHidden(string Name) => nk_window_is_hidden(ctx, Name) != 0;

        public static bool WindowIsCollapsed(string Name) => nk_window_is_collapsed(ctx, Name) != 0;

        public static bool ButtonLabel(string Label)
        {
            return nk_button_label(ctx, Label) != 0;
        }

        public static bool ButtonText(string Text)
        {
            return nk_button_text(ctx, Text, Text.Length) != 0;
        }

        public static bool ButtonText(char Char) => ButtonText(Char.ToString());

        public static void LayoutRowStatic(float Height, int ItemWidth, int Cols)
        {
            nk_layout_row_static(ctx, Height, ItemWidth, Cols);
        }

        public static void LayoutRowDynamic(float Height = 0, int Cols = 1)
        {
            nk_layout_row_dynamic(ctx, Height, Cols);
        }

        public static nk_rect WindowGetBounds()
        {
            return nk_window_get_bounds(ctx);
        }
        
        public static int Combo(string[] items, int selected, int item_height, nk_vec2 size)
        {
            return nk_combo(ctx, items, items.Length, selected, item_height, size);
        }

    }
}
