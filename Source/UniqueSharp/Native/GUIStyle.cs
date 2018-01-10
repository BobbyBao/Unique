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
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct nk_style_item_data
    {
        [FieldOffset(0)]
        public nk_image image;
        [FieldOffset(0)]
        public nk_color color;        
    }

    public struct nk_style_item
    {
        public nk_style_item_type type;
        public nk_style_item_data data;
    }

    public struct nk_style_text
    {
        public nk_color color;
        public nk_vec2 padding;
    }

    public struct nk_style_button
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;
        public nk_color border_color;

        /* text */
        public nk_color text_background;
        public nk_color text_normal;
        public nk_color text_hover;
        public nk_color text_active;
        public uint text_alignment;

        /* properties */
        public float border;
        public float rounding;
        public nk_vec2 padding;
        public nk_vec2 image_padding;
        public nk_vec2 touch_padding;

        /* optional user callbacks */
        public IntPtr userdata;
        public IntPtr draw_begin;
        public IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, IntPtr userdata);
        //void (* draw_end) (struct nk_command_buffer*, IntPtr userdata);
    };

    public struct nk_style_toggle
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;
        public nk_color border_color;

        /* cursor */
        public nk_style_item cursor_normal;
        public nk_style_item cursor_hover;

        /* text */
        public nk_color text_normal;
        public nk_color text_hover;
        public nk_color text_active;
        public nk_color text_background;
        public uint text_alignment;

        /* properties */
        public nk_vec2 padding;
        public nk_vec2 touch_padding;
        public float spacing;
        public float border;

        /* optional user callbacks */
        public IntPtr userdata;
        public IntPtr draw_begin;
        public IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        //void (* draw_end) (struct nk_command_buffer*, nk_handle);
    }

    public struct nk_style_selectable
    {
        /* background (inactive) */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item pressed;

        /* background (active) */
        public nk_style_item normal_active;
        public nk_style_item hover_active;
        public nk_style_item pressed_active;

        /* text color (inactive) */
        public nk_color text_normal;
        public nk_color text_hover;
        public nk_color text_pressed;

        /* text color (active) */
        public nk_color text_normal_active;
        public nk_color text_hover_active;
        public nk_color text_pressed_active;
        public nk_color text_background;
        public uint text_alignment;

        /* properties */
        public float rounding;
        public nk_vec2 padding;
        public nk_vec2 touch_padding;
        public nk_vec2 image_padding;

        /* optional user callbacks */
        public IntPtr userdata;
        public IntPtr draw_begin;
        public IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        //void (* draw_end) (struct nk_command_buffer*, nk_handle);
    }

    public struct nk_style_slider
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;
        public nk_color border_color;

        /* background bar */
        public nk_color bar_normal;
        public nk_color bar_hover;
        public nk_color bar_active;
        public nk_color bar_filled;

        /* cursor */
        public nk_style_item cursor_normal;
        public nk_style_item cursor_hover;
        public nk_style_item cursor_active;

        /* properties */
        public float border;
        public float rounding;
        public float bar_height;
        public nk_vec2 padding;
        public nk_vec2 spacing;
        public nk_vec2 cursor_size;

        /* optional buttons */
        public int show_buttons;
        public nk_style_button inc_button;
        public nk_style_button dec_button;
        public nk_symbol_type inc_symbol;
        public nk_symbol_type dec_symbol;

        /* optional user callbacks */
        public IntPtr userdata;
        public IntPtr draw_begin;
        public IntPtr draw_end;
        // void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        // void (* draw_end) (struct nk_command_buffer*, nk_handle);
    }

    public struct nk_style_progress
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;
        public nk_color border_color;

        /* cursor */
        public nk_style_item cursor_normal;
        public nk_style_item cursor_hover;
        public nk_style_item cursor_active;
        public nk_color cursor_border_color;

        /* properties */
        public float rounding;
        public float border;
        public float cursor_border;
        public float cursor_rounding;
        public nk_vec2 padding;

        /* optional user callbacks */
        public IntPtr userdata;
        public IntPtr draw_begin;
        public IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        //void (* draw_end) (struct nk_command_buffer*, nk_handle);
    }

    public struct nk_style_scrollbar
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;
        public nk_color border_color;

        /* cursor */
        public nk_style_item cursor_normal;
        public nk_style_item cursor_hover;
        public nk_style_item cursor_active;
        public nk_color cursor_border_color;

        /* properties */
        public float border;
        public float rounding;
        public float border_cursor;
        public float rounding_cursor;
        public nk_vec2 padding;

        /* optional buttons */
        public int show_buttons;
        public nk_style_button inc_button;
        public nk_style_button dec_button;
        public nk_symbol_type inc_symbol;
        public nk_symbol_type dec_symbol;

        /* optional user callbacks */
        public IntPtr userdata;
        public IntPtr draw_begin;
        public IntPtr draw_end;
        //void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        //void (* draw_end) (struct nk_command_buffer*, nk_handle);
    }

    public struct nk_style_edit
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;
        public nk_color border_color;
        public nk_style_scrollbar scrollbar;

        /* cursor  */
        public nk_color cursor_normal;
        public nk_color cursor_hover;
        public nk_color cursor_text_normal;
        public nk_color cursor_text_hover;

        /* text (unselected) */
        public nk_color text_normal;
        public nk_color text_hover;
        public nk_color text_active;

        /* text (selected) */
        public nk_color selected_normal;
        public nk_color selected_hover;
        public nk_color selected_text_normal;
        public nk_color selected_text_hover;

        /* properties */
        public float border;
        public float rounding;
        public float cursor_size;
        public nk_vec2 scrollbar_size;
        public nk_vec2 padding;
        public float row_padding;
    }

    public struct nk_style_property
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;
        public nk_color border_color;

        /* text */
        public nk_color label_normal;
        public nk_color label_hover;
        public nk_color label_active;

        /* symbols */
        public nk_symbol_type sym_left;
        public nk_symbol_type sym_right;

        /* properties */
        public float border;
        public float rounding;
        public nk_vec2 padding;

        public nk_style_edit edit;
        public nk_style_button inc_button;
        public nk_style_button dec_button;

        /* optional user callbacks */
        public IntPtr userdata;
        public IntPtr draw_begin;
        public IntPtr draw_end;
        // void (* draw_begin) (struct nk_command_buffer*, nk_handle);
        // void (* draw_end) (struct nk_command_buffer*, nk_handle);
    }

    public struct nk_style_chart
    {
        /* colors */
        public nk_style_item background;
        public nk_color border_color;
        public nk_color selected_color;
        public nk_color color;

        /* properties */
        public float border;
        public float rounding;
        public nk_vec2 padding;
    }

    public struct nk_style_combo
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;
        public nk_color border_color;

        /* label */
        public nk_color label_normal;
        public nk_color label_hover;
        public nk_color label_active;

        /* symbol */
        public nk_color symbol_normal;
        public nk_color symbol_hover;
        public nk_color symbol_active;

        /* button */
        public nk_style_button button;
        public nk_symbol_type sym_normal;
        public nk_symbol_type sym_hover;
        public nk_symbol_type sym_active;

        /* properties */
        public float border;
        public float rounding;
        public nk_vec2 content_padding;
        public nk_vec2 button_padding;
        public nk_vec2 spacing;
    }

    public struct nk_style_tab
    {
        /* background */
        public nk_style_item background;
        public nk_color border_color;
        public nk_color text;

        /* button */
        public nk_style_button tab_maximize_button;
        public nk_style_button tab_minimize_button;
        public nk_style_button node_maximize_button;
        public nk_style_button node_minimize_button;
        public nk_symbol_type sym_minimize;
        public nk_symbol_type sym_maximize;

        /* properties */
        public float border;
        public float rounding;
        public float indent;
        public nk_vec2 padding;
        public nk_vec2 spacing;
    }

    public enum nk_style_header_align
    {
        NK_HEADER_LEFT,
        NK_HEADER_RIGHT
    }

    public struct nk_style_window_header
    {
        /* background */
        public nk_style_item normal;
        public nk_style_item hover;
        public nk_style_item active;

        /* button */
        public nk_style_button close_button;
        public nk_style_button minimize_button;
        public nk_symbol_type close_symbol;
        public nk_symbol_type minimize_symbol;
        public nk_symbol_type maximize_symbol;

        /* title */
        public nk_color label_normal;
        public nk_color label_hover;
        public nk_color label_active;

        /* properties */
        public nk_style_header_align align;
        public nk_vec2 padding;
        public nk_vec2 label_padding;
        public nk_vec2 spacing;
    }

    public struct nk_style_window
    {
        public nk_style_window_header header;
        public nk_style_item fixed_background;
        public nk_color background;

        public nk_color border_color;
        public nk_color popup_border_color;
        public nk_color combo_border_color;
        public nk_color contextual_border_color;
        public nk_color menu_border_color;
        public nk_color group_border_color;
        public nk_color tooltip_border_color;
        public nk_style_item scaler;

        public float border;
        public float combo_border;
        public float contextual_border;
        public float menu_border;
        public float group_border;
        public float tooltip_border;
        public float popup_border;
        public float min_row_height_padding;

        public float rounding;
        public nk_vec2 spacing;
        public nk_vec2 scrollbar_size;
        public nk_vec2 min_size;

        public nk_vec2 padding;
        public nk_vec2 group_padding;
        public nk_vec2 popup_padding;
        public nk_vec2 combo_padding;
        public nk_vec2 contextual_padding;
        public nk_vec2 menu_padding;
        public nk_vec2 tooltip_padding;
    }

    public unsafe struct nk_style
    {
        //nk_user_font *font;
        public IntPtr font;
        public fixed /*nk_cursor**/long cursors[(int)nk_style_cursor.NK_CURSOR_COUNT];
        public nk_cursor* cursor_active;
        public nk_cursor* cursor_last;
        public int cursor_visible;

        public nk_style_text text;
        public nk_style_button button;
        public nk_style_button contextual_button;
        public nk_style_button menu_button;
        public nk_style_toggle option;
        public nk_style_toggle checkbox;
        public nk_style_selectable selectable;
        public nk_style_slider slider;
        public nk_style_progress progress;
        public nk_style_property property;
        public nk_style_edit edit;
        public nk_style_chart chart;
        public nk_style_scrollbar scrollh;
        public nk_style_scrollbar scrollv;
        public nk_style_tab tab;
        public nk_style_combo combo;
        public nk_style_window window;
    }

    /* =============================================================================
     *
     *                                  STYLE
     *
     * ============================================================================= */
    public enum nk_style_colors
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
    }

    public enum nk_style_cursor
    {
        NK_CURSOR_ARROW,
        NK_CURSOR_TEXT,
        NK_CURSOR_MOVE,
        NK_CURSOR_RESIZE_VERTICAL,
        NK_CURSOR_RESIZE_HORIZONTAL,
        NK_CURSOR_RESIZE_TOP_LEFT_DOWN_RIGHT,
        NK_CURSOR_RESIZE_TOP_RIGHT_DOWN_LEFT,
        NK_CURSOR_COUNT
    }

    public unsafe class GUIStyle
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_style_item nk_style_item_image(nk_image img);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_style_item nk_style_item_color(nk_color c);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern nk_style_item nk_style_item_hide();

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
