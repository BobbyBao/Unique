using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    public partial class Camera : Component
    {
        public Camera()
        {
        }

        public Camera NearClip(float nearClip)
        {
            Camera_SetNearClip(native_, nearClip);
            return this;
        }

        public Camera FarClip(float nearClip)
        {
            Camera_SetFarClip(native_, nearClip);
            return this;
        }

        public Camera Fov(float fov)
        {
            Camera_SetFov(native_, fov);
            return this;
        }

        public Camera OrthoSize(Vector2 orthoSize)
        {
            Camera_SetOrthoSize(native_, ref orthoSize);
            return this;
        }

        public Camera AspectRatio(float aspectRatio)
        {
            Camera_SetAspectRatio(native_, aspectRatio);
            return this;
        }

    }
}
