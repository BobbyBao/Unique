using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    [SuppressUnmanagedCodeSecurity]
    internal unsafe static partial class Native
    {
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        public static extern IntPtr StringID_Create([MarshalAs(UnmanagedType.LPStr)]string name);


        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Object_Create(ref StringID type);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_AddRef(IntPtr intPtr);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_ReleaseRef(IntPtr intPtr);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Object_Refs(IntPtr intPtr);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Object_WeakRefs(IntPtr intPtr);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_Delete(IntPtr refCount);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Object_GetType(IntPtr obj);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_SendEvent(IntPtr self, ref StringID eventType, IntPtr eventData);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_SubscribeToGlobalEvent(IntPtr self, ref StringID eventType, EventFn handler);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_SubscribeToEvent(IntPtr self, IntPtr sender, ref StringID eventType, EventFn handler);
        
    }
}
