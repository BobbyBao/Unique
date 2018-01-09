using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    public enum nk_buttons
    {
        NK_BUTTON_LEFT,
        NK_BUTTON_MIDDLE,
        NK_BUTTON_RIGHT,
        NK_BUTTON_DOUBLE,
        NK_BUTTON_MAX
    };

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
    }
    
    public struct nk_color { public byte r, g, b, a; }
    public struct nk_colorf { public float r, g, b, a; }
    public struct nk_vec2 { public float x, y; }
    public struct nk_vec2i { public short x, y; }
    public struct nk_rect { public float x, y, w, h; }
    public struct nk_recti { public short x, y, w, h; }
        public float x, y, w, h;
        public nk_rect(float x, float y, float w, float h)
        {
            this.x = x;
            this.y = y;
            this.w = w;
            this.h = h;
        }
    }

    public struct nk_recti { public short x, y, w, h; }

    //typedef char nk_glyph[NK_UTF_SIZE];
    // typedef union {void *ptr; int id; }
    // nk_handle;
    public unsafe struct nk_image
    {
        public IntPtr handle;
        public ushort w, h;
        public fixed ushort region[4];
    };

    public struct nk_cursor
    {
        public nk_image img;
        public nk_vec2 size, offset;
    };

    public struct nk_scroll { public uint x, y; };

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

    public enum nk_symbol_type
    {
        NK_SYMBOL_NONE,
        NK_SYMBOL_X,
        NK_SYMBOL_UNDERSCORE,
        NK_SYMBOL_CIRCLE_SOLID,
        NK_SYMBOL_CIRCLE_OUTLINE,
        NK_SYMBOL_RECT_SOLID,
        NK_SYMBOL_RECT_OUTLINE,
        NK_SYMBOL_TRIANGLE_UP,
        NK_SYMBOL_TRIANGLE_DOWN,
        NK_SYMBOL_TRIANGLE_LEFT,
        NK_SYMBOL_TRIANGLE_RIGHT,
        NK_SYMBOL_PLUS,
        NK_SYMBOL_MINUS,
        NK_SYMBOL_MAX
    }


    [SuppressUnmanagedCodeSecurity]
    public unsafe static partial class ImGUI
    {
        /*  nk_begin - starts a new window; needs to be called every frame for every window (unless hidden) or otherwise the window gets removed
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @title window title and identifier. Needs to be persistent over frames to identify the window
         *      @bounds initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame
         *      @flags window flags defined in `enum nk_panel_flags` with a number of different window behaviors
         *  Return values:
         *      returns 1 if the window can be filled up with widgets from this point until `nk_end or 0 otherwise for example if minimized `*/
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr nk_ctx();
       
        /*  nk_begin - starts a new window; needs to be called every frame for every window (unless hidden) or otherwise the window gets removed
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @title window title and identifier. Needs to be persistent over frames to identify the window
         *      @bounds initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame
         *      @flags window flags defined in `enum nk_panel_flags` with a number of different window behaviors
         *  Return values:
                 *      returns 1 if the window can be filled up with widgets from this point until `nk_end or 0 otherwise for example if minimized `*/
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_begin(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string title, nk_rect bounds, uint flags);

        /*  nk_begin_titled - extended window start with separated title and identifier to allow multiple windows with same name but not title
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name window identifier. Needs to be persistent over frames to identify the window
         *      @title window title displayed inside header if flag `NK_WINDOW_TITLE` or either `NK_WINDOW_CLOSABLE` or `NK_WINDOW_MINIMIZED` was set
         *      @bounds initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame
         *      @flags window flags defined in `enum nk_panel_flags` with a number of different window behaviors
         *  Return values:
         *      returns 1 if the window can be filled up with widgets from this point until `nk_end or 0 otherwise `*/
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_begin_titled(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, [MarshalAs(UnmanagedType.LPStr)]string title, nk_rect bounds, uint flags);

        /*  nk_end - needs to be called at the end of the window building process to process scaling, scrollbars and general cleanup.
         *  All widget calls after this functions will result in asserts or no state changes
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_end(IntPtr ctx);

        /*  nk_window_find - finds and returns the window with give name
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name window identifier
         *  Return values:
         *      returns a `nk_window` struct pointing to the identified window or 0 if no window with given name was found */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr nk_window_find(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        /*  nk_window_get_bounds - returns a rectangle with screen position and size of the currently processed window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns a `nk_rect` struct with window upper left position and size */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_window_get_bounds(IntPtr ctx);

        /*  nk_window_get_position - returns the position of the currently processed window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns a `nk_vec2` struct with window upper left position */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_position(IntPtr ctx);

        /*  nk_window_get_size - returns the size with width and height of the currently processed window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns a `nk_vec2` struct with window size */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_size(IntPtr ctx);

        /*  nk_window_get_width - returns the width of the currently processed window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns the window width */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_window_get_width(IntPtr ctx);

        /*  nk_window_get_height - returns the height of the currently processed window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns the window height */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_window_get_height(IntPtr ctx);

        /*  nk_window_get_panel - returns the underlying panel which contains all processing state of the current window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns a pointer to window internal `nk_panel` state. DO NOT keep this pointer around it is only valid until `nk_end` */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr nk_window_get_panel(IntPtr ctx);

        /*  nk_window_get_content_region - returns the position and size of the currently visible and non-clipped space inside the currently processed window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns `nk_rect` struct with screen position and size (no scrollbar offset) of the visible space inside the current window */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_window_get_content_region(IntPtr ctx);

        /*  nk_window_get_content_region_min - returns the upper left position of the currently visible and non-clipped space inside the currently processed window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns `nk_vec2` struct with  upper left screen position (no scrollbar offset) of the visible space inside the current window */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_content_region_min(IntPtr ctx);

        /*  nk_window_get_content_region_max - returns the lower right screen position of the currently visible and non-clipped space inside the currently processed window.
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns `nk_vec2` struct with lower right screen position (no scrollbar offset) of the visible space inside the current window */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_content_region_max(IntPtr ctx);

        /*  nk_window_get_content_region_size - returns the size of the currently visible and non-clipped space inside the currently processed window
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns `nk_vec2` struct with size the visible space inside the current window */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_window_get_content_region_size(IntPtr ctx);

        /*  nk_window_get_canvas - returns the draw command buffer. Can be used to draw custom widgets
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns a pointer to window internal `nk_command_buffer` struct used as drawing canvas. Can be used to do custom drawing */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr nk_window_get_canvas(IntPtr ctx);

        /*  nk_window_has_focus - returns if the currently processed window is currently active
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns 0 if current window is not active or 1 if it is */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_has_focus(IntPtr ctx);
        
        /*  nk_window_is_collapsed - returns if the window with given name is currently minimized/collapsed
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of window you want to check is collapsed
         *  Return values:
         *      returns 1 if current window is minimized and 0 if window not found or is not minimized */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_collapsed(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        /*  nk_window_is_closed - returns if the window with given name was closed by calling `nk_close`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of window you want to check is closed
         *  Return values:
         *      returns 1 if current window was closed or 0 window not found or not closed */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_closed(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);
       
        /*  nk_window_is_hidden - returns if the window with given name is hidden
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of window you want to check is hidden
         *  Return values:
         *      returns 1 if current window is hidden or 0 window not found or visible */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_hidden(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        /*  nk_window_is_active - same as nk_window_has_focus for some reason
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of window you want to check is hidden
         *  Return values:
         *      returns 1 if current window is active or 0 window not found or not active */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_active(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        /*  nk_window_is_hovered - return if the current window is being hovered
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns 1 if current window is hovered or 0 otherwise */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_hovered(IntPtr ctx);
       
        /*  nk_window_is_any_hovered - returns if the any window is being hovered
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns 1 if any window is hovered or 0 otherwise */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_window_is_any_hovered(IntPtr ctx);

        /*  nk_item_is_any_active - returns if the any window is being hovered or any widget is currently active.
         *  Can be used to decide if input should be processed by UI or your specific input handling.
         *  Example could be UI and 3D camera to move inside a 3D space.
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *  Return values:
         *      returns 1 if any window is hovered or any item is active or 0 otherwise */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_item_is_any_active(IntPtr ctx);
       
        /*  nk_window_set_bounds - updates position and size of the currently processed window
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to modify both position and size
         *      @bounds points to a `nk_rect` struct with the new position and size of currently active window */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_set_bounds(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_rect bounds);

        /*  nk_window_set_position - updates position of the currently processed window
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to modify position of
         *      @pos points to a `nk_vec2` struct with the new position of currently active window */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_set_position(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_vec2 pos);

        /*  nk_window_set_size - updates size of the currently processed window
         *  IMPORTANT: only call this function between calls `nk_begin_xxx` and `nk_end`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to modify size of
         *      @size points to a `nk_vec2` struct with the new size of currently active window */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_set_size(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_vec2 sz);

        /*  nk_window_set_focus - sets the window with given name as active
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to be set active */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_set_focus(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);

        /*  nk_window_close - closed a window and marks it for being freed at the end of the frame
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to be closed */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_close(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name);
        
        /*  nk_window_collapse - updates collapse state of a window with given name
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to be either collapse or maximize */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_collapse(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_collapse_states state);

        /*  nk_window_collapse - updates collapse state of a window with given name if given condition is met
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to be either collapse or maximize
         *      @state the window should be put into
         *      @condition that has to be true to actually commit the collapse state change */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_collapse_if(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_collapse_states state, int cond);

        /*  nk_window_show - updates visibility state of a window with given name
         *      Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to be either collapse or maximize
         *      @state with either visible or hidden to modify the window with */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_show(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_show_states state);
        
        /*  nk_window_show_if - updates visibility state of a window with given name if a given condition is met
         *      Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @name of the window to be either collapse or maximize
         *      @state with either visible or hidden to modify the window with
         *      @condition that has to be true to actually commit the visible state change */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_window_show_if(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, nk_show_states state, int cond);


        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_set_min_row_height(IntPtr ctx, float height);
        
        /*  nk_layout_reset_min_row_height - Reset the currently used minimum row height
         *  back to font height + text padding + additional padding (style_window.min_row_height_padding)
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_begin_xxx` */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_reset_min_row_height(IntPtr ctx);
        
        /*  nk_layout_widget_bounds - returns the width of the next row allocate by one of the layouting functions
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_layout_widget_bounds(IntPtr ctx);
        
        /*  nk_layout_ratio_from_pixel - utility functions to calculate window ratio from pixel size
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context`
         *      @pixel_width to convert to window ratio */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_layout_ratio_from_pixel(IntPtr ctx, float pixel_width);
        
        /*  nk_layout_row_dynamic - Sets current row layout to share horizontal space
         *  between @cols number of widgets evenly. Once called all subsequent widget
         *  calls greater than @cols will allocate a new row with same layout.
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_begin_xxx`
         *      @row_height holds height of each widget in row or zero for auto layouting
         *      @cols number of widget inside row */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_dynamic(IntPtr ctx, float height, int cols);
        
        /*  nk_layout_row_static - Sets current row layout to fill @cols number of widgets
         *  in row with same @item_width horizontal size. Once called all subsequent widget
         *  calls greater than @cols will allocate a new row with same layout.
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_begin_xxx`
         *      @height holds row height to allocate from panel for widget height
         *      @item_width holds width of each widget in row
         *      @cols number of widget inside row */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_static(IntPtr ctx, float height, int item_width, int cols);
        
        /*  nk_layout_row_begin - Starts a new dynamic or fixed row with given height and columns.
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_begin_xxx`
         *      @fmt either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns
         *      @row_height holds height of each widget in row or zero for auto layouting
         *      @cols number of widget inside row */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_begin(IntPtr ctx, nk_layout_format fmt, float row_height, int cols);
        
        /*  nk_layout_row_push - Specifies either window ratio or width of a single column
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_row_begin`
         *      @value either a window ratio or fixed width depending on @fmt in previous `nk_layout_row_begin` call */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_push(IntPtr ctx, float value);
      
        /*  nk_layout_row_end - finished previously started row
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_row_begin` */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_end(IntPtr ctx);

        /*  nk_layout_row - specifies row columns in array as either window ratio or size
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context`
         *      @fmt either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns
         *      @row_height holds height of each widget in row or zero for auto layouting
         *      @cols number of widget inside row */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row(IntPtr ctx, nk_layout_format fmt, float height, int cols, IntPtr ratio);
       
        /*  nk_layout_row_template_begin - Begins the row template declaration
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @row_height holds height of each widget in row or zero for auto layouting */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_template_begin(IntPtr ctx, float row_height);
        
        /*  nk_layout_row_template_push_dynamic - adds a dynamic column that dynamically grows and can go to zero if not enough space
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_row_template_begin` */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_template_push_dynamic(IntPtr ctx);

        /*  nk_layout_row_template_push_variable - adds a variable column that dynamically grows but does not shrink below specified pixel width
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_row_template_begin`
         *      @min_width holds the minimum pixel width the next column must be */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_template_push_variable(IntPtr ctx, float min_width);
        
        /*  nk_layout_row_template_push_static - adds a static column that does not grow and will always have the same size
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_row_template_begin`
         *      @width holds the absolute pixel width value the next column must be */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_template_push_static(IntPtr ctx, float width);
        
        /*  nk_layout_row_template_end - marks the end of the row template
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_row_template_begin` */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_row_template_end(IntPtr ctx);
        
        /*  nk_layout_space_begin - begins a new layouting space that allows to specify each widgets position and size.
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct
         *      @fmt either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns
         *      @row_height holds height of each widget in row or zero for auto layouting
         *      @widget_count number of widgets inside row */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_space_begin(IntPtr ctx, nk_layout_format fmt, float height, int widget_count);
       
        /*  nk_layout_space_push - pushes position and size of the next widget in own coordinate space either as pixel or ratio
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_space_begin`
         *      @bounds position and size in laoyut space local coordinates */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_space_push(IntPtr ctx, nk_rect rect);
        
        /*  nk_layout_space_end - marks the end of the layout space
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_space_begin` */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_layout_space_end(IntPtr ctx);

        /*  nk_layout_space_bounds - returns total space allocated for `nk_layout_space`
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_space_begin` */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_layout_space_bounds(IntPtr ctx);

        /*  nk_layout_space_to_screen - converts vector from nk_layout_space coordinate space into screen space
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_space_begin`
         *      @vec position to convert from layout space into screen coordinate space */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_layout_space_to_screen(IntPtr ctx, nk_vec2 pos);

        /*  nk_layout_space_to_screen - converts vector from layout space into screen space
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_space_begin`
         *      @vec position to convert from screen space into layout coordinate space */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_layout_space_to_local(IntPtr ctx, nk_vec2 pos);

        /*  nk_layout_space_rect_to_screen - converts rectangle from screen space into layout space
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_space_begin`
         *      @bounds rectangle to convert from layout space into screen space */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_layout_space_rect_to_screen(IntPtr ctx, nk_rect rect);

        /*  nk_layout_space_rect_to_local - converts rectangle from layout space into screen space
         *  Parameters:
         *      @ctx must point to an previously initialized `nk_context` struct after call `nk_layout_space_begin`
         *      @bounds rectangle to convert from screen space into layout space */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_layout_space_rect_to_local(IntPtr ctx, nk_rect rect);

        /* =============================================================================
         *
         *                                  GROUP
         *
         * ============================================================================= */
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_group_begin(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string title, uint flag);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_group_scrolled_offset_begin(IntPtr ctx, ref uint x_offset, ref uint y_offset, [MarshalAs(UnmanagedType.LPStr)]string p1, uint p2);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_group_scrolled_begin(IntPtr ctx, ref nk_scroll scr, uint title, uint flag);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_group_scrolled_end(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_group_end(IntPtr ctx);
        
        /* =============================================================================
         *
         *                                  LIST VIEW
         *
         * ============================================================================= */
        public unsafe struct nk_list_view
        {
            /* public: */
            public int begin, end, count;
            /* private: */
            public int total_height;
            public IntPtr ctx;
            public uint* scroll_pointer;
            public uint scroll_value;
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_list_view_begin(IntPtr ctx, nk_list_view* outlv, [MarshalAs(UnmanagedType.LPStr)]string id, uint flag, int row_height, int row_count);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_list_view_end(nk_list_view* lv);
        
        /* =============================================================================
         *
         *                                  TREE
         *
         * ============================================================================= */
//#define nk_tree_push(ctx, type, title, state) nk_tree_push_hashed(ctx, type, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),__LINE__)
//#define nk_tree_push_id(ctx, type, title, state, id) nk_tree_push_hashed(ctx, type, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),id)

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_tree_push_hashed(IntPtr ctx, nk_tree_type type, [MarshalAs(UnmanagedType.LPStr)]string title, nk_collapse_states initial_state, [MarshalAs(UnmanagedType.LPStr)]string hash, int len,int seed);
        //#define nk_tree_image_push(ctx, type, img, title, state) nk_tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),__LINE__)
        //#define nk_tree_image_push_id(ctx, type, img, title, state, id) nk_tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),id)

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_tree_image_push_hashed(IntPtr ctx, nk_tree_type type, nk_image image, [MarshalAs(UnmanagedType.LPStr)]string title, nk_collapse_states initial_state, [MarshalAs(UnmanagedType.LPStr)]string hash, int len,int seed);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_tree_pop(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_tree_state_push(IntPtr ctx, nk_tree_type type, [MarshalAs(UnmanagedType.LPStr)]string title, nk_collapse_states *state);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_tree_state_image_push(IntPtr ctx, nk_tree_type type, nk_image image, [MarshalAs(UnmanagedType.LPStr)]string title, nk_collapse_states *state);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_tree_state_pop(IntPtr ctx);

        /* =============================================================================
         *
         *                                  WIDGET
         *
         * ============================================================================= */
        enum nk_widget_layout_states
        {
            NK_WIDGET_INVALID, /* The widget cannot be seen and is completely out of view */
            NK_WIDGET_VALID, /* The widget is completely inside the window and can be updated and drawn */
            NK_WIDGET_ROM /* The widget is partially visible and cannot be updated */
        };

        enum nk_widget_states
        {
            NK_WIDGET_STATE_MODIFIED = (1 << 1),
            NK_WIDGET_STATE_INACTIVE = (1 << 2), /* widget is neither active nor hovered */
            NK_WIDGET_STATE_ENTERED = (1 << 3), /* widget has been hovered on the current frame */
            NK_WIDGET_STATE_HOVER = (1 << 4), /* widget is being hovered */
            NK_WIDGET_STATE_ACTIVED = (1 << 5),/* widget is currently activated */
            NK_WIDGET_STATE_LEFT = (1 << 6), /* widget is from this frame on not hovered anymore */
            NK_WIDGET_STATE_HOVERED = NK_WIDGET_STATE_HOVER | NK_WIDGET_STATE_MODIFIED, /* widget is being hovered */
            NK_WIDGET_STATE_ACTIVE = NK_WIDGET_STATE_ACTIVED | NK_WIDGET_STATE_MODIFIED /* widget is currently activated */
        };


        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_widget_layout_states nk_widget(ref nk_rect rc, IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_widget_layout_states nk_widget_fitting(ref nk_rect rc, IntPtr ctx, nk_vec2 p);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_rect nk_widget_bounds(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_widget_position(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_vec2 nk_widget_size(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_widget_width(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_widget_height(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_widget_is_hovered(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_widget_is_mouse_clicked(IntPtr ctx, nk_buttons btn);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_widget_has_mouse_click_down(IntPtr ctx, nk_buttons btn, int down);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_spacing(IntPtr ctx, int cols);

        /* =============================================================================
         *
         *                                  TEXT
         *
         * ============================================================================= */
        public enum nk_text_align
        {
            NK_TEXT_ALIGN_LEFT = 0x01,
            NK_TEXT_ALIGN_CENTERED = 0x02,
            NK_TEXT_ALIGN_RIGHT = 0x04,
            NK_TEXT_ALIGN_TOP = 0x08,
            NK_TEXT_ALIGN_MIDDLE = 0x10,
            NK_TEXT_ALIGN_BOTTOM = 0x20
        }

        public enum nk_text_alignment
        {
            NK_TEXT_LEFT = nk_text_align.NK_TEXT_ALIGN_MIDDLE | nk_text_align.NK_TEXT_ALIGN_LEFT,
            NK_TEXT_CENTERED = nk_text_align.NK_TEXT_ALIGN_MIDDLE | nk_text_align.NK_TEXT_ALIGN_CENTERED,
            NK_TEXT_RIGHT = nk_text_align.NK_TEXT_ALIGN_MIDDLE | nk_text_align.NK_TEXT_ALIGN_RIGHT
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string text, int p, uint flag);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_text_colored(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string text, int p, uint flag, nk_color c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_text_wrap(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string text, int p);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_text_wrap_colored(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string text, int p, nk_color c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, uint align);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_label_colored(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, uint align, nk_color c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_label_wrap(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_label_colored_wrap(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, nk_color c);

        //[DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        //static extern void nk_image(IntPtr ctx, nk_image image);

#if NK_INCLUDE_STANDARD_VARARGS
NK_API void nk_labelf(IntPtr ctx, nk_flags, [MarshalAs(UnmanagedType.LPStr)]string, ...);
        NK_API void nk_labelf_colored(IntPtr ctx, nk_flags align, struct nk_color, [MarshalAs(UnmanagedType.LPStr)]string,...);
        NK_API void nk_labelf_wrap(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string,...);
        NK_API void nk_labelf_colored_wrap(IntPtr ctx, struct nk_color, [MarshalAs(UnmanagedType.LPStr)]string,...);
#endif

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_value_bool(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string prefix, int v);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_value_int(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string prefix, int v);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_value_uint(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string prefix, uint v);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_value_float(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string prefix, float v);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_value_color_byte(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string prefix, nk_color c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_value_color_float(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string prefix, nk_color c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_value_color_hex(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string prefix, nk_color c);

        /* =============================================================================
         *
         *                                  BUTTON
         *
         * ============================================================================= */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string title, int len);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string title);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_color(IntPtr ctx, nk_color c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_symbol(IntPtr ctx, nk_symbol_type t);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_image(IntPtr ctx, nk_image img);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_symbol_label(IntPtr ctx, nk_symbol_type t, [MarshalAs(UnmanagedType.LPStr)]string text, uint text_alignment);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_symbol_text(IntPtr ctx, nk_symbol_type t, [MarshalAs(UnmanagedType.LPStr)]string text, int p, uint alignment);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_image_label(IntPtr ctx, nk_image img, [MarshalAs(UnmanagedType.LPStr)]string text, uint text_alignment);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_image_text(IntPtr ctx, nk_image img, [MarshalAs(UnmanagedType.LPStr)]string text, int p, uint alignment);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_text_styled(IntPtr ctx, nk_style_button* btn, [MarshalAs(UnmanagedType.LPStr)]string title, int len);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_label_styled(IntPtr ctx, nk_style_button* btn, [MarshalAs(UnmanagedType.LPStr)]string title);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_symbol_styled(IntPtr ctx, nk_style_button* btn, nk_symbol_type t);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_image_styled(IntPtr ctx, nk_style_button* btn, nk_image img);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_symbol_text_styled(IntPtr ctx, nk_style_button* btn, nk_symbol_type t, [MarshalAs(UnmanagedType.LPStr)]string text, int p, uint alignment);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_symbol_label_styled(IntPtr ctx, nk_style_button *style, nk_symbol_type symbol, [MarshalAs(UnmanagedType.LPStr)]string title, uint align);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_image_label_styled(IntPtr ctx, nk_style_button* style, nk_image img, [MarshalAs(UnmanagedType.LPStr)]string title, uint text_alignment);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_image_text_styled(IntPtr ctx, nk_style_button* style, nk_image img, [MarshalAs(UnmanagedType.LPStr)]string title, int p1, uint alignment);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_button_set_behavior(IntPtr ctx, nk_button_behavior b);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_push_behavior(IntPtr ctx, nk_button_behavior b);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_button_pop_behavior(IntPtr ctx);
        /* =============================================================================
         *
         *                                  CHECKBOX
         *
         * ============================================================================= */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_check_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int active);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_check_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int p1,int active);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern uint nk_check_flags_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, uint flags, uint value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern uint nk_check_flags_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int p1, uint flags, uint value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_checkbox_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int* active);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_checkbox_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int p1, int* active);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_checkbox_flags_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, uint* flags, uint value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_checkbox_flags_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int p1, uint* flags, uint value);
        /* =============================================================================
         *
         *                                  RADIO BUTTON
         *
         * ============================================================================= */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_radio_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int* active);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_radio_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int p1, int* active);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_option_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int active);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_option_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int p1, int active);
        /* =============================================================================
         *
         *                                  SELECTABLE
         *
         * ============================================================================= */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_selectable_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, uint align, int* value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_selectable_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int p1, uint align, int* value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_selectable_image_label(IntPtr ctx, nk_image img,  [MarshalAs(UnmanagedType.LPStr)]string label, uint align, int* value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_selectable_image_text(IntPtr ctx, nk_image img, [MarshalAs(UnmanagedType.LPStr)]string label, int p1, uint align, int* value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_select_label(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, uint align, int value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_select_text(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string label, int p1, uint align, int value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_select_image_label(IntPtr ctx, nk_image img,[MarshalAs(UnmanagedType.LPStr)]string label, uint align, int value);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_select_image_text(IntPtr ctx, nk_image img,[MarshalAs(UnmanagedType.LPStr)]string label, int p1, uint align, int value);
        /* =============================================================================
         *
         *                                  SLIDER
         *
         * ============================================================================= */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_slide_float(IntPtr ctx, float min, float val, float max, float step);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_slide_int(IntPtr ctx, int min, int val, int max, int step);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_slider_float(IntPtr ctx, float min, float* val, float max, float step);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_slider_int(IntPtr ctx, int min, int* val, int max, int step);
        /* =============================================================================
         *
         *                                  PROGRESSBAR
         *
         * ============================================================================= */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_progress(IntPtr ctx, long* cur, long max, int modifyable);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern long nk_prog(IntPtr ctx, long cur, long max, int modifyable);

        /* =============================================================================
         *
         *                                  COLOR PICKER
         *
         * ============================================================================= */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_color nk_color_picker(IntPtr ctx, nk_color c, nk_color_format format);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_color_pick(IntPtr ctx, nk_color* c, nk_color_format format);
        /* =============================================================================
         *
         *                                  PROPERTIES
         *
         * ============================================================================= */

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_property_int(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, int min, int* val, int max, int step, float inc_per_pixel);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_property_float(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, float min, float* val, float max, float step, float inc_per_pixel);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_property_double(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, double min, double* val, double max, double step, float inc_per_pixel);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_propertyi(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, int min, int val, int max, int step, float inc_per_pixel);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float nk_propertyf(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, float min, float val, float max, float step, float inc_per_pixel);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern double nk_propertyd(IntPtr ctx, [MarshalAs(UnmanagedType.LPStr)]string name, double min, double val, double max, double step, float inc_per_pixel);

    }
}
