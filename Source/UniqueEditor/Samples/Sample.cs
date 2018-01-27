using System;
using System.Collections.Generic;
using System.Text;
using UniqueEngine;

namespace UniqueEditor.Samples
{
    public abstract class Sample : UniqueEngine.Object
    {
        protected Scene scene;
        protected Camera camera;

        /// Camera yaw angle.
        protected float yaw_;
        /// Camera pitch angle.
        protected float pitch_;

        public virtual void Enter()
        {
        }

        public virtual void Update(float timeStep)
        {
            UpdateCamera(timeStep);
        }

        public virtual void Exit()
        {

        }

        protected void UpdateCamera(float timeStep)
        {
            var input = GetSubsystem<Input>();
            Vector3 offset = Vector3.Zero;

            // Movement speed as world units per second
            const float MOVE_SPEED = 20.0f;
            // Mouse sensitivity as degrees per pixel
            const float MOUSE_SENSITIVITY = 10.0f;

            if (input.GetMouseButtonDown(MouseButton.Right))
            {
                //System.Console.WriteLine("mouse down");
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
