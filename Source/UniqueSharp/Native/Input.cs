using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    [SuppressUnmanagedCodeSecurity]
    public partial class Input
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern bool Input_GetKeyDown(IntPtr self, int key);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern bool Input_GetKeyPress(IntPtr self, int key);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern bool Input_GetMouseButtonDown(IntPtr self, int button);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern bool Input_GetMouseButtonPress(IntPtr self, int button);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntVector2 Input_GetMousePosition(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntVector2 Input_GetMouseMove(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Input_GetMouseMoveX(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Input_GetMouseMoveY(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Input_GetMouseMoveWheel(IntPtr self);
    }
}
