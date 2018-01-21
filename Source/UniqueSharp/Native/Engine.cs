using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    public partial class Engine
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Engine_SetMinFps(IntPtr self, int fps);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Engine_SetMaxFps(IntPtr self, int fps);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float Engine_GetNextTimeStep(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Engine_GetMinFps(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Engine_GetMaxFps(IntPtr self);
    }
}
