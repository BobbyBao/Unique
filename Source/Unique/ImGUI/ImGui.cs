using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    using ImVec2 = Vector2;
    using ImVec4 = Vector4;



    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct ImGuiStyle
    {
        public float Alpha;                      // Global alpha applies to everything in ImGui
        public ImVec2 WindowPadding;              // Padding within a window
        public ImVec2 WindowMinSize;              // Minimum window size
        public float WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
        public ImVec2 WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
        public float ChildWindowRounding;        // Radius of child window corners rounding. Set to 0.0f to have rectangular windows
        public ImVec2 FramePadding;               // Padding within a framed rectangle (used by most widgets)
        public float FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
        public ImVec2 ItemSpacing;                // Horizontal and vertical spacing between widgets/lines
        public ImVec2 ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
        public ImVec2 TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
        public float IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
        public float ColumnsMinSpacing;          // Minimum horizontal spacing between two columns
        public float ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar
        public float ScrollbarRounding;          // Radius of grab corners for scrollbar
        public float GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
        public float GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
        public ImVec2 ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f,0.5f) for horizontally+vertically centered.
        public ImVec2 DisplayWindowPadding;       // Window positions are clamped to be visible within the display area by at least this amount. Only covers regular windows.
        public ImVec2 DisplaySafeAreaPadding;     // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
        public Bool AntiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
        public Bool AntiAliasedShapes;          // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
        public float CurveTessellationTol;       // Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.

        //ImVec4 Colors[ImGuiColor.COUNT];

        public fixed float Colors[(int)ImGuiColor.COUNT * 4];
        //IMGUI_API ImGuiStyle();
        //IMGUI_API void ScaleAllSizes(float scale_factor);
    };

    [SuppressUnmanagedCodeSecurity]
    public unsafe static partial class ImGui
    {
        public static bool Button(string label)
        {
            ImVec2 sz = new ImVec2();
            return Button( label, ref sz);
        }
    }
}
