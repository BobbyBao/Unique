using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace UniqueEngine
{
    /// Device type
    public enum DeviceType : Int32
    {
        Undefined = 0,  ///< Undefined device
        D3D11,      ///< D3D11 device
        D3D12,      ///< D3D12 device
        OpenGL,     ///< OpenGL device 
        OpenGLES    ///< OpenGLES device
    }

    [SuppressUnmanagedCodeSecurity]
    public partial class Graphics
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        static extern int Graphics_GetWidth(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        static extern int Graphics_GetHeight(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        static extern float Graphics_GetRenderWait();

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        static extern float Graphics_GetUpdateWait();
    }

    [SuppressUnmanagedCodeSecurity]
    public partial class Camera
    {
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

    [SuppressUnmanagedCodeSecurity]
    public partial class Light
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetLightType(IntPtr self, LightType type);
        
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetColor(IntPtr self, ref Color color);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetTemperature(IntPtr self, float temperature);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetRadius(IntPtr self, float radius);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetLength(IntPtr self, float length);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetUsePhysicalValues(IntPtr self, float enable);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetSpecularIntensity(IntPtr self, float intensity);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetBrightness(IntPtr self, float brightness);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetRange(IntPtr self, float range);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetFov(IntPtr self, float fov);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Light_SetAspectRatio(IntPtr self, float aspectRatio);
    }

    [SuppressUnmanagedCodeSecurity]
    public partial class StaticModel
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetModel(IntPtr self, IntPtr model);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetMaterial(IntPtr self, IntPtr material);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetMaterialAt(IntPtr self, int index, IntPtr material);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetModelAttr(IntPtr self, IntPtr model);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetMaterialsAttr(IntPtr self, IntPtr material);

    }

    [SuppressUnmanagedCodeSecurity]
    public partial class AnimatedModel
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void AnimatedModel_SetModelAttr(IntPtr self, IntPtr model);
    }

    [SuppressUnmanagedCodeSecurity]
    public partial class Material
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        static extern void Material_SetShader(IntPtr self, IntPtr shader);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        static extern IntPtr Material_GetShader(IntPtr self);


        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        static extern IntPtr Material_GetShaderAttr(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        static extern void Material_SetShaderAttr(IntPtr self, IntPtr shader);

    }

}
