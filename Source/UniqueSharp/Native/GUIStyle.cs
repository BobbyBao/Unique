using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public enum nk_style_item_type
    {
        NK_STYLE_ITEM_COLOR,
        NK_STYLE_ITEM_IMAGE
    };

    public struct nk_style_item_data
    {
        nk_image image;
        nk_color color;
    };

    public struct nk_style_item
    {
        nk_style_item_type type;
        nk_style_item_data data;
    };

    struct nk_style_text
    {
        nk_color color;
        nk_vec2 padding;
    };

    struct nk_style_button
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;
        nk_color border_color;

        /* text */
        nk_color text_background;
        nk_color text_normal;
        nk_color text_hover;
        nk_color text_active;
        uint text_alignment;

        /* properties */
        float border;
        float rounding;
        nk_vec2 padding;
        nk_vec2 image_padding;
        nk_vec2 touch_padding;

        /* optional user callbacks */
        IntPtr userdata;
        IntPtr draw_begin;
        IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, IntPtr userdata);
        //void (* draw_end) (struct nk_command_buffer*, IntPtr userdata);
    };

    struct nk_style_toggle
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;
        nk_color border_color;

        /* cursor */
        nk_style_item cursor_normal;
        nk_style_item cursor_hover;

        /* text */
        nk_color text_normal;
        nk_color text_hover;
        nk_color text_active;
        nk_color text_background;
        uint text_alignment;

        /* properties */
        nk_vec2 padding;
        nk_vec2 touch_padding;
        float spacing;
        float border;

        /* optional user callbacks */
        IntPtr userdata;
        IntPtr draw_begin;
        IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        //void (* draw_end) (struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_selectable
    {
        /* background (inactive) */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item pressed;

        /* background (active) */
        nk_style_item normal_active;
        nk_style_item hover_active;
        nk_style_item pressed_active;

        /* text color (inactive) */
        nk_color text_normal;
        nk_color text_hover;
        nk_color text_pressed;

        /* text color (active) */
        nk_color text_normal_active;
        nk_color text_hover_active;
        nk_color text_pressed_active;
        nk_color text_background;
        uint text_alignment;

        /* properties */
        float rounding;
        nk_vec2 padding;
        nk_vec2 touch_padding;
        nk_vec2 image_padding;

        /* optional user callbacks */
        IntPtr userdata;
        IntPtr draw_begin;
        IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        //void (* draw_end) (struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_slider
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;
        nk_color border_color;

        /* background bar */
        nk_color bar_normal;
        nk_color bar_hover;
        nk_color bar_active;
        nk_color bar_filled;

        /* cursor */
        nk_style_item cursor_normal;
        nk_style_item cursor_hover;
        nk_style_item cursor_active;

        /* properties */
        float border;
        float rounding;
        float bar_height;
        nk_vec2 padding;
        nk_vec2 spacing;
        nk_vec2 cursor_size;

        /* optional buttons */
        int show_buttons;
        nk_style_button inc_button;
        nk_style_button dec_button;
        nk_symbol_type inc_symbol;
        nk_symbol_type dec_symbol;

        /* optional user callbacks */
        IntPtr userdata;
        IntPtr draw_begin;
        IntPtr draw_end;
        // void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        // void (* draw_end) (struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_progress
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;
        nk_color border_color;

        /* cursor */
        nk_style_item cursor_normal;
        nk_style_item cursor_hover;
        nk_style_item cursor_active;
        nk_color cursor_border_color;

        /* properties */
        float rounding;
        float border;
        float cursor_border;
        float cursor_rounding;
        nk_vec2 padding;

        /* optional user callbacks */
        IntPtr userdata;
        IntPtr draw_begin;
        IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        //void (* draw_end) (struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_scrollbar
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;
        nk_color border_color;

        /* cursor */
        nk_style_item cursor_normal;
        nk_style_item cursor_hover;
        nk_style_item cursor_active;
        nk_color cursor_border_color;

        /* properties */
        float border;
        float rounding;
        float border_cursor;
        float rounding_cursor;
        nk_vec2 padding;

        /* optional buttons */
        int show_buttons;
        nk_style_button inc_button;
        nk_style_button dec_button;
        nk_symbol_type inc_symbol;
        nk_symbol_type dec_symbol;

        /* optional user callbacks */
        IntPtr userdata;
        IntPtr draw_begin;
        IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        //void (* draw_end) (struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_edit
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;
        nk_color border_color;
        nk_style_scrollbar scrollbar;

        /* cursor  */
        nk_color cursor_normal;
        nk_color cursor_hover;
        nk_color cursor_text_normal;
        nk_color cursor_text_hover;

        /* text (unselected) */
        nk_color text_normal;
        nk_color text_hover;
        nk_color text_active;

        /* text (selected) */
        nk_color selected_normal;
        nk_color selected_hover;
        nk_color selected_text_normal;
        nk_color selected_text_hover;

        /* properties */
        float border;
        float rounding;
        float cursor_size;
        nk_vec2 scrollbar_size;
        nk_vec2 padding;
        float row_padding;
    };

    struct nk_style_property
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;
        nk_color border_color;

        /* text */
        nk_color label_normal;
        nk_color label_hover;
        nk_color label_active;

        /* symbols */
        nk_symbol_type sym_left;
        nk_symbol_type sym_right;

        /* properties */
        float border;
        float rounding;
        nk_vec2 padding;

        nk_style_edit edit;
        nk_style_button inc_button;
        nk_style_button dec_button;

        /* optional user callbacks */
        IntPtr userdata;
        IntPtr draw_begin;
        IntPtr draw_end;
        // void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        // void (* draw_end) (struct nk_command_buffer*, nk_handle);
    };

    struct nk_style_chart
    {
        /* colors */
        nk_style_item background;
        nk_color border_color;
        nk_color selected_color;
        nk_color color;

        /* properties */
        float border;
        float rounding;
        nk_vec2 padding;
    };

    struct nk_style_combo
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;
        nk_color border_color;

        /* label */
        nk_color label_normal;
        nk_color label_hover;
        nk_color label_active;

        /* symbol */
        nk_color symbol_normal;
        nk_color symbol_hover;
        nk_color symbol_active;

        /* button */
        nk_style_button button;
        nk_symbol_type sym_normal;
        nk_symbol_type sym_hover;
        nk_symbol_type sym_active;

        /* properties */
        float border;
        float rounding;
        nk_vec2 content_padding;
        nk_vec2 button_padding;
        nk_vec2 spacing;
    };

    struct nk_style_tab
    {
        /* background */
        nk_style_item background;
        nk_color border_color;
        nk_color text;

        /* button */
        nk_style_button tab_maximize_button;
        nk_style_button tab_minimize_button;
        nk_style_button node_maximize_button;
        nk_style_button node_minimize_button;
        nk_symbol_type sym_minimize;
        nk_symbol_type sym_maximize;

        /* properties */
        float border;
        float rounding;
        float indent;
        nk_vec2 padding;
        nk_vec2 spacing;
    };

    enum nk_style_header_align
    {
        NK_HEADER_LEFT,
        NK_HEADER_RIGHT
    };

    struct nk_style_window_header
    {
        /* background */
        nk_style_item normal;
        nk_style_item hover;
        nk_style_item active;

        /* button */
        nk_style_button close_button;
        nk_style_button minimize_button;
        nk_symbol_type close_symbol;
        nk_symbol_type minimize_symbol;
        nk_symbol_type maximize_symbol;

        /* title */
        nk_color label_normal;
        nk_color label_hover;
        nk_color label_active;

        /* properties */
        nk_style_header_align align;
        nk_vec2 padding;
        nk_vec2 label_padding;
        nk_vec2 spacing;
    };

    struct nk_style_window
    {
        nk_style_window_header header;
        nk_style_item fixed_background;
        nk_color background;

        nk_color border_color;
        nk_color popup_border_color;
        nk_color combo_border_color;
        nk_color contextual_border_color;
        nk_color menu_border_color;
        nk_color group_border_color;
        nk_color tooltip_border_color;
        nk_style_item scaler;

        float border;
        float combo_border;
        float contextual_border;
        float menu_border;
        float group_border;
        float tooltip_border;
        float popup_border;
        float min_row_height_padding;

        float rounding;
        nk_vec2 spacing;
        nk_vec2 scrollbar_size;
        nk_vec2 min_size;

        nk_vec2 padding;
        nk_vec2 group_padding;
        nk_vec2 popup_padding;
        nk_vec2 combo_padding;
        nk_vec2 contextual_padding;
        nk_vec2 menu_padding;
        nk_vec2 tooltip_padding;
    };

    public unsafe struct nk_style
    {
        //nk_user_font *font;
        IntPtr font;
        fixed /*nk_cursor**/long cursors[(int)nk_style_cursor.NK_CURSOR_COUNT];
        nk_cursor* cursor_active;
        nk_cursor* cursor_last;
        int cursor_visible;

        nk_style_text text;
        nk_style_button button;
        nk_style_button contextual_button;
        nk_style_button menu_button;
        nk_style_toggle option;
        nk_style_toggle checkbox;
        nk_style_selectable selectable;
        nk_style_slider slider;
        nk_style_progress progress;
        nk_style_property property;
        nk_style_edit edit;
        nk_style_chart chart;
        nk_style_scrollbar scrollh;
        nk_style_scrollbar scrollv;
        nk_style_tab tab;
        nk_style_combo combo;
        nk_style_window window;
    };

    /* =============================================================================
     *
     *                                  STYLE
     *
     * ============================================================================= */
    enum nk_style_colors
    {
        NK_COLOR_TEXT,
        NK_COLOR_WINDOW,
        NK_COLOR_HEADER,
        NK_COLOR_BORDER,
        NK_COLOR_BUTTON,
        NK_COLOR_BUTTON_HOVER,
        NK_COLOR_BUTTON_ACTIVE,
        NK_COLOR_TOGGLE,
        NK_COLOR_TOGGLE_HOVER,
        NK_COLOR_TOGGLE_CURSOR,
        NK_COLOR_SELECT,
        NK_COLOR_SELECT_ACTIVE,
        NK_COLOR_SLIDER,
        NK_COLOR_SLIDER_CURSOR,
        NK_COLOR_SLIDER_CURSOR_HOVER,
        NK_COLOR_SLIDER_CURSOR_ACTIVE,
        NK_COLOR_PROPERTY,
        NK_COLOR_EDIT,
        NK_COLOR_EDIT_CURSOR,
        NK_COLOR_COMBO,
        NK_COLOR_CHART,
        NK_COLOR_CHART_COLOR,
        NK_COLOR_CHART_COLOR_HIGHLIGHT,
        NK_COLOR_SCROLLBAR,
        NK_COLOR_SCROLLBAR_CURSOR,
        NK_COLOR_SCROLLBAR_CURSOR_HOVER,
        NK_COLOR_SCROLLBAR_CURSOR_ACTIVE,
        NK_COLOR_TAB_HEADER,
        NK_COLOR_COUNT
    };

    enum nk_style_cursor
    {
        NK_CURSOR_ARROW,
        NK_CURSOR_TEXT,
        NK_CURSOR_MOVE,
        NK_CURSOR_RESIZE_VERTICAL,
        NK_CURSOR_RESIZE_HORIZONTAL,
        NK_CURSOR_RESIZE_TOP_LEFT_DOWN_RIGHT,
        NK_CURSOR_RESIZE_TOP_RIGHT_DOWN_LEFT,
        NK_CURSOR_COUNT
    };

    public unsafe class GUIStyle
    {

        /*
NK_API nk_style_item nk_style_item_image(nk_image img);
NK_API nk_style_item nk_style_item_color(nk_color);
NK_API nk_style_item nk_style_item_hide(void);*/

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_style_default(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_style_from_table(IntPtr ctx, nk_color* c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_style_load_cursor(IntPtr ctx, nk_style_cursor style, nk_cursor* cursor);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_style_load_all_cursors(IntPtr ctx, nk_cursor* cursor);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.LPStr)] 
        static extern string nk_style_get_color_by_name(nk_style_colors c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_style_set_font(IntPtr ctx, IntPtr userFont);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_set_cursor(IntPtr ctx, nk_style_cursor cursor);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_style_show_cursor(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void nk_style_hide_cursor(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_push_font(IntPtr ctx, IntPtr userFont);// nk_user_font*);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_push_float(IntPtr ctx, float* p1, float p2);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_push_vec2(IntPtr ctx, nk_vec2* p1, nk_vec2 p2);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_push_style_item(IntPtr ctx, nk_style_item* p1, nk_style_item p2);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_push_flags(IntPtr ctx, uint* p1, uint p2);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_push_color(IntPtr ctx, nk_color* p1, nk_color p2);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_pop_font(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_pop_float(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_pop_vec2(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_pop_style_item(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_pop_flags(IntPtr ctx);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int nk_style_pop_color(IntPtr ctx);
        
        
    }
}
