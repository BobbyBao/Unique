using System;
using System.Collections.Generic;
using System.Text;
using UniqueEngine;

namespace UniqueEditor.Samples
{
    public class GUISample : Sample
    {
        static int EASY = 0;
        static int HARD = 1;
        static int op = EASY;
        static int property = 20;
        nk_color background = new nk_color(28, 48, 62);

        public override void OnGUI()
        {
            if (ImGUI.Begin("GUISample", new nk_rect(50, 50, 230, 250),
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

                    background = ImGUI.ColorPicker(background, nk_color_format.NK_RGBA);

                    ImGUI.LayoutRowDynamic(25, 1);

                    background.r = (byte)ImGUI.Propertyi("#R:", 0, background.r, 255, 1, 1);
                    background.g = (byte)ImGUI.Propertyi("#G:", 0, background.g, 255, 1, 1);
                    background.b = (byte)ImGUI.Propertyi("#B:", 0, background.b, 255, 1, 1);
                    background.a = (byte)ImGUI.Propertyi("#A:", 0, background.a, 255, 1, 1);

                    ImGUI.ComboEnd();
                }
            }
            ImGUI.End();


        }
    }
}
