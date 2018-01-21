using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    public partial class Application : Object
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Unique_Setup(int argc, [MarshalAs(UnmanagedType.LPArray)]string[] argv);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Unique_Start(IntPtr native, DeviceType deviceType, IntPtr window);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Unique_Shutdown();

    }
}
