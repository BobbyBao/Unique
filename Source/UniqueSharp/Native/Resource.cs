using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public partial class ResourceRef
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        public static extern IntPtr ResourceRef_new2(StringID type, [MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        public static extern IntPtr ResourceRef_delete(IntPtr self);
    }

    public partial class ResourceRefList
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        public static extern IntPtr ResourceRefList_new3(StringID type, [MarshalAs(UnmanagedType.LPArray)]string[] name, int count);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        public static extern IntPtr ResourceRefList_delete(IntPtr self);
    }
}
