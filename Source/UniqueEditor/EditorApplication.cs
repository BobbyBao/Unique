using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Unique.Engine;

namespace Unique.Editor
{
    public class EditorApplication : Application
    {
        Scene scene;
        Node lightNode;
        Camera camera;
        Node floorNode;
        Node characterNode;

        protected override void Initialize()
        {
            base.Initialize();

            (scene = new Scene())
                .Component<Octree>()
                .Component<DebugRenderer>()
                .Child("Light", c => c
                    .Component<Light>()
                )
                .Child("Floor", c => c
                    .Position(Vector3.Zero)
                    .Scaling(new Vector3(30.0f, 30.0f, 30.0f))
                    .Component<StaticModel>(sm => sm
                       .Model(new ResourceRef("Models/Plane.mdl"))
                       .Material(new ResourceRefList(new List<string> { "Models/Stone.material" }))
                    )
                )
                .Child("Camera", c => c
                    .Position(new Vector3(0.0f, 20.0f, -30.0f))
                    .LookAt(new Vector3(0.0f, 0.0f, 0.0f))
                    .Component<Camera>()
                );

            Renderer.Viewport(0)
                .Rect(new IntRect(0, 0, 1280, 720))
                .Scene(scene)
                .Camera(
                    scene.GetChild("Camera").GetComponent<Camera>()
                )
                .Debug(true);

        }
        
        protected override void Shutdown()
        {
            base.Shutdown();
        }


        float val;

        protected override void OnGUI()
        {
            ImGUI.Begin("test", new nk_rect(50, 50, 230, 250),
            nk_panel_flags.NK_WINDOW_BORDER | nk_panel_flags.NK_WINDOW_MOVABLE | nk_panel_flags.NK_WINDOW_SCALABLE |
            nk_panel_flags.NK_WINDOW_MINIMIZABLE | nk_panel_flags.NK_WINDOW_TITLE);
            ImGUI.End();
        }

        protected override void UpdateFrame(float timeStep)
        {
            UpdateCamera(timeStep);
        }

        IntVector3 mouseNow_;
        IntVector3 mouseLast_;
        float mouseSpeed_ = 0.10f;
        float moveSpeed_ = 30.0f;

        float yaw_;
        float patch_;

        void UpdateCamera(float timeStep)
        {
            Vector3 offset = Vector3.Zero;
            /*
            var input = input.

            Input.GetMouse(ref mouseNow_);
     
            if(mouseLast_ == IntVector3.Zero)
            {
                mouseLast_ = mouseNow_;
            }
   
            IntVector3 mouseDelta = mouseNow_ - mouseLast_;

            if(Input.GetMouseState(MouseButton.Right))
            {
                yaw_ += mouseSpeed_ * mouseDelta.x;
                patch_ += mouseSpeed_ * mouseDelta.y;

                cameraNode.Rotation(new Quaternion(patch_, yaw_, 0.0f));
            }

            if (Input.GetMouseState(MouseButton.Middle))
            {
                offset = new Vector3(-mouseDelta.x, mouseDelta.y, 0.0f);
            }
           
            if (Input.GetKeyState(Key.KeyW))
            {
                offset.Z = 1;
            }

            if(Input.GetKeyState(Key.KeyS))
            {
                offset.Z = -1;
            }

            if(Input.GetKeyState(Key.KeyA))
            {
                offset.X = -1;
            }

            if(Input.GetKeyState(Key.KeyD))
            {
                offset.X = 1;
            }

            cameraNode.Translate(offset * moveSpeed_ * timeStep + Vector3.Forward * (float)mouseDelta.z);

            mouseLast_ = mouseNow_;*/
          
        }
    }
}
