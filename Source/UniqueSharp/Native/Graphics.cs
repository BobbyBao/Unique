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
