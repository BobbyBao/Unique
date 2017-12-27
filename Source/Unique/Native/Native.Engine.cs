using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    [SuppressUnmanagedCodeSecurity]
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    delegate void EventFn(IntPtr receiver, StringID eventType, IntPtr eventData);

    [SuppressUnmanagedCodeSecurity]
    internal unsafe static partial class Native
    {
#if DEBUG
        public const string DllName = "UniqueNative_d.dll";
#else
        public const string DllName = "UniqueNative.dll";
#endif

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Engine_SetPlatformData(ref PlatformData data);
        
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Engine_Init(int w, int h);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Engine_Shutdown(IntPtr engine);
        
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Engine_RunFrame(IntPtr str);


        #region RENDERER

#if false
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void view_add_pass(IntPtr view, PassData* pass);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void view_remove_pass(IntPtr view, PassData* pass);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void view_set_render_path(IntPtr view, int renderPath);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void view_render(IntPtr view);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]

        public static extern PassData* pass_create();
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void pass_destroy(PassData* view);
#endif
        #endregion


    }

    public struct PlatformData
    {
        /// <summary>
        /// EGL native display type.
        /// </summary>
        public IntPtr DisplayType;

        /// <summary>
        /// Platform window handle.
        /// </summary>
        public IntPtr WindowHandle;

        /// <summary>
        /// Device context to use instead of letting the library create its own.
        /// </summary>
        public IntPtr Context;

        /// <summary>
        /// Backbuffer pointer to use instead of letting the library create its own.
        /// </summary>
        public IntPtr Backbuffer;

        /// <summary>
        /// Depth-stencil pointer to use instead of letting the library create its own.
        /// </summary>
        public IntPtr BackbufferDepthStencil;

        /// <summary>
        /// Occulus OVR session.
        /// </summary>
        public IntPtr OvrSession;
    }

}
