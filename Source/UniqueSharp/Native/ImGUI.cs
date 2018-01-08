using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    public enum nk_panel_flags
    {
        NK_WINDOW_BORDER = (1 << 0), /* Draws a border around the window to visually separate window from the background */
        NK_WINDOW_MOVABLE = (1 << 1), /* The movable flag indicates that a window can be moved by user input or by dragging the window header */
        NK_WINDOW_SCALABLE = (1 << 2), /* The scalable flag indicates that a window can be scaled by user input by dragging a scaler icon at the button of the window */
        NK_WINDOW_CLOSABLE = (1 << 3), /* adds a closable icon into the header */
        NK_WINDOW_MINIMIZABLE = (1 << 4), /* adds a minimize icon into the header */
        NK_WINDOW_NO_SCROLLBAR = (1 << 5), /* Removes the scrollbar from the window */
        NK_WINDOW_TITLE = (1 << 6), /* Forces a header at the top at the window showing the title */
        NK_WINDOW_SCROLL_AUTO_HIDE = (1 << 7), /* Automatically hides the window scrollbar if no user interaction: also requires delta time in `nk_context` to be set each frame */
        NK_WINDOW_BACKGROUND = (1 << 8), /* Always keep window in the background */
        NK_WINDOW_SCALE_LEFT = (1 << 9), /* Puts window scaler in the left-ottom corner instead right-bottom*/
        NK_WINDOW_NO_INPUT = (1 << 10) /* Prevents window of scaling, moving or getting focus */
    };

    public struct nk_color { byte r, g, b, a; }
    public struct nk_colorf { float r, g, b, a; }
    public struct nk_vec2 { float x, y; }
    public struct nk_vec2i { short x, y; }
    public struct nk_rect { float x, y, w, h; }
    public struct nk_recti { short x, y, w, h; }

    //typedef char nk_glyph[NK_UTF_SIZE];
    // typedef union {void *ptr; int id; }
    // nk_handle;
    public unsafe struct nk_image
    {
        IntPtr handle;
        ushort w, h;
        fixed ushort region[4];
    };

    public struct nk_cursor
    {
        nk_image img;
        nk_vec2 size, offset;
    };
    public struct nk_scroll { uint x, y; };

    public enum nk_heading { NK_UP, NK_RIGHT, NK_DOWN, NK_LEFT };
    public enum nk_button_behavior { NK_BUTTON_DEFAULT, NK_BUTTON_REPEATER };
    public enum nk_modify { NK_FIXED = 0, NK_MODIFIABLE = 1 };
    public enum nk_orientation { NK_VERTICAL, NK_HORIZONTAL };
    public enum nk_collapse_states { NK_MINIMIZED = 0, NK_MAXIMIZED = 1 };
    public enum nk_show_states { NK_HIDDEN = 0, NK_SHOWN = 1 };
    public enum nk_chart_type { NK_CHART_LINES, NK_CHART_COLUMN, NK_CHART_MAX };
    public enum nk_chart_event { NK_CHART_HOVERING = 0x01, NK_CHART_CLICKED = 0x02 };
    public enum nk_color_format { NK_RGB, NK_RGBA };
    public enum nk_popup_type { NK_POPUP_STATIC, NK_POPUP_DYNAMIC };
    public enum nk_layout_format { NK_DYNAMIC, NK_STATIC };
    public enum nk_tree_type { NK_TREE_NODE, NK_TREE_TAB };

    [SuppressUnmanagedCodeSecurity]
    public unsafe static partial class ImGui
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_begin(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string title, nk_rect bounds, uint flags);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_begin_titled(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, [MarshalAs(UnmanagedType.LPStr)]string title, nk_rect bounds, uint flags);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_end(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr nk_window_find(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_window_get_bounds(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_position(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_size(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_window_get_width(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_window_get_height(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr nk_window_get_panel(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_window_get_content_region(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_content_region_min(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_content_region_max(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_content_region_size(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr nk_window_get_canvas(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_has_focus(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_collapsed(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_closed(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_hidden(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_active(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_hovered(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_any_hovered(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_item_is_any_active(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_set_bounds(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_rect bounds);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_set_position(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_vec2 pos);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_set_size(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_vec2 sz);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_set_focus(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_close(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_collapse(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_collapse_states state);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_collapse_if(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_collapse_states state, int cond);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_show(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_show_states state);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_show_if(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_show_states state, int cond);


    }
}
