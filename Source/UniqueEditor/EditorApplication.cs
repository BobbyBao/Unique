using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Unique.Engine;

namespace Unique.Editor
{
    struct TestEvent
    {
        public string str;
    }
    public class EditorApplication : Application
    {
        Scene scene;
        Node lightNode;
        Node cameraNode;
        Node floorNode;
        Node characterNode;

        protected override void Start()
        {
            base.Start();

            (scene = new Scene())
                .Component<Octree>()
                .Component<DebugRenderer>()
                .Child("Light", c => c
                    .Component<Light>()
                )
                .Child("Floor", c => c
                    .Position(Vector3.Zero)
                    .Scaling(new Vector3(30.0f, 30.0f, 30.0f))
                    .Component<StaticModel>( sm => sm
                        .Model(new ResourceRef("models/Plane.mdl"))
                        .Material(new ResourceRefList(new List<string> { "materials/ground.mat" }))
                    )
                );

            /*
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
                .Fov(60.0f);

            Renderer.Viewport(0)
                .Rect(new IntRect(0, 0, window_.Width, window_.Height))
                .Scene(scene)
                .Camera(camera)
                .Debug(true);*/

        }
        
        protected override void Shutdown()
        {
            base.Shutdown();
        }


        float val;

        protected override void OnGUI()
        {
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
