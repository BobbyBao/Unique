using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace UniqueEngine
{
    [SuppressUnmanagedCodeSecurity]
    internal unsafe static partial class Native
    {
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Auto)]
        public static extern IntPtr StringID_new([MarshalAs(UnmanagedType.LPStr)]string name);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Context_CreateObject(IntPtr conext, ref StringID type);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RefCounted_AddRef(IntPtr intPtr);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RefCounted_ReleaseRef(IntPtr intPtr);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int RefCounted_Refs(IntPtr intPtr);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int RefCounted_WeakRefs(IntPtr intPtr);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RefCounted_Delete(IntPtr refCount);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Object_GetType(IntPtr obj);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_SendEvent(IntPtr self, ref StringID eventType, IntPtr eventData);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_Subscribe(IntPtr self, ref StringID eventType, EventFn handler);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_SubscribeTo(IntPtr self, IntPtr sender, ref StringID eventType, EventFn handler);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_Unsubscribe(IntPtr self, ref StringID eventType);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Object_UnsubscribeFrom(IntPtr self, IntPtr sender, ref StringID eventType);

        
    }
}
