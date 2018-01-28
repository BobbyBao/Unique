using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    public class Camera : Component
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

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetNearClip(IntPtr self, float p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetFarClip(IntPtr self, float p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetFov(IntPtr self, float p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetOrthoSize(IntPtr self, ref Vector2 p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetAspectRatio(IntPtr self, float p1);
        
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetZoom(IntPtr self, float p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Camera_SetLodBias(IntPtr self, float p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetViewMask(IntPtr self, uint p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetViewOverrideFlags(IntPtr self, uint p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetOrthographic(IntPtr self, bool p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetAutoAspectRatio(IntPtr self, bool p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetProjectionOffset(IntPtr self, ref Vector2 p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetUseReflection(IntPtr self, bool p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetReflectionPlane(IntPtr self, ref Plane p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetUseClipping(IntPtr self, bool p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetClipPlane(IntPtr self, ref Plane p1);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Camera_SetFlipVertical(IntPtr self, bool p1);
        
    }
}
