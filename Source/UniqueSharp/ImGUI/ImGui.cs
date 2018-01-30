using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace UniqueEngine
{
    public unsafe static partial class ImGUI
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
        public static bool Begin(string title, nk_rect bounds, nkPanelFlags flags) => nk_begin(ctx, title, bounds, (uint)flags) != 0;
        public static int BeginTitled(string name, string title, nk_rect bounds, nkPanelFlags flags) => nk_begin_titled(ctx, name, title, bounds, (uint)flags);
        public static void End() => nk_end(ctx);

        public static bool Window(string name, string title, float x, float y, float w, float h, nkPanelFlags flags, Action a)
        {
            bool res = true;

            if(nk_begin_titled(ctx, name, title, new nk_rect(x, y, w, h), (uint)flags) != 0)
                a?.Invoke();
            else
                res = false;

            nk_end(ctx);
            return res;
        }

        public static bool Window(string title, float x, float y, float w, float h, nkPanelFlags flags, Action a) => Window(title, title, x, y, w, h, flags, a);
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
        public static void LayoutRowStatic(float height, int itemWidth, int cols) => nk_layout_row_static(ctx, height, itemWidth, cols);
        public static void LayoutRowBegin(nk_layout_format fmt, float row_height, int cols) => nk_layout_row_begin(ctx, fmt, row_height, cols);
        public static void LayoutRowPush(float value) => nk_layout_row_push(ctx, value);
        public static void LayoutRowEnd() => nk_layout_row_end(ctx);
        public static void LayoutRow(nk_layout_format fmt, float height, int cols, IntPtr ratio) => nk_layout_row(ctx, fmt, height, cols, ratio);
        public static void LayoutRow_template_begin(float row_height) => nk_layout_row_template_begin(ctx, row_height);
        public static void LayoutRowTemplate_push_dynamic() => nk_layout_row_template_push_dynamic(ctx);
        public static void LayoutRow_template_push_variable(float min_width) => nk_layout_row_template_push_variable(ctx, min_width);
        public static void LayoutRow_template_push_static(float width) => nk_layout_row_template_push_static(ctx, width);
        public static void LayoutRow_template_end() => nk_layout_row_template_end(ctx);
        public static void LayoutSpaceBegin(nk_layout_format fmt, float height, int widget_count) => nk_layout_space_begin(ctx, fmt, height, widget_count);
        public static void LayoutSpacePush(nk_rect rect) => nk_layout_space_push(ctx, rect);
        public static void LayoutSpaceEnd() => nk_layout_space_end(ctx);
        public static nk_rect LayoutSpaceBounds() => nk_layout_space_bounds(ctx);
        public static nk_vec2 LayoutSpaceToScreen(nk_vec2 pos) => nk_layout_space_to_screen(ctx, pos);
        public static nk_vec2 LayoutSpaceToLocal(nk_vec2 pos) => nk_layout_space_to_local(ctx, pos);
        public static nk_rect LayoutSpaceRectToScreen(nk_rect rect) => nk_layout_space_rect_to_screen(ctx, rect);
        public static nk_rect LayoutSpaceRectToLocal(nk_rect rect) => nk_layout_space_rect_to_local(ctx, rect);

        public static int GroupBegin(string title, uint flag) => nk_group_begin(ctx, title, flag);
        public static int GroupScrolledOffsetBegin(ref uint x_offset, ref uint y_offset, string title, uint flag) => nk_group_scrolled_offset_begin(ctx, ref x_offset, ref y_offset, title, flag);
        public static int GroupScrolledBegin(ref nk_scroll scr, string title, uint flag) => nk_group_scrolled_begin(ctx,  ref scr, title, flag);
        public static void GroupScrolledEnd() => nk_group_scrolled_end(ctx);
        public static void GroupEnd() => nk_group_end(ctx);

        public static int ListViewBegin(ref nk_list_view outlv, string id, uint flag, int row_height, int row_count)
            => nk_list_view_begin(ctx, Utilities.As(ref outlv), id, flag, row_height, row_count);
        public static void ListViewEnd(ref nk_list_view outlv) => nk_list_view_end(Utilities.As(ref outlv));

        public static int TreePushHashed(nk_tree_type type, string title, nk_collapse_states initial_state, string hash, int len, int seed) 
            => nk_tree_push_hashed(ctx, type, title, initial_state, hash, len, seed);

        //#define nk_tree_image_push(ctx, type, img, title, state) nk_tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),__LINE__)
        //#define nk_tree_image_push_id(ctx, type, img, title, state, id) nk_tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE,nk_strlen(NK_FILE_LINE),id)

        public static int TreeImagePushHashed(nk_tree_type type, nkImage image, string title, nk_collapse_states initial_state, string hash, int len, int seed)
            => nk_tree_image_push_hashed(ctx, type, image, title, initial_state, hash, len, seed);

        public static void TreePop() => nk_tree_pop(ctx);

        public static int TreeStatePush(nk_tree_type type, string title, ref nk_collapse_states state)
            => nk_tree_state_push(ctx, type, title, (nk_collapse_states*)Utilities.As(ref state));

        public static unsafe int TreeStateImagePush(nk_tree_type type, nkImage image, string title, ref nk_collapse_states state)
            =>nk_tree_state_image_push(ctx, type, image, title, (nk_collapse_states*)Utilities.As(ref state));

        public static void TreeStatePop() => nk_tree_state_pop(ctx);

        public static nk_widget_layout_states Widget(ref nk_rect rc)
            => nk_widget(ref rc, ctx);
        public static nk_widget_layout_states WidgetFitting(ref nk_rect rc, nk_vec2 p)
            => nk_widget_fitting(ref rc, ctx, p);
        public static nk_rect WidgetBounds()=> nk_widget_bounds(ctx);
        public static nk_vec2 WidgetPosition() => nk_widget_position(ctx);
        public static nk_vec2 WidgetSize() => nk_widget_size(ctx);
        public static float WidgetWidth() => nk_widget_width(ctx);
        public static float WidgetHeight() => nk_widget_height(ctx);
        public static bool WidgetIsHovered() => nk_widget_is_hovered(ctx) != 0;
        public static bool WidgetIsMouseClicked(nk_buttons btn) => nk_widget_is_mouse_clicked(ctx, btn) != 0;
        public static bool WidgetHasMouseClickDown(nk_buttons btn, int down)
            => nk_widget_has_mouse_click_down(ctx, btn, down) != 0;
        public static void Spacing(int cols) => nk_spacing(ctx, cols);

        public static void Text(string text, nk_text_alignment flag = nk_text_alignment.NK_TEXT_LEFT) => nk_text(ctx, text, text.Length, (uint)flag);
        public static void Text(string text, nk_text_alignment flag, nk_color c) => nk_text_colored(ctx, text, text.Length, (uint)flag, c);
        public static void TextWrap(string text) => nk_text_wrap(ctx, text, text.Length);
        public static void TextWrap(string text, nk_color c) => nk_text_wrap_colored(ctx, text, text.Length, c);
        public static void Label(string label, nk_text_alignment align = nk_text_alignment.NK_TEXT_LEFT) => nk_label(ctx, label, (uint)align);
        public static void Label(string label, nk_text_alignment align, nk_color c) => nk_label_colored(ctx, label, (uint)align, c);
        public static void LabelWrap(string label) => nk_label_wrap(ctx, label);
        public static void LabelWrap(string label, nk_color c) => nk_label_colored_wrap(ctx, label, c);

        public static void Image(nkImage image) => nk_image(ctx, image);

        public static void Value(string prefix, bool v) => nk_value_bool(ctx, prefix, v ? 1 : 0);
        public static void Value(string prefix, int v) => nk_value_int(ctx, prefix, v);
        public static void Value(string prefix, uint v) => nk_value_uint(ctx, prefix, v);
        public static void Value(string prefix, float v) => nk_value_float(ctx, prefix, v);
        public static void ValueColorByte(string prefix, nk_color c)=> nk_value_color_byte(ctx, prefix, c);
        public static void ValueColorFloat(string prefix, nk_color c) => nk_value_color_float(ctx, prefix, c);
        public static void ValueColorHex(string prefix, nk_color c) => nk_value_color_hex(ctx, prefix, c);

        public static bool Button(string text) => nk_button_text(ctx, text, text.Length) != 0;
        //public static bool ButtonLabel(string label) => nk_button_label(ctx, label) != 0;
        public static bool Button(char c) => Button(c.ToString());      
        public static bool Button(nk_color c)=> nk_button_color(ctx, c) != 0;
        public static bool Button(nk_symbol_type t) => nk_button_symbol(ctx, t) != 0;
        public static bool Button(nkImage img) => nk_button_image(ctx, img) != 0;
        public static bool Button(nk_symbol_type t, string text, uint alignment) =>
            nk_button_symbol_text(ctx, t, text, text.Length, alignment) != 0;        
        //public static bool Button(nkImage img, string text, uint text_alignment) =>
        //    nk_button_image_label(ctx, img, text, text_alignment) != 0;
        public static bool Button(nkImage img, string text, uint alignment) =>
            nk_button_image_text(ctx, img, text, text.Length, alignment) != 0;
        public static bool ButtonTextStyled(nk_style_button* btn, string title, int len)
            => nk_button_text_styled(ctx, btn, title, len) != 0;
        public static bool ButtonLabelStyled(nk_style_button* btn, string title) =>
            nk_button_label_styled(ctx, btn, title) != 0;        
        public static bool ButtonSymbolStyled(nk_style_button* btn, nk_symbol_type t)
            => nk_button_symbol_styled(ctx, btn, t) != 0;        
        public static bool ButtonImageStyled(nk_style_button* btn, nkImage img)
            => nk_button_image_styled(ctx, btn, img) != 0;
        public static bool ButtonSymbolTextStyled(ref nk_style_button btn, nk_symbol_type t, string text, uint alignment)
            => nk_button_symbol_text_styled(ctx, (nk_style_button*)Utilities.As(ref btn), t, text, text.Length, alignment) != 0;
        public static bool ButtonSymbolLabelStyled(ref nk_style_button style, nk_symbol_type symbol, string title, uint align)
            => nk_button_symbol_label_styled(ctx, (nk_style_button*)Utilities.As(ref style), symbol, title, align) != 0;
        public static bool ButtonImageLabelStyled(ref nk_style_button style, nkImage img, string title, uint text_alignment)
            => nk_button_image_label_styled(ctx, (nk_style_button*)Utilities.As(ref style), img, title, text_alignment) != 0;
        public static bool ButtonImageTextStyled(ref nk_style_button style, nkImage img, string title, uint alignment)
            => nk_button_image_text_styled(ctx, (nk_style_button*)Utilities.As(ref style), img, title, title.Length, alignment) != 0;
        public static void ButtonSetBehavior(nk_button_behavior b) => nk_button_set_behavior(ctx, b);
        public static bool ButtonPushBehavior(nk_button_behavior b) => nk_button_push_behavior(ctx, b) != 0;
        public static bool ButtonPopBehavior() => nk_button_pop_behavior(ctx) != 0;
        public static int CheckLabel(string label, int active) => nk_check_label(ctx, label, active);
        public static int CheckText(string label, int active) => nk_check_text(ctx, label, label.Length, active);
        public static uint CheckFlagsLabel(string label, uint flags, uint value) => nk_check_flags_label(ctx, label, flags, value);
        public static uint CheckFlagsText(string label, int p1, uint flags, uint value) => nk_check_flags_text(ctx, label, p1, flags, value);
        public static int CheckboxLabel(string label, ref int active) => nk_checkbox_label(ctx, label, (int*)Utilities.As(ref active));
        public static int CheckboxText(string label, int p1, ref int active) => nk_checkbox_text(ctx, label, p1, (int*)Utilities.As(ref active));
        public static int CheckboxFlagsLabel(string label, ref uint flags, uint value) => nk_checkbox_flags_label(ctx, label, (uint *)Utilities.As(ref flags), value);
        public static int CheckboxFlagsText(string label, ref uint flags, uint value) => nk_checkbox_flags_text(ctx, label, label.Length, (uint*) Utilities.As(ref flags), value);
        public static int Radio(string label, ref int active) => nk_radio_text(ctx, label,  label.Length, (int*)Utilities.As(ref active));
        //public static int OptionLabel(string label, int active) => nk_option_label(ctx, label, active);
        public static bool Option(string label, bool active) => nk_option_text(ctx, label, label.Length, active ? 1 :0) != 0;
        //public static int SelectableLabel(string label, uint align, ref int value) => nk_selectable_label(ctx, label, align, (int*) Utilities.As(ref value));
        public static int Selectable(string label, uint align, ref int value) => nk_selectable_text(ctx, label, label.Length, align, (int*)Utilities.As(ref value));
        public static int Selectable(nkImage img, string label, uint align, ref int value) => nk_selectable_image_label(ctx, img, label, align, (int*)Utilities.As(ref value));
        //public static int Selectable(nkImage img, string label, uint align, ref int value) => nk_selectable_image_text(ctx, img, label, label.Length, align, (int*)Utilities.As(ref value));
        public static int Select(string label, uint align, int value) => nk_select_label(ctx, label, align, value);
        //public static int SelectText(string label, uint align, int value) => nk_select_text(ctx, label, p1, align, value);
        public static int Select(nkImage img, string label, uint align, int value) => nk_select_image_label(ctx, img, label, align, value);
        //public static int Select(nkImage img, string label, uint align, int value) => nk_select_image_text(ctx, img, label, label.Length, align, value);
        public static float Slide(float min, float val, float max, float step) => nk_slide_float(ctx, min, val, max, step);
        public static int Slide(int min, int val, int max, int step) => nk_slide_int(ctx, min, val, max, step);
        public static int Slider(float min, ref float val, float max, float step) => nk_slider_float(ctx, min, (float*)Utilities.As(ref val), max, step);
        public static int Slider(int min, ref int val, int max, int step) => nk_slider_int(ctx, min, (int*)Utilities.As(ref val), max, step);
        public static int Progress(ref int cur, int max, int modifyable) => nk_progress(ctx, (int *)Utilities.As(ref cur), max, modifyable);
        public static int Prog(int cur, int max, int modifyable) => nk_prog(ctx, cur, max, modifyable);
        public static nk_color ColorPicker(Color c, nk_color_format format) => nk_color_picker(ctx, c, (uint)format);
        public static bool ColorPick(ref Color c, nk_color_format format) => nk_color_pick(ctx, (Color *)Utilities.As(ref c), (uint)format) != 0;
        public static void PropertyInt(string name, int min, ref int val, int max, int step, float inc_per_pixel) => nk_property_int(ctx, name, min, (int*)Utilities.As(ref val), max, step, inc_per_pixel);
        public static void PropertyFloat(string name, float min, ref float val, float max, float step, float inc_per_pixel) => nk_property_float(ctx, name, min, (float*)Utilities.As(ref val), max, step, inc_per_pixel);
        public static void PropertyDouble(string name, double min, ref double val, double max, double step, float inc_per_pixel) => nk_property_double(ctx, name, min, (double*)Utilities.As(ref val), max, step, inc_per_pixel);
        public static int Propertyi(string name, int min, int val, int max, int step, float inc_per_pixel) => nk_propertyi(ctx, name, min, val, max, step, inc_per_pixel);
        public static float Propertyf(string name, float min, float val, float max, float step, float inc_per_pixel) => nk_propertyf(ctx, name, min, val, max, step, inc_per_pixel);
        public static double Propertyd(string name, double min, double val, double max, double step, float inc_per_pixel) => nk_propertyd(ctx, name, min, val, max, step, inc_per_pixel);

        public static uint Edit(uint p, StringBuilder buffer, ref int len, nk_plugin_filter filter)
                =>nk_edit_string(ctx, p, buffer, (int*) Utilities.As(ref len), buffer.MaxCapacity, filter);
        public static uint Edit(uint p, StringBuilder buffer, nk_plugin_filter filter)
                => nk_edit_string_zero_terminated(ctx, p, buffer, buffer.MaxCapacity, filter);
        public static uint Edit_buffer(uint p, IntPtr text_edit/*nk_text_edit**/, nk_plugin_filter filter)
                =>nk_edit_buffer(ctx, p, text_edit, filter);
        public static void EditFocus(uint flags) => nk_edit_focus(ctx, flags);
        public static void EditUnfocus() => nk_edit_unfocus(ctx);
        
        public static int ChartBegin(nk_chart_type chart_type, int num, float min, float max)
            => nk_chart_begin(ctx, chart_type, num, min, max);
        public static int ChartBegin(nk_chart_type chart_type, nk_color c, nk_color active, int num, float min, float max)
            => nk_chart_begin_colored(ctx, chart_type, c, active, num, min, max);
        public static void ChartAddSlot(nk_chart_type chart_type, int count, float min_value, float max_value)
            => nk_chart_add_slot(ctx, chart_type, count, min_value, max_value);
        public static void ChartAddSlot(nk_chart_type chart_type, nk_color c, nk_color active, int count, float min_value, float max_value) 
            => nk_chart_add_slot_colored(ctx, chart_type, c, active, count, min_value, max_value);
        public static uint ChartPush(float p1) => nk_chart_push(ctx, p1);
        public static uint ChartPushSlot(float p1, int p2) => nk_chart_push_slot(ctx, p1, p2);
        public static void ChartEnd() => nk_chart_end(ctx);
        public static void Plot(nk_chart_type chart_type, IntPtr values, int count, int offset)
            => nk_plot(ctx, chart_type, values, count, offset);
        public static void PlotFunction(nk_chart_type chart_type, IntPtr userdata, value_getter val_getter, int count, int offset) =>
            nk_plot_function(ctx, chart_type, userdata, val_getter, count, offset);
        public static int PopupBegin(nk_popup_type type, string text, uint p1, nk_rect bounds) =>nk_popup_begin(ctx, type, text, p1, bounds);
        public static void PopupClose() => nk_popup_close(ctx);
        public static void PopupEnd() => nk_popup_end(ctx);

        public static int Combo(string[] items, int selected, int item_height, nk_vec2 size) => nk_combo(ctx, items, items.Length, selected, item_height, size);
        
        public static bool ComboBegin(string selected, nk_vec2 size) => nk_combo_begin_text(ctx, selected, selected.Length, size) != 0;
        public static bool ComboBegin(nk_color color, nk_vec2 size) => nk_combo_begin_color(ctx, color, size) != 0;
        public static bool ComboBegin(nk_symbol_type symbol, nk_vec2 size) => nk_combo_begin_symbol(ctx, symbol, size) != 0;        
        public static bool ComboBegin(string selected, nk_symbol_type symbol, nk_vec2 size) => nk_combo_begin_symbol_text(ctx, selected, selected.Length, symbol, size) != 0;
        public static bool ComboBegin(nkImage img, nk_vec2 size) => nk_combo_begin_image(ctx, img, size) != 0;
        public static bool ComboBegin(string selected, nkImage image, nk_vec2 size) => nk_combo_begin_image_text(ctx, selected, selected.Length, image, size) != 0;
        public static bool ComboItem(string text, uint alignment) => nk_combo_item_text(ctx, text, text.Length, alignment) != 0;
        public static bool ComboItem(nkImage image, string label, uint alignment) => nk_combo_item_image_text(ctx, image, label, label.Length, alignment) != 0;
        public static bool ComboItem(nk_symbol_type symbol, string text, uint alignment) => nk_combo_item_symbol_text(ctx, symbol, text, text.Length, alignment) != 0;
        public static void ComboClose() => nk_combo_close(ctx);
        public static void ComboEnd() => nk_combo_end(ctx);
        
        public static int ContextualBegin(uint p1, nk_vec2 p2, nk_rect trigger_bounds)
            => nk_contextual_begin(ctx, p1, p2, trigger_bounds);
        public static int ContextualItem(string label, uint align)
            => nk_contextual_item_text(ctx, label, label.Length, align); 
        public static int ContextualItem(nkImage image, string text, uint alignment)
            => nk_contextual_item_image_text(ctx, image, text, text.Length, alignment); 
        public static int ContextualItem(nk_symbol_type symbol, string text, uint alignment)
            => nk_contextual_item_symbol_text(ctx, symbol, text, text.Length, alignment);        
        public static void ContextualClose() => nk_contextual_close(ctx);           
        public static void ContextualEnd() => nk_contextual_end(ctx);      
        public static void Tooltip(string tip) => nk_tooltip(ctx, tip);                 
        public static int TooltipBegin(float width) => nk_tooltip_begin(ctx, width);
        public static void TooltipEnd() => nk_tooltip_end(ctx); 

        public static void MenubarBegin() => nk_menubar_begin(ctx);
        public static void MenubarEnd() => nk_menubar_end(ctx);
        public static bool MenuBegin(string title, nk_text_alignment align, nk_vec2 size) => nk_menu_begin_text(ctx, title, title.Length, (uint)align, size) != 0;
        public static bool MenuBegin(string label, nkImage image, nk_vec2 size) => nk_menu_begin_image(ctx, label, image, size) != 0;
        public static bool MenuBegin(string label, nk_text_alignment align, nkImage image, nk_vec2 size) => nk_menu_begin_image_text(ctx, label, label.Length, (uint)align, image, size) != 0;      
        public static bool MenuBegin( string label, nk_symbol_type symbol, nk_vec2 size)
            => nk_menu_begin_symbol(ctx, label, symbol, size) != 0;
        public static bool MenuBegin( string text, nk_text_alignment align, nk_symbol_type symbol, nk_vec2 size)
            => nk_menu_begin_symbol_text(ctx, text, text.Length, (uint)align, symbol, size) != 0;
        public static bool MenuItem(string text, nk_text_alignment align) 
            => nk_menu_item_text(ctx, text, text.Length, (uint)align) != 0;
        public static bool MenuItem( nkImage image, string text, uint alignment)
            => nk_menu_item_image_text(ctx, image, text, text.Length, alignment) != 0;
        public static bool MenuItem(nk_symbol_type symol, string text, nk_text_alignment align) 
            => nk_menu_item_symbol_text(ctx, symol, text, text.Length, (uint)align) != 0;
        public static void MenuClose() => nk_menu_close(ctx);
        public static void MenuEnd() => nk_menu_end(ctx);


    }
}
