﻿using System;
using System.Collections.Generic;
using UniqueEditor.Samples;
using UniqueEngine;

namespace UniqueEditor
{
    public class EditorApplication : Application
    {
        Sample current;

        protected override void Setup()
        {
            base.Setup();

        //    deviceType = DeviceType.OpenGL;
        }

        protected override void Initialize()
        {
            base.Initialize();

            Engine.instance.maxFps = 1000;


            if(Sample.all.Count > 0)
            {
                SetSample(Activator.CreateInstance(Sample.all[0].Item4)as Sample);
            }
        }
        
        protected override void Shutdown()
        {
            if(current)
            {
                current.Exit();
            }

            base.Shutdown();
        }

        int selected = 0;
        protected override void OnGUI()
        {
            var graphics = GetSubsystem<Graphics>();

            if (ImGUI.Begin("Editor", new nk_rect(0, 0, graphics.width, 35), 0))
            {
                ImGUI.MenubarBegin();
                ImGUI.LayoutRowStatic(20, 60, 2);
                if(ImGUI.MenuBegin("Demo", nk_text_alignment.NK_TEXT_LEFT,  new nk_vec2(160, 200)))
                {
                    ImGUI.LayoutRowDynamic(25);
                    
                    foreach(var s in Sample.all)
                    {
                        (string name, string d, int sort, Type t) = s;
                        var currentType = current?.GetType();
                        if (ImGUI.MenuItem( t == currentType ? nk_symbol_type.NK_SYMBOL_CIRCLE_SOLID : nk_symbol_type.NK_SYMBOL_NONE, name, nk_text_alignment.NK_TEXT_RIGHT))
                        {
                            SetSample(System.Activator.CreateInstance(t) as Sample);
                        }
                        
                    }           
                   
                    ImGUI.MenuEnd();
                }

                if (ImGUI.MenuBegin("Skin", nk_text_alignment.NK_TEXT_LEFT, new nk_vec2(120, 160)))
                {
                    ImGUI.LayoutRowDynamic(25);

                    string[] names = typeof(Theme).GetEnumNames();
                    for(int i = 0; i < names.Length; i++)
                    {
                        if (ImGUI.MenuItem(i == selected ? nk_symbol_type.NK_SYMBOL_X : nk_symbol_type.NK_SYMBOL_NONE, names[i], nk_text_alignment.NK_TEXT_CENTERED))
                        {
                            selected = i;
                            ImGUI.SetStyle((Theme)selected);
                        }
                    }
          
                    ImGUI.MenuEnd();
                }

                ImGUI.MenubarEnd();

            }

            ImGUI.End();


            if (ImGUI.Begin("Debug", new nk_rect(graphics.width - 200, 40, 200, 120), nkPanelFlags.MINIMIZABLE | nkPanelFlags.TITLE))
            {
                ImGUI.LayoutRowDynamic(20, 2);
                ImGUI.Label("FPS:");
                ImGUI.Text(Engine.instance.fps.ToString(), nk_text_alignment.NK_TEXT_LEFT);
                ImGUI.Label("RenderWait:");
                ImGUI.Text(Graphics.renderWait.ToString(), nk_text_alignment.NK_TEXT_LEFT);
                ImGUI.Label("UpdateWait:");
                ImGUI.Text(Graphics.updateWait.ToString(), nk_text_alignment.NK_TEXT_LEFT);

            }

            ImGUI.End();

            if (current)
            {
                current.OnGUI();
            }


        }


        void SetSample(Sample sample)
        {
            if(current != sample)
            {
                if(current)
                {
                    current.Exit();
                }
            
                current = sample;

                if (current)
                {
                    current.Enter();
                }
            }

        }

        protected override void UpdateFrame(float timeStep)
        {
            if(current)
            {
                current.Update(timeStep);
            }
        }
        
    }
}
