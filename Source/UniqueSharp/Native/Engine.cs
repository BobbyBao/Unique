using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public partial class Engine
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern float Engine_GetNextTimeStep(IntPtr self);
    }
}
