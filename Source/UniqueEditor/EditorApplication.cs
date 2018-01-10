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
        {/*
            var input = GetSubsystem<Input>();
            Vector3 offset = Vector3.Zero;

            // Movement speed as world units per second
            const float MOVE_SPEED = 20.0f;
            // Mouse sensitivity as degrees per pixel
            const float MOUSE_SENSITIVITY = 0.1f;

            if (input.GetMouseButtonDown(MOUSEB_RIGHT))
            {
                // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
                IntVector2 mouseMove = input.GetMouseMove();
                yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
                pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
                pitch_ = Clamp(pitch_, -90.0f, 90.0f);

                // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
                camera_->GetNode()->SetRotation(Quaternion(pitch_ * M_DEGTORAD, yaw_ * M_DEGTORAD, 0.0f));
            }

            // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
            // Use the Translate() function (default local space) to move relative to the node's orientation.
            if (input.GetKeyDown(KEY_W))
                camera_->GetNode()->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
            if (input.GetKeyDown(KEY_S))
                camera_->GetNode()->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
            if (input.GetKeyDown(KEY_A))
                camera_->GetNode()->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
            if (input.GetKeyDown(KEY_D))
                camera_->GetNode()->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

            */
        }
    }
}
