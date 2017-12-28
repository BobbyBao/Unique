using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public partial class Application
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Unique_Setup(int argc, [MarshalAs(UnmanagedType.LPArray)]string[] argv);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Unique_Start(DeviceType deviceType, IntPtr window);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Unique_Shutdown();
    }
}
