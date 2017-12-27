using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Text;
using Unique.Engine;

namespace Unique.Editor
{
    class ImGUI
    {

        static int output_dest = 0;
        static bool output_only_modified = false;
        static ImGuiTextFilter filter;
        static ImGuiColorEditFlags alpha_flags = 0;
        public static unsafe void ShowStyleEditor( ImGuiStyle* refStyle)
        {
            ImGuiStyle* style = ImGui.GetStyle();

            // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it compares to the default style)
            ImGuiStyle default_style = new ImGuiStyle(); // Default style
            if(ImGui.Button("Revert Style"))
                style = refStyle != null ? refStyle : &default_style;

            if(refStyle != null)
            {
                ImGui.SameLine();
                if(ImGui.Button("Save Style"))
                    *refStyle = *style;
            }

            ImGui.PushItemWidth(ImGui.GetWindowWidth() * 0.55f);

            if(ImGui.TreeNode("Rendering"))
            {
                ImGui.Checkbox("Anti-aliased lines", &style->AntiAliasedLines); ImGui.SameLine(); //ShowHelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
                ImGui.Checkbox("Anti-aliased shapes", &style->AntiAliasedShapes);
                ImGui.PushItemWidth(100);
                ImGui.DragFloat("Curve Tessellation Tolerance", &style->CurveTessellationTol, 0.02f, 0.10f, float.MaxValue, null, 2.0f);
                if(style->CurveTessellationTol < 0.0f) style->CurveTessellationTol = 0.10f;
                ImGui.DragFloat("Global Alpha", &style->Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
                ImGui.PopItemWidth();
                ImGui.TreePop();
            }

            if(ImGui.TreeNode("Settings"))
            {
                ImGui.SliderFloat2("WindowPadding", ref style->WindowPadding, 0.0f, 20.0f, "%.0f");
                ImGui.SliderFloat("WindowRounding", ref style->WindowRounding, 0.0f, 16.0f, "%.0f");
                ImGui.SliderFloat("ChildWindowRounding", ref style->ChildWindowRounding, 0.0f, 16.0f, "%.0f");
                ImGui.SliderFloat2("FramePadding", ref style->FramePadding, 0.0f, 20.0f, "%.0f");
                ImGui.SliderFloat("FrameRounding", ref style->FrameRounding, 0.0f, 16.0f, "%.0f");
                ImGui.SliderFloat2("ItemSpacing", ref style->ItemSpacing, 0.0f, 20.0f, "%.0f");
                ImGui.SliderFloat2("ItemInnerSpacing", ref style->ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
                ImGui.SliderFloat2("TouchExtraPadding", ref style->TouchExtraPadding, 0.0f, 10.0f, "%.0f");
                ImGui.SliderFloat("IndentSpacing", ref style->IndentSpacing, 0.0f, 30.0f, "%.0f");
                ImGui.SliderFloat("ScrollbarSize", ref style->ScrollbarSize, 1.0f, 20.0f, "%.0f");
                ImGui.SliderFloat("ScrollbarRounding", ref style->ScrollbarRounding, 0.0f, 16.0f, "%.0f");
                ImGui.SliderFloat("GrabMinSize", ref style->GrabMinSize, 1.0f, 20.0f, "%.0f");
                ImGui.SliderFloat("GrabRounding", ref style->GrabRounding, 0.0f, 16.0f, "%.0f");
                ImGui.Text("Alignment");
                ImGui.SliderFloat2("WindowTitleAlign", ref style->WindowTitleAlign, 0.0f, 1.0f, "%.2f");
                ImGui.SliderFloat2("ButtonTextAlign", ref style->ButtonTextAlign, 0.0f, 1.0f, "%.2f"); ImGui.SameLine(); //ShowHelpMarker("Alignment applies when a button is larger than its text content.");
                ImGui.TreePop();
            }
          /*
            if(ImGui.TreeNode("Colors"))
            {
                if(ImGui.Button("Copy Colors"))
                {
                    if(output_dest == 0)
                        ImGui.LogToClipboard();
                    else
                        ImGui.LogToTTY();
                    ImGui.LogText("ImVec4* colors = ImGui.GetStyle().Colors;" IM_NEWLINE);
                    for(int i = 0; i < ImGuiCol_COUNT; i++)
                    {
                        const ImVec4&col = style->Colors[i];
                        const char* name = ImGui.GetStyleColorName(i);
                        if(!output_only_modified || memcmp(&col, (ref ? &ref->Colors[i] : &default_style.Colors[i]), sizeof(ImVec4)) != 0)
                    ImGui.LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                ImGui.LogFinish();
            }
            ImGui.SameLine(); ImGui.PushItemWidth(120); ImGui.Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); ImGui.PopItemWidth();
            ImGui.SameLine(); ImGui.Checkbox("Only Modified Fields", &output_only_modified);
*/
            ImGui.Text("Tip: Left-click on colored square to open color picker,\nRight-click to open edit options menu.");
          
            filter.Draw("Filter colors", 200);

            ImGui.RadioButton("Opaque", &alpha_flags, 0); ImGui.SameLine();
            ImGui.RadioButton("Alpha", &alpha_flags, ImGuiColorEditFlags_AlphaPreview); ImGui.SameLine();
            ImGui.RadioButton("Both", &alpha_flags, ImGuiColorEditFlags_AlphaPreviewHalf);
  /*
            ImGui.BeginChild("#colors", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            ImGui.PushItemWidth(-160);
            for(int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui.GetStyleColorName(i);
                if(!filter.PassFilter(name))
                    continue;
                ImGui.PushID(i);
                ImGui.ColorEdit4(name, (float*)&style->Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if(memcmp(&style->Colors[i], (ref ? &ref->Colors[i] : &default_style.Colors[i]), sizeof(ImVec4)) != 0)
            {
                ImGui.SameLine(); if(ImGui.Button("Revert")) style->Colors[i] = ref ? ref->Colors[i] : default_style.Colors[i];
                if(ref) { ImGui.SameLine(); if(ImGui.Button("Save")) ref->Colors[i] = style->Colors[i]; }
            }
            ImGui.PopID();
        }
        ImGui.PopItemWidth();
        ImGui.EndChild();

        ImGui.TreePop();
    }

    bool fonts_opened = ImGui.TreeNode("Fonts", "Fonts (%d)", ImGui.GetIO().Fonts->Fonts.Size);
    ImGui.SameLine(); ShowHelpMarker("Tip: Load fonts with io.Fonts->AddFontFromFileTTF()\nbefore calling io.Fonts->GetTex* functions.");
    if (fonts_opened)
    {
        ImFontAtlas* atlas = ImGui.GetIO().Fonts;
        if (ImGui.TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
        {
            ImGui.Image(atlas->TexID, ImVec2((float) atlas->TexWidth, (float) atlas->TexHeight), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
            ImGui.TreePop();
        }
        ImGui.PushItemWidth(100);
        for (int i = 0; i<atlas->Fonts.Size; i++)
        {
            ImFont* font = atlas->Fonts[i];
            ImGui.PushID(font);
            bool font_details_opened = ImGui.TreeNode(font, "Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
            ImGui.SameLine(); if (ImGui.SmallButton("Set as default")) ImGui.GetIO().FontDefault = font;
            if (font_details_opened)
            {
                ImGui.PushFont(font);
                ImGui.Text("The quick brown fox jumps over the lazy dog");
                ImGui.PopFont();
                ImGui.DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
                ImGui.SameLine(); ShowHelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
                ImGui.Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                ImGui.Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                ImGui.Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int) sqrtf((float) font->MetricsTotalSurface), (int) sqrtf((float) font->MetricsTotalSurface));
                for (int config_i = 0; config_i<font->ConfigDataCount; config_i++)
                {
                    ImFontConfig* cfg = &font->ConfigData[config_i];
                    ImGui.BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
                }
                if (ImGui.TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
                {
                    // Display all glyphs of the fonts in separate pages of 256 characters
                    const ImFontGlyph* glyph_fallback = font->FallbackGlyph; // Forcefully/dodgily make FindGlyph() return NULL on fallback, which isn't the default behavior.
                    font->FallbackGlyph = NULL;
                    for (int base = 0; base < 0x10000; base += 256)
                    {
                        int count = 0;
                        for (int n = 0; n< 256; n++)
                            count += font->FindGlyph((ImWchar)(base + n)) ? 1 : 0;
                        if (count > 0 && ImGui.TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base+255, count, count > 1 ? "glyphs" : "glyph"))
                        {
                            float cell_spacing = style->ItemSpacing.y;
ImVec2 cell_size(font->FontSize* 1, font->FontSize* 1);
ImVec2 base_pos = ImGui.GetCursorScreenPos();
ImDrawList* draw_list = ImGui.GetWindowDrawList();
                            for (int n = 0; n< 256; n++)
                            {
                                ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size.x + cell_spacing), base_pos.y + (n / 16) * (cell_size.y + cell_spacing));
                                ImVec2 cell_p2(cell_p1.x + cell_size.x, cell_p1.y + cell_size.y);
const ImFontGlyph* glyph = font->FindGlyph((ImWchar)(base + n));;
                                draw_list->AddRect(cell_p1, cell_p2, glyph? IM_COL32(255,255,255,100) : IM_COL32(255,255,255,50));
                                font->RenderChar(draw_list, cell_size.x, cell_p1, ImGui.GetColorU32(ImGuiCol_Text), (ImWchar)(base+n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
                                if (glyph && ImGui.IsMouseHoveringRect(cell_p1, cell_p2))
                                {
                                    ImGui.BeginTooltip();
                                    ImGui.Text("Codepoint: U+%04X", base+n);
                                    ImGui.Separator();
                                    ImGui.Text("AdvanceX: %.1f", glyph->AdvanceX);
                                    ImGui.Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                                    ImGui.Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                                    ImGui.EndTooltip();
                                }
                            }
                            ImGui.Dummy(ImVec2((cell_size.x + cell_spacing) * 16, (cell_size.y + cell_spacing) * 16));
                            ImGui.TreePop();
                        }
                    }
                    font->FallbackGlyph = glyph_fallback;
                    ImGui.TreePop();
                }
                ImGui.TreePop();
            }
            ImGui.PopID();
        }
        static float window_scale = 1.0f;
ImGui.DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
        ImGui.DragFloat("global scale", &ImGui.GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
        ImGui.PopItemWidth();
        ImGui.SetWindowFontScale(window_scale);
        ImGui.TreePop();
    }

    ImGui.PopItemWidth();*/
}
    }
}
