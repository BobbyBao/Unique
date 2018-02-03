using System;
using System.Collections.Generic;
using System.Text;
using UniqueEngine;

namespace UniqueEditor.Samples
{
    [SampleDesc(sortOrder = 2)]
    public class GUISample : Sample
    {
        static int EASY = 0;
        static int HARD = 1;
        static int op = EASY;
        static int property = 20;

        Color background = new Color(28, 48, 62);

        public override void OnGUI()
        {
            base.OnGUI();

            if (ImGUI.Begin("GUI Demo", new nk_rect(50, 50, 230, 250),
            nkPanelFlags.BORDER | nkPanelFlags.MOVABLE | nkPanelFlags.SCALABLE |
            nkPanelFlags.MINIMIZABLE | nkPanelFlags.TITLE))
            {
                ImGUI.LayoutRowStatic(30, 80, 1);

                if (ImGUI.Button("button"))
                {
                    System.Console.WriteLine("button pressed\n");
                }

                ImGUI.LayoutRowDynamic(30, 2);
                if (ImGUI.Option("easy", op == EASY)) op = EASY;
                if (ImGUI.Option("hard", op == HARD)) op = HARD;

                ImGUI.LayoutRowDynamic(22, 1);

                ImGUI.PropertyInt("Compression:", 0, ref property, 100, 10, 1);

                ImGUI.LayoutRowDynamic(20, 1);

                ImGUI.Label("background:", nk_text_alignment.NK_TEXT_LEFT);

                ImGUI.LayoutRowDynamic(25, 1);

                if (ImGUI.ComboBegin(background, new nk_vec2(ImGUI.WidgetWidth(), 400)))
                {
                    ImGUI.LayoutRowDynamic(120, 1);                    
                    ImGUI.ColorPick(ref background, nk_color_format.NK_RGBA);

                    ImGUI.LayoutRowDynamic(25, 1);
                    background.r = ImGUI.Propertyf("#R:", 0, background.r, 1, 0.01f, 0.005f);
                    background.g = ImGUI.Propertyf("#G:", 0, background.g, 1, 0.01f, 0.005f);
                    background.b = ImGUI.Propertyf("#B:", 0, background.b, 1, 0.01f, 0.005f);
                    background.a = ImGUI.Propertyf("#A:", 0, background.a, 1, 0.01f, 0.005f);

                    ImGUI.ComboEnd();
                }
            }

            ImGUI.End();


        }
    }
}
