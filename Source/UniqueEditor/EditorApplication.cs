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
        Node cameraNode;
        Node floorNode;
        Node characterNode;

        protected override void Init()
        {
            base.Init();

            scene = new Scene();
            scene.Component<Octree>();
            scene.Component<DebugRenderer>();

            (lightNode = scene.Child("Light")).Component<Light>();

            floorNode = scene.Child("Floor")
                .Position(Vector3.Zero)
                .Scaling(new Vector3(30.0f, 30.0f, 30.0f));

            StaticModel floor = floorNode.Component<StaticModel>();
            floor.Model(new ResourceRef("models/Plane.mdl"));
            floor.Material(
                new ResourceRefList(new List<string>{ "materials/ground.mat" }));

            Vector3 eye = new Vector3(0.0f, 20.0f, -30.0f);
            Vector3 at = new Vector3(0.0f, 0.0f, 0.0f);

            cameraNode = scene.Child("Camera")
                .Position(eye)
                .LookAt(at);

           var camera = cameraNode.Component<Camera>()            
                .Fov(60.0f)
                .AspectRatio((float)window_.Width / window_.Height);
            
            Renderer.Viewport(0)
                .Rect(new IntRect(0, 0, window_.Width, window_.Height))
                .Scene(scene)
                .Camera(camera)
                .Debug(true);

        }

        protected override void Setup()
        {
            base.Setup();
        }

        protected override void Shutdown()
        {
            base.Shutdown();
        }


        float val;

        protected override void OnGUI()
        {
            unsafe
            {
                fixed (float* p = &val)
                    ImGui.DragFloat("test", p);

                ImGUI.ShowStyleEditor((ImGuiStyle*)ImGui.GetStyle());
            }
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

            mouseLast_ = mouseNow_;
          
        }
    }
}
