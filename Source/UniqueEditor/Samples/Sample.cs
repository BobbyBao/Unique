using System;
using System.Collections.Generic;
using System.Text;
using UniqueEngine;

namespace UniqueEditor.Samples
{
    public class SampleDescAttribute : System.Attribute
    {
        public string name;
        public string desc;
        public int sortOrder;
    }

    public abstract class Sample : UniqueEngine.Object
    {
        protected Scene scene;
        protected Camera camera;
        protected Viewport viewport;
        /// Camera yaw angle.
        protected float yaw_;
        /// Camera pitch angle.
        protected float pitch_;

        public static List<(string, string, int, Type)> all = new List<(string, string, int, Type)>();

        static Sample()
        {
            var types = System.Reflection.Assembly.GetExecutingAssembly().GetTypes();
            foreach (var t in types)
            {
                if (t.IsSubclassOf(typeof(Sample)))
                {
                    var attr = t.GetCustomAttributes(typeof(SampleDescAttribute), true);
                    if (attr.IsNullOrEmpty())
                    {
                        all.Add((t.Name, "", 0, t));
                    }
                    else
                    {
                        var a = (SampleDescAttribute)attr[0];
                        all.Add((string.IsNullOrEmpty(a.name) ? t.Name : a.name, a.desc, a.sortOrder, t));
                    }
                }
            }

            all.Sort((v1, v2) => v1.Item3 - v2.Item3);
        }

        public virtual void Enter()
        {
            var renderer = GetSubsystem<Renderer>();
            viewport = renderer.CreateViewport(0)                
            .Debug(false);
        }

        public virtual void Update(float timeStep)
        {
            UpdateCamera(timeStep);
        }

        public virtual void OnGUI()
        {
            var graphics = GetSubsystem<Graphics>();

            if (ImGUI.Begin(GetType().Name, new nk_rect(graphics.width - 200, 40, 200, 120), nkPanelFlags.MINIMIZABLE | nkPanelFlags.TITLE))
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
        }

        public virtual void Exit()
        {
            scene?.Dispose();
        }

        protected void UpdateCamera(float timeStep)
        {
            if(camera == null)
            {
                return;
            }

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
