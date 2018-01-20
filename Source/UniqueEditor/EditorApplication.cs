using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Unique.Engine;

namespace Unique.Editor
{
    public class EditorApplication : Application
    {
        Scene scene;
        Camera camera;
        protected override void Setup()
        {
            base.Setup();

           // deviceType = DeviceType.OpenGL;
        }

        protected override void Initialize()
        {
            base.Initialize();

            New(ref scene)
            .Component<Octree>()
            .Component<DebugRenderer>()
            .Child("Light", c => c
                .Component<Light>()
            )
            .Child("Camera", c => c
                .Position(new Vector3(0.0f, 10.0f, -30.0f))
                .LookAt(new Vector3(0.0f, 0.0f, 0.0f))
                .Component<Camera>( cam => cam
                    .Store(ref camera)
                )
            )  
            
            .Child("Floor", c => c
                .Position(Vector3.Zero)
                .Scaling(new Vector3(30.0f, 30.0f, 30.0f))
                .Component<StaticModel>(sm => sm
                    .Model(new ResourceRef(TypeOf<Model>(), "Models/Plane.mdl"))
                    .Material(new ResourceRefList(TypeOf<Material>(), "Models/Stone.material"))
                )
            )          
            .Child("Mushroom", c => c
                .Position(new Vector3(10, 0, 0))
                .Component<StaticModel>(sm => sm
                    .Model(new ResourceRef(TypeOf<Model>(), "Models/Mushroom.mdl"))
                    .Material(new ResourceRefList(TypeOf<Material>(), "Models/Mushroom.material"))
                )
            )
            .Child("Mushroom", c => c
                .Position(Vector3.One)
                .Component<StaticModel>(sm => sm
                    .Model(new ResourceRef(TypeOf<Model>(), "Models/Mushroom.mdl"))
                    .Material(new ResourceRefList(TypeOf<Material>(), "Models/Mushroom.material"))
                )
            )
            ;

            Subsystem<Renderer>()
            .Viewport(0)
            .Rect(new IntRect(0, 0, 1280, 720))               
            .Camera(camera)
            .Debug(false)
            .Scene(scene);

        }
        
        protected override void Shutdown()
        {
            base.Shutdown();
        }

        int selected = 0;
        protected override void OnGUI()
        {

            if(ImGUI.Begin("Demo", new nk_rect(50, 50, 230, 250), 
                nk_panel_flags.NK_WINDOW_BORDER | nk_panel_flags.NK_WINDOW_MOVABLE | nk_panel_flags.NK_WINDOW_SCALABLE |
                nk_panel_flags.NK_WINDOW_MINIMIZABLE | nk_panel_flags.NK_WINDOW_TITLE))
            {
                ImGUI.MenubarBegin();
                ImGUI.LayoutRowStatic(20, 40, 2);
                if(ImGUI.MenuBeginText("Sample", 4, nk_text_alignment.NK_TEXT_LEFT,  new nk_vec2(100, 100)))
                {
                    ImGUI.LayoutRowDynamic(25);
                    if (ImGUI.MenuItemText("Scene", nk_text_alignment.NK_TEXT_LEFT))
                    {

                    }
                   
                    ImGUI.MenuEnd();
                }

                if (ImGUI.MenuBeginText("Skin", 4, nk_text_alignment.NK_TEXT_LEFT, new nk_vec2(100, 160)))
                {
                    ImGUI.LayoutRowDynamic(25);
                    string[] names = typeof(Theme).GetEnumNames();
                    for(int i = 0; i < names.Length; i++)
                    {
                        if (ImGUI.MenuItemSymbolText(i == selected ? nk_symbol_type.NK_SYMBOL_X : nk_symbol_type.NK_SYMBOL_NONE, names[i], nk_text_alignment.NK_TEXT_CENTERED))
                        {
                            selected = i;
                            ImGUI.SetStyle((Theme)selected);
                        }
                    }
          

                    ImGUI.MenuEnd();
                }
                ImGUI.MenubarEnd();

                /*
                ImGUI.LayoutRowDynamic(35);
                
                int sel =  ImGUI.Combo(typeof(Theme).GetEnumNames(), selected, 20, new nk_vec2(100, 200));
                if (sel != selected)
                {
                    selected = sel;
                    ImGUI.SetStyle((Theme)selected);
                }*/
            }

            ImGUI.End();            
        }

        protected override void UpdateFrame(float timeStep)
        {
            UpdateCamera(timeStep);
        }
        
        /// Camera yaw angle.
        float yaw_;
        /// Camera pitch angle.
        float pitch_;
        void UpdateCamera(float timeStep)
        {
            var input = Subsystem<Input>();
            Vector3 offset = Vector3.Zero;

            // Movement speed as world units per second
            const float MOVE_SPEED = 20.0f;
            // Mouse sensitivity as degrees per pixel
            const float MOUSE_SENSITIVITY = 10.0f;

            if (input.GetMouseButtonDown(MouseButton.Right))
            {
                // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
                IntVector2 mouseMove = input.GetMouseMove();
                yaw_ += MOUSE_SENSITIVITY * mouseMove.x * timeStep;
                pitch_ += MOUSE_SENSITIVITY * mouseMove.y * timeStep;
                pitch_ = MathHelper.Clamp(pitch_, -90.0f, 90.0f);

                // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
                camera.node.Rotation(new Quaternion(pitch_, yaw_, 0.0f));
            }

            // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
            // Use the Translate() function (default local space) to move relative to the node's orientation.
            if (input.GetKeyDown(Keycode.W))
                camera.node.Translate(Vector3.Forward * MOVE_SPEED * timeStep);
            if (input.GetKeyDown(Keycode.S))
                camera.node.Translate(Vector3.Back * MOVE_SPEED * timeStep);
            if (input.GetKeyDown(Keycode.A))
                camera.node.Translate(Vector3.Left * MOVE_SPEED * timeStep);
            if (input.GetKeyDown(Keycode.D))
                camera.node.Translate(Vector3.Right * MOVE_SPEED * timeStep);
            
        }
    }
}
