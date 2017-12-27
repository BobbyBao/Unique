using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    using ImVec2 = Vector2;
    using ImVec4 = Vector4;
    using ImGuiID = System.UInt32;
    using ImU32 = System.UInt32;

    [SuppressUnmanagedCodeSecurity]
    public unsafe static partial class ImGui
    {
        [DllImport(Native.DllName, EntryPoint = "ImGui_GetStyle", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImGuiStyle* GetStyle();

        // Window
        #region WINDOW
        [return: MarshalAs(UnmanagedType.U1)]
        [DllImport(Native.DllName, EntryPoint = "ImGui_Begin", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Begin(IntPtr name, IntPtr p_open/* = IntPtr.Zero*/, ImGuiWindowFlags flags = 0);                                                   // push window to the stack and start appending to it. see .cpp for details. return false when window is collapsed, so you can early out in your code. 'bool* p_open' creates a widget on the upper-right to close the window (which sets your bool to false).

        [DllImport(Native.DllName, EntryPoint = "ImGui_End", CallingConvention = CallingConvention.Cdecl)]
        public static extern void End();                                                                                                                      // finish appending to current window, pop it off the window stack.

        [return: MarshalAs(UnmanagedType.U1)]
        [DllImport(Native.DllName, EntryPoint = "ImGui_BeginChild", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool BeginChild(IntPtr str_id, ref ImVec2 size /*= ImVec2(0,0)*/, bool border = false, ImGuiWindowFlags extra_flags = 0);    // begin a scrolling region. size==0.0f: use remaining window size, size<0.0f: use remaining window size minus abs(size). size>0.0f: fixed size. each axis can use a different mode, e.g. ImVec2(0,400).

        [return: MarshalAs(UnmanagedType.U1)]
        [DllImport(Native.DllName, EntryPoint = "ImGui_BeginChild_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool BeginChild(ImGuiID id, ref ImVec2 size/* = ImVec2(0,0)*/, bool border = false, ImGuiWindowFlags extra_flags = 0);            // "
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_EndChild", CallingConvention = CallingConvention.Cdecl)]
        public static extern void EndChild();

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetContentRegionMax", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetContentRegionMax();                                              // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetContentRegionAvail", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetContentRegionAvail();                                            // == GetContentRegionMax() - GetCursorPos()

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetContentRegionAvailWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetContentRegionAvailWidth();                                       //

        [DllImport(Native.DllName, EntryPoint = "GetWindowContentRegionMin", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 ImGui_GetWindowContentRegionMin();                                        // content boundaries min (roughly (0,0)-Scroll), in window coordinates

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetWindowContentRegionMax", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetWindowContentRegionMax();                                        // content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetWindowContentRegionWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetWindowContentRegionWidth();                                      //

        //[DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        //public static extern  ImDrawList*   GetWindowDrawList();                                                // get rendering command-list if you want to append your own draw primitives

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetWindowPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetWindowPos();                                                     // get current window position in screen space (useful if you want to do your own drawing via the DrawList api)

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetWindowSize", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetWindowSize();                                                    // get current window size

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetWindowWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetWindowWidth();

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetWindowHeight", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetWindowHeight();

        [return: MarshalAs(UnmanagedType.U1)]
        [DllImport(Native.DllName, EntryPoint = "ImGui_IsWindowCollapsed", CallingConvention = CallingConvention.Cdecl)]
        public static extern  bool IsWindowCollapsed();

        [return: MarshalAs(UnmanagedType.U1)]
        [DllImport(Native.DllName, EntryPoint = "ImGui_IsWindowAppearing", CallingConvention = CallingConvention.Cdecl)]
        public static extern  bool IsWindowAppearing();

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowFontScale", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowFontScale(float scale);

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetNextWindowPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetNextWindowPos(ref ImVec2 pos, ImGuiCond cond, ref ImVec2 pivot); // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetNextWindowSize", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetNextWindowSize(ref ImVec2 size, ImGuiCond cond = 0);          // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()

       // [DllImport(Native.DllName, EntryPoint = "ImGui_SetNextWindowSizeConstraints", CallingConvention = CallingConvention.Cdecl)]
       // public static extern void SetNextWindowSizeConstraints(ref ImVec2 size_min, ref ImVec2 size_max, ImGuiSizeConstraintCallback custom_callback, IntPtr custom_callback_data); // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Use callback to apply non-trivial programmatic constraints.

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetNextWindowContentSize", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetNextWindowContentSize(ref ImVec2 size);                       // set next window content size (enforce the range of scrollbars). set axis to 0.0f to leave it automatic. call before Begin()

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetNextWindowContentWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetNextWindowContentWidth(float width);                             // set next window content width (enforce the range of horizontal scrollbar). call before Begin()

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetNextWindowCollapsed", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0);         // set next window collapsed state. call before Begin()

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetNextWindowFocus", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetNextWindowFocus();                                               // set next window to be focused / front-most. call before Begin()

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowPos(ref ImVec2 pos, ImGuiCond cond = 0);                // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowSize", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowSize(ref ImVec2 size, ImGuiCond cond = 0);              // (not recommended) set current window size - call within Begin()/End(). set to ImVec2(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.    

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowCollapsed", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowCollapsed(bool collapsed, ImGuiCond cond = 0);             // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowFocus", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowFocus();                                                   // (not recommended) set current window to be focused / front-most. prefer using SetNextWindowFocus().

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowPos(IntPtr name, ref ImVec2 pos, ImGuiCond cond = 0);      // set named window position.

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowSize", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowSize(IntPtr name, ref ImVec2 size, ImGuiCond cond = 0);    // set named window size. set axis to 0.0f to force an auto-fit on this axis.

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowCollapsed", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowCollapsed(IntPtr name, [MarshalAs(UnmanagedType.U1)] bool collapsed, ImGuiCond cond = 0);   // set named window collapsed state

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetWindowFocus", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetWindowFocus(IntPtr name);


        [DllImport(Native.DllName, EntryPoint = "ImGui_GetScrollX", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetScrollX();                                                       // get scrolling amount [0..GetScrollMaxX()]
        [DllImport(Native.DllName, EntryPoint = "ImGui_GetScrollY", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetScrollY();                                                       // get scrolling amount [0..GetScrollMaxY()]
        [DllImport(Native.DllName, EntryPoint = "ImGui_GetScrollMaxX", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetScrollMaxX();                                                    // get maximum scrolling amount ~~ ContentSize.X - WindowSize.X
        [DllImport(Native.DllName, EntryPoint = "ImGui_GetScrollMaxY", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetScrollMaxY();                                                    // get maximum scrolling amount ~~ ContentSize.Y - WindowSize.Y
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetScrollX", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetScrollX(float scroll_x);                                         // set scrolling amount [0..GetScrollMaxX()]
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetScrollY", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetScrollY(float scroll_y);                                         // set scrolling amount [0..GetScrollMaxY()]
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetScrollHere", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetScrollHere(float center_y_ratio = 0.5f);                         // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom.
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetScrollFromPosY", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetScrollFromPosY(float pos_y, float center_y_ratio = 0.5f);        // adjust scrolling amount to make given position valid. use GetCursorPos() or GetCursorStartPos()+offset to get valid positions.
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetKeyboardFocusHere", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetKeyboardFocusHere(int offset = 0);                               // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetStateStorage", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetStateStorage(IntPtr tree);                                // replace tree state storage with our own (if you want to manipulate it yourself, typically clear subsection of it)
        [DllImport(Native.DllName, EntryPoint = "ImGui_GetStateStorage", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetStateStorage();

        #endregion WINDOW


        // Parameters stacks (shared)
        [DllImport(Native.DllName, EntryPoint = "ImGui_PushFont", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushFont(IntPtr font);                                             // use NULL as a shortcut to push default font

        [DllImport(Native.DllName, EntryPoint = "ImGui_PopFont", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PopFont();

        [DllImport(Native.DllName, EntryPoint = "ImGui_PushStyleColor", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushStyleColor(ImGuiColor idx, ImU32 col);

        [DllImport(Native.DllName, EntryPoint = "ImGui_PushStyleColor", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushStyleColor(ImGuiColor idx, ref ImVec4 col);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_PopStyleColor_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PopStyleColor(int count = 1);

        [DllImport(Native.DllName, EntryPoint = "ImGui_PushStyleVar", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushStyleVar(ImGuiStyleVar idx, float val);

        [DllImport(Native.DllName, EntryPoint = "ImGui_PushStyleVar_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushStyleVar(ImGuiStyleVar idx, ref ImVec2 val);

        [DllImport(Native.DllName, EntryPoint = "ImGui_PopStyleVar", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PopStyleVar(int count = 1);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetStyleColorVec4", CallingConvention = CallingConvention.Cdecl)]
        public static extern ref ImVec4 GetStyleColorVec4(ImGuiColor idx);                                    // retrieve style color as stored in ImGuiStyle structure. use to feed back into PushStyleColor(), otherwhise use GetColorU32() to get style color + style alpha.

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetFont", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetFont();                                                          // get current font

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetFontSize", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetFontSize();                                                      // get current font size (= height in pixels) of current font with current scale applied

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetFontTexUvWhitePixel", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetFontTexUvWhitePixel();                                           // get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColorU32", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImU32 GetColorU32(ImGuiColor idx, float alpha_mul = 1.0f);                  // retrieve given style color with style alpha applied and optional extra alpha multiplier

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColorU32_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImU32 GetColorU32(ref ImVec4 col);                                     // retrieve given color with style alpha applied
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColorU32_2", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImU32 GetColorU32(ImU32 col);


        // Parameters stacks (current window)

        [DllImport(Native.DllName, EntryPoint = "ImGui_PushItemWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushItemWidth(float item_width);                                    // width of items for the common item+label case, pixels. 0.0f = default to ~2/3 of windows width, >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)

        [DllImport(Native.DllName, EntryPoint = "ImGui_PopItemWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PopItemWidth();

        [DllImport(Native.DllName, EntryPoint = "ImGui_CalcItemWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern float CalcItemWidth();                                                    // width of item given pushed settings and current cursor position

        [DllImport(Native.DllName, EntryPoint = "ImGui_PushTextWrapPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushTextWrapPos(float wrap_pos_x = 0.0f);                           // word-wrapping for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space

        [DllImport(Native.DllName, EntryPoint = "ImGui_PopTextWrapPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PopTextWrapPos();

        [DllImport(Native.DllName, EntryPoint = "ImGui_PushAllowKeyboardFocus", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushAllowKeyboardFocus(bool allow_keyboard_focus);                  // allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets

        [DllImport(Native.DllName, EntryPoint = "ImGui_PopAllowKeyboardFocus", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PopAllowKeyboardFocus();

        [DllImport(Native.DllName, EntryPoint = "ImGui_PushButtonRepeat", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushButtonRepeat(bool repeat);                                      // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.

        [DllImport(Native.DllName, EntryPoint = "ImGui_PopButtonRepeat", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PopButtonRepeat();



        // Cursor / Layout

        [DllImport(Native.DllName, EntryPoint = "ImGui_Separator", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Separator();                                                        // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.

        [DllImport(Native.DllName, EntryPoint = "ImGui_SameLine", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SameLine(float pos_x = 0.0f, float spacing_w = -1.0f);              // call between widgets or groups to layout them horizontally

        [DllImport(Native.DllName, EntryPoint = "ImGui_NewLine", CallingConvention = CallingConvention.Cdecl)]
        public static extern void NewLine();                                                          // undo a SameLine()

        [DllImport(Native.DllName, EntryPoint = "ImGui_Spacing", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Spacing();                                                          // add vertical spacing

        [DllImport(Native.DllName, EntryPoint = "ImGui_Dummy", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Dummy(ref ImVec2 size);                                          // add a dummy item of given size
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_Indent", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Indent(float indent_w = 0.0f);                                      // move content position toward the right, by style.IndentSpacing or indent_w if >0

        [DllImport(Native.DllName, EntryPoint = "ImGui_Unindent", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Unindent(float indent_w = 0.0f);                                    // move content position back to the left, by style.IndentSpacing or indent_w if >0

        [DllImport(Native.DllName, EntryPoint = "ImGui_BeginGroup", CallingConvention = CallingConvention.Cdecl)]
        public static extern void BeginGroup();                                                       // lock horizontal starting position + capture group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)

        [DllImport(Native.DllName, EntryPoint = "ImGui_EndGroup", CallingConvention = CallingConvention.Cdecl)]
        public static extern void EndGroup();

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetCursorPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetCursorPos();                                                     // cursor position is relative to window position

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetCursorPosX", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetCursorPosX();                                                    // "

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetCursorPosY", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetCursorPosY();                                                    // "

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetCursorPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetCursorPos(ref ImVec2 local_pos);                              // "
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetCursorPosX", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetCursorPosX(float x);                                             // "

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetCursorPosY", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetCursorPosY(float y);                                             // "

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetCursorStartPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetCursorStartPos();                                                // initial cursor position

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetCursorScreenPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImVec2 GetCursorScreenPos();                                               // cursor position in absolute screen coordinates [0..io.DisplaySize] (useful to work with ImDrawList API)

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetCursorScreenPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetCursorScreenPos(ref ImVec2 pos);                              // cursor position in absolute screen coordinates [0..io.DisplaySize]
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_AlignTextToFramePadding", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AlignTextToFramePadding();                                          // vertically align/lower upcoming text to FramePadding.y so that it will aligns to upcoming widgets (call if you have text on a line before regular widgets)

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetTextLineHeight", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetTextLineHeight();                                                // height of font == GetWindowFontSize()

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetTextLineHeightWithSpacing", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetTextLineHeightWithSpacing();                                     // distance (in pixels) between 2 consecutive lines of text == GetWindowFontSize() + GetStyle().ItemSpacing.y

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetItemsLineHeightWithSpacing", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetItemsLineHeightWithSpacing();                                    // distance (in pixels) between 2 consecutive lines of standard height widgets == GetWindowFontSize() + GetStyle().FramePadding.y*2 + GetStyle().ItemSpacing.y



        // Columns
        // You can also use SameLine(pos_x) for simplified columns. The columns API is still work-in-progress and rather lacking.

        [DllImport(Native.DllName, EntryPoint = "ImGui_Columns", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Columns(int count = 1, [MarshalAs(UnmanagedType.LPStr)]string id = null, [MarshalAs(UnmanagedType.U1)] bool border = true);

        [DllImport(Native.DllName, EntryPoint = "ImGui_NextColumn", CallingConvention = CallingConvention.Cdecl)]
        public static extern void NextColumn();                                                       // next column, defaults to current row or next row if the current row is finished

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnIndex", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetColumnIndex();                                                   // get current column index

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetColumnWidth(int column_index = -1);                              // get column width (in pixels). pass -1 to use current column

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetColumnWidth", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetColumnWidth(int column_index, float width);                      // set column width (in pixels). pass -1 to use current column

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnOffset", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetColumnOffset(int column_index = -1);                             // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetColumnOffset", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetColumnOffset(int column_index, float offset_x);                  // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetColumnsCount();

        // ID scopes
        // If you are creating widgets in a loop you most likely want to push a unique identifier (e.g. object pointer, loop index) so ImGui can differentiate them.
        // You can also use the "##foobar" syntax within widget label to distinguish them from each others. Read "A primer on the use of labels/IDs" in the FAQ for more details.

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushID([MarshalAs(UnmanagedType.LPStr)]string str_id);                                         // push identifier into the ID stack. IDs are hash of the entire stack!

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushID([MarshalAs(UnmanagedType.LPStr)]string str_id_begin, [MarshalAs(UnmanagedType.LPStr)]string str_id_end);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushID(IntPtr ptr_id);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PushID(int int_id);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PopID();

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImGuiID GetID([MarshalAs(UnmanagedType.LPStr)]string str_id);                                          // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into ImGuiStorage yourself

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImGuiID GetID([MarshalAs(UnmanagedType.LPStr)]string str_id_begin, [MarshalAs(UnmanagedType.LPStr)]string str_id_end);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern ImGuiID GetID(IntPtr ptr_id);

        // Widgets: Text

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void TextUnformatted([MarshalAs(UnmanagedType.LPStr)]string text, [MarshalAs(UnmanagedType.LPStr)]string text_end /*= NULL*/);               // doesn't require null terminated string if 'text_end' is specified. no copy done, no limits, recommended for long chunks of text

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Text([MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...)                                     IM_FMTARGS(1); // simple formatted text

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        //public static extern void TextV([MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args)                           IM_FMTLIST(1);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void TextColored(ref ImVec4 col, [MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...)           IM_FMTARGS(2); // shortcut for PushStyleColor(ImGuiCol_Text, col); Text(fmt, ...); PopStyleColor();

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        //public static extern void TextColoredV(ref ImVec4 col, [MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args) IM_FMTLIST(2);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void TextDisabled([MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...)                             IM_FMTARGS(1); // shortcut for PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); PopStyleColor();

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        //public static extern void TextDisabledV([MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args)                   IM_FMTLIST(1);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void TextWrapped([MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...)                              IM_FMTARGS(1); // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        // public static extern void TextWrappedV([MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args)                    IM_FMTLIST(1);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void LabelText([MarshalAs(UnmanagedType.LPStr)]string label, [MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...)             IM_FMTARGS(2); // display text+label aligned the same way as value+label widgets

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        // public static extern void LabelTextV([MarshalAs(UnmanagedType.LPStr)]string label, [MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args)   IM_FMTLIST(2);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void BulletText([MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...)                               IM_FMTARGS(1); // shortcut for Bullet()+Text()

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        //public static extern void BulletTextV([MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args)                     IM_FMTLIST(1);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Bullet();                                                                     // draw a small circle and keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

        // Widgets: Main

        [DllImport(Native.DllName, EntryPoint = "ImGui_Button", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Button([MarshalAs(UnmanagedType.LPStr)]string label, ref ImVec2 size /*= ImVec2(0,0)*/);            // button
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_SmallButton", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SmallButton([MarshalAs(UnmanagedType.LPStr)]string label);                                         // button with FramePadding=(0,0) to easily embed within text

        [DllImport(Native.DllName, EntryPoint = "ImGui_InvisibleButton", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InvisibleButton([MarshalAs(UnmanagedType.LPStr)]string str_id, ref ImVec2 size);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_Image", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Image(IntPtr user_texture_id, ref ImVec2 size, ref ImVec2 uv0 /*= ImVec2(0,0)*/, ref ImVec2 uv1/* = ImVec2(1,1)*/, ref ImVec4 tint_col /*= ImVec4(1,1,1,1)*/, ref ImVec4 border_col /*= ImVec4(0,0,0,0)*/);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_ImageButton", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool ImageButton(IntPtr user_texture_id, ref ImVec2 size, ref ImVec2 uv0 /*= ImVec2(0,0)*/,  ref ImVec2 uv1 /*= ImVec2(1,1)*/, int frame_padding/* = -1*/, ref ImVec4 bg_col/* = ImVec4(0,0,0,0)*/, ref ImVec4 tint_col/* = ImVec4(1,1,1,1)*/);    // <0 frame_padding uses default frame padding settings. 0 for no padding
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_Checkbox", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Checkbox([MarshalAs(UnmanagedType.LPStr)]string label, Bool* v);

        [DllImport(Native.DllName, EntryPoint = "ImGui_CheckboxFlags", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CheckboxFlags([MarshalAs(UnmanagedType.LPStr)]string label, uint* flags, uint flags_value);

        [DllImport(Native.DllName, EntryPoint = "ImGui_RadioButton", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool RadioButton([MarshalAs(UnmanagedType.LPStr)]string label, bool active);

        [DllImport(Native.DllName, EntryPoint = "ImGui_RadioButton_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool RadioButton([MarshalAs(UnmanagedType.LPStr)]string label, int* v, int v_button);

        [DllImport(Native.DllName, EntryPoint = "ImGui_Combo", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Combo([MarshalAs(UnmanagedType.LPStr)]string label, int* current_item, [MarshalAs(UnmanagedType.LPStr)]string[] items, int items_count, int height_in_items = -1);

        [DllImport(Native.DllName, EntryPoint = "ImGui_Combo_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Combo([MarshalAs(UnmanagedType.LPStr)]string label, int* current_item, [MarshalAs(UnmanagedType.LPStr)]string items_separated_by_zeros, int height_in_items = -1);    // separate items with \0, end item-list with \0\0

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        //public static extern bool Combo([MarshalAs(UnmanagedType.LPStr)]string label, int* current_item, bool (* items_getter) (void* data, int idx, [MarshalAs(UnmanagedType.LPStr)]string* out_text), void* data, int items_count, int height_in_items = -1);

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlotLines([MarshalAs(UnmanagedType.LPStr)]string label, float* values, int values_count, int values_offset /*= 0*/, [MarshalAs(UnmanagedType.LPStr)]string overlay_text /*= NULL*/, float scale_min /*= FLT_MAX*/, float scale_max /*= FLT_MAX*/, ImVec2 graph_size /*= ImVec2(0,0)*/, int stride = sizeof(float));

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        //public static extern void PlotLines([MarshalAs(UnmanagedType.LPStr)]string label, float (* values_getter) (void* data, int idx), void* data, int values_count, int values_offset = 0, [MarshalAs(UnmanagedType.LPStr)]string overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0,0));
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlotHistogram([MarshalAs(UnmanagedType.LPStr)]string label, float* values, int values_count, int values_offset /*= 0*/, [MarshalAs(UnmanagedType.LPStr)]string overlay_text /*= NULL*/, float scale_min/* = FLT_MAX*/, float scale_max /*= FLT_MAX*/, ImVec2 graph_size /*= ImVec2(0,0)*/, int stride = sizeof(float));

        //[DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
       // public static extern void PlotHistogram([MarshalAs(UnmanagedType.LPStr)]string label, float (* values_getter) (void* data, int idx), void* data, int values_count, int values_offset = 0, [MarshalAs(UnmanagedType.LPStr)]string overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0,0));
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_GetColumnsCount", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ProgressBar(float fraction, ref ImVec2 size_arg /*= ImVec2(-1,0)*/, [MarshalAs(UnmanagedType.LPStr)]string overlay /*= NULL*/);

        

        // Widgets: Drags (tip: ctrl+click on a drag box to input with keyboard. manually input values aren't clamped, can go off-bounds)
        // For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragFloat", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragFloat([MarshalAs(UnmanagedType.LPStr)]string label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);     // If v_min >= v_max we have no bound

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragFloat2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragFloat2([MarshalAs(UnmanagedType.LPStr)]string label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragFloat3", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragFloat3([MarshalAs(UnmanagedType.LPStr)]string label, float* v/*[3]*/, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragFloat4", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragFloat4([MarshalAs(UnmanagedType.LPStr)]string label, float* v/*[4]*/, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragFloatRange2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragFloatRange2([MarshalAs(UnmanagedType.LPStr)]string label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", [MarshalAs(UnmanagedType.LPStr)]string display_format_max = null, float power = 1.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragInt", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragInt([MarshalAs(UnmanagedType.LPStr)]string label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");                                       // If v_min >= v_max we have no bound

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragInt2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragInt2([MarshalAs(UnmanagedType.LPStr)]string label, int* v/*[2]*/, float v_speed = 1.0f, int v_min = 0, int v_max = 0, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragInt3", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragInt3([MarshalAs(UnmanagedType.LPStr)]string label, int* v/*[3]*/, float v_speed = 1.0f, int v_min = 0, int v_max = 0, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragInt4", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragInt4([MarshalAs(UnmanagedType.LPStr)]string label, int* v/*[4]*/, float v_speed = 1.0f, int v_min = 0, int v_max = 0, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");

        [DllImport(Native.DllName, EntryPoint = "ImGui_DragIntRange2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DragIntRange2([MarshalAs(UnmanagedType.LPStr)]string label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f", [MarshalAs(UnmanagedType.LPStr)]string display_format_max = null);

        // Widgets: Input with Keyboard

        public unsafe delegate int TextEditCallback(TextEditCallbackData* data);

        [DllImport(Native.DllName, EntryPoint = "ImGui_InputText", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputText([MarshalAs(UnmanagedType.LPStr)]string label, IntPtr buf, uint buf_size, ImGuiInputTextFlags flags/* = 0*/, TextEditCallback callback/* = NULL*/, void* user_data/* = NULL*/);

        [DllImport(Native.DllName, EntryPoint = "ImGui_InputTextMultiline", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputTextMultiline([MarshalAs(UnmanagedType.LPStr)]string label, IntPtr buf, uint buf_size, ref ImVec2 size /*= ImVec2(0,0)*/, ImGuiInputTextFlags flags/* = 0*/, TextEditCallback callback/* = NULL*/, void* user_data/* = NULL*/);

        [DllImport(Native.DllName, EntryPoint = "ImGui_InputFloat", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputFloat([MarshalAs(UnmanagedType.LPStr)]string label, ref float v, float step /*= 0.0f*/, float step_fast /*= 0.0f*/, int decimal_precision/* = -1*/, ImGuiInputTextFlags extra_flags/* = 0*/);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_InputFloat2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputFloat2([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector2 v, int decimal_precision/* = -1*/, ImGuiInputTextFlags extra_flags/* = 0*/);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_InputFloat3", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputFloat3([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector3 v, int decimal_precision/* = -1*/, ImGuiInputTextFlags extra_flags/* = 0*/);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_InputFloat4", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputFloat4([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector4 v, int decimal_precision = -1, ImGuiInputTextFlags extra_flags = 0);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_InputInt", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputInt([MarshalAs(UnmanagedType.LPStr)]string label, ref int v, int step = 1, int step_fast = 100, ImGuiInputTextFlags extra_flags = 0);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_InputInt2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputInt2([MarshalAs(UnmanagedType.LPStr)]string label, ref IntVector2 v, ImGuiInputTextFlags extra_flags = 0);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_InputInt3", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InputInt3([MarshalAs(UnmanagedType.LPStr)]string label, ref IntVector3 v, ImGuiInputTextFlags extra_flags = 0);

        //[DllImport(Native.DllName, EntryPoint = "ImGui_DragIntRange2", CallingConvention = CallingConvention.Cdecl)]
        //public static extern bool InputInt4([MarshalAs(UnmanagedType.LPStr)]string label, int v[4], ImGuiInputTextFlags extra_flags = 0);


        // Widgets: Sliders (tip: ctrl+click on a slider to input with keyboard. manually input values aren't clamped, can go off-bounds)
        [DllImport(Native.DllName, EntryPoint = "ImGui_SliderFloat", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SliderFloat([MarshalAs(UnmanagedType.LPStr)]string label, ref float v, float v_min, float v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);     // adjust display_format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for logarithmic sliders

        [DllImport(Native.DllName, EntryPoint = "ImGui_SliderFloat2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SliderFloat2([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector2 v, float v_min, float v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_SliderFloat3", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SliderFloat3([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector3 v, float v_min, float v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_SliderFloat4", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SliderFloat4([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector4 v, float v_min, float v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_SliderAngle", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SliderAngle([MarshalAs(UnmanagedType.LPStr)]string label, ref float v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_SliderInt", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SliderInt([MarshalAs(UnmanagedType.LPStr)]string label, ref int v, int v_min, int v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");

        [DllImport(Native.DllName, EntryPoint = "ImGui_SliderInt2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SliderInt2([MarshalAs(UnmanagedType.LPStr)]string label, ref IntVector2 v, int v_min, int v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");

        [DllImport(Native.DllName, EntryPoint = "ImGui_SliderInt3", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SliderInt3([MarshalAs(UnmanagedType.LPStr)]string label, ref IntVector3 v, int v_min, int v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");

        //[DllImport(Native.DllName, EntryPoint = "ImGui_DragIntRange2", CallingConvention = CallingConvention.Cdecl)]
        //public static extern bool SliderInt4([MarshalAs(UnmanagedType.LPStr)]string label, int v[4], int v_min, int v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");

        [DllImport(Native.DllName, EntryPoint = "ImGui_VSliderFloat", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool VSliderFloat([MarshalAs(UnmanagedType.LPStr)]string label, ref ImVec2 size, float* v, float v_min, float v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.3f", float power = 1.0f);

        [DllImport(Native.DllName, EntryPoint = "ImGui_VSliderInt", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool VSliderInt([MarshalAs(UnmanagedType.LPStr)]string label, ref ImVec2 size, int* v, int v_min, int v_max, [MarshalAs(UnmanagedType.LPStr)]string display_format = "%.0f");

        // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
        // Note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can the pass the address of a first float element out of a contiguous structure, e.g. &myvector.x

        [DllImport(Native.DllName, EntryPoint = "ImGui_ColorEdit3", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool ColorEdit3([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector3 col, ImGuiColorEditFlags flags = 0);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_ColorEdit4", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool ColorEdit4([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector4 col, ImGuiColorEditFlags flags = 0);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_ColorPicker3", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool ColorPicker3([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector3 col, ImGuiColorEditFlags flags = 0);
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_ColorPicker4", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool ColorPicker4([MarshalAs(UnmanagedType.LPStr)]string label, ref Vector4 col, ImGuiColorEditFlags flags /*= 0*/, IntPtr ref_col);

        [DllImport(Native.DllName, EntryPoint = "ImGui_ColorButton", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool ColorButton([MarshalAs(UnmanagedType.LPStr)]string desc_id, ref Vector4 col, ImGuiColorEditFlags flags /*= 0*/, ImVec2 size /*= ImVec2(0,0)*/);  // display a colored square/button, hover for details, return true when pressed.
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_SetColorEditOptions", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetColorEditOptions(ImGuiColorEditFlags flags);                         // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.


        // Widgets: Trees

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreeNode", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool TreeNode([MarshalAs(UnmanagedType.LPStr)]string label);                                            // if returning 'true' the node is open and the tree id is pushed into the id stack. user is responsible for calling TreePop().

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreeNode_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool TreeNode([MarshalAs(UnmanagedType.LPStr)]string str_id, [MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...) IM_FMTARGS(2);       // read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreeNode_2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool TreeNode(IntPtr ptr_id, [MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...) IM_FMTARGS(2);       // "

        //[DllImport(Native.DllName, EntryPoint = "ImGui_SetColorEditOptions", CallingConvention = CallingConvention.Cdecl)]
        //public static extern bool TreeNodeV([MarshalAs(UnmanagedType.LPStr)]string str_id, [MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args) IM_FMTLIST(2);

       // [DllImport(Native.DllName, EntryPoint = "ImGui_SetColorEditOptions", CallingConvention = CallingConvention.Cdecl)]
       // public static extern bool TreeNodeV(IntPtr ptr_id, [MarshalAs(UnmanagedType.LPStr)]string fmt);//, va_list args) IM_FMTLIST(2);

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreeNodeEx", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool TreeNodeEx([MarshalAs(UnmanagedType.LPStr)]string label, ImGuiTreeNodeFlags flags = 0);

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreeNodeEx_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool TreeNodeEx([MarshalAs(UnmanagedType.LPStr)]string str_id, ImGuiTreeNodeFlags flags, [MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...) IM_FMTARGS(3);

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreeNodeEx_2", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool TreeNodeEx(IntPtr ptr_id, ImGuiTreeNodeFlags flags, [MarshalAs(UnmanagedType.LPStr)]string fmt);//, ...) IM_FMTARGS(3);

        //[DllImport(Native.DllName, EntryPoint = "ImGui_TreeNodeExV", CallingConvention = CallingConvention.Cdecl)]
        //public static extern bool TreeNodeExV([MarshalAs(UnmanagedType.LPStr)]string str_id, ImGuiTreeNodeFlags flags, [MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args) IM_FMTLIST(3);

        //[DllImport(Native.DllName, EntryPoint = "ImGui_TreeNodeExV_1", CallingConvention = CallingConvention.Cdecl)]
        //public static extern bool TreeNodeExV(IntPtr ptr_id, ImGuiTreeNodeFlags flags, [MarshalAs(UnmanagedType.LPStr)]string fmt, va_list args) IM_FMTLIST(3);

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreePush", CallingConvention = CallingConvention.Cdecl)]
        public static extern void TreePush([MarshalAs(UnmanagedType.LPStr)]string str_id /*= NULL*/);                                    // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call Push/Pop yourself for layout purpose

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreePush_1", CallingConvention = CallingConvention.Cdecl)]
        public static extern void TreePush(IntPtr ptr_id /*= NULL*/);                                    // "

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreePop", CallingConvention = CallingConvention.Cdecl)]
        public static extern void TreePop();                                                              // ~ Unindent()+PopId()

        [DllImport(Native.DllName, EntryPoint = "ImGui_TreeAdvanceToLabelPos", CallingConvention = CallingConvention.Cdecl)]
        public static extern void TreeAdvanceToLabelPos();                                                // advance cursor x position by GetTreeNodeToLabelSpacing()

        [DllImport(Native.DllName, EntryPoint = "ImGui_GetTreeNodeToLabelSpacing", CallingConvention = CallingConvention.Cdecl)]
        public static extern float GetTreeNodeToLabelSpacing();                                            // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode

        [DllImport(Native.DllName, EntryPoint = "ImGui_SetNextTreeNodeOpen", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetNextTreeNodeOpen(bool is_open, ImGuiCond cond = 0);               // set next TreeNode/CollapsingHeader open state.

        [DllImport(Native.DllName, EntryPoint = "ImGui_CollapsingHeader", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CollapsingHeader([MarshalAs(UnmanagedType.LPStr)]string label, ImGuiTreeNodeFlags flags = 0);      // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
    
        [DllImport(Native.DllName, EntryPoint = "ImGui_CollapsingHeader", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CollapsingHeader([MarshalAs(UnmanagedType.LPStr)]string label, bool* p_open, ImGuiTreeNodeFlags flags = 0); // when 'p_open' isn't NULL, display an additional small close button on upper right of the header

    }
}
