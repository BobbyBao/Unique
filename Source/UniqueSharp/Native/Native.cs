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
    delegate void EventFn(IntPtr receiver, IntPtr eventData);

    [SuppressUnmanagedCodeSecurity]
    internal unsafe static partial class Native
    {
#if DEBUG
        public const string DllName = "UniqueNativeD.dll";
#else
        public const string DllName = "UniqueNative.dll";
#endif



    }
    

}
