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

        protected override void Initialize()
        {
            base.Initialize();

            New(ref scene)
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
                .Component<Camera>( cam => cam
                    .Store(ref camera)
                )
            );

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
        
        /// Camera yaw angle.
        float yaw_;
        /// Camera pitch angle.
        float pitch_;
        /// Flag to indicate whether touch input has been enabled.
        bool touchEnabled_;

        void UpdateCamera(float timeStep)
        {
            var input = Subsystem<Input>();
            Vector3 offset = Vector3.Zero;

            // Movement speed as world units per second
            const float MOVE_SPEED = 20.0f;
            // Mouse sensitivity as degrees per pixel
            const float MOUSE_SENSITIVITY = 1.0f;

            if (input.GetMouseButtonDown(MouseButton.Right))
            {
                // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
                IntVector2 mouseMove = input.GetMouseMove();
                yaw_ += MOUSE_SENSITIVITY * mouseMove.x;
                pitch_ += MOUSE_SENSITIVITY * mouseMove.y;
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
