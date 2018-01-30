using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace UniqueEngine
{
    public partial class Component
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern uint Component_GetID(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Component_GetNode(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Component_GetScene(IntPtr self);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        static extern bool Component_IsEnabled(IntPtr self);
    }

    [SuppressUnmanagedCodeSecurity]
    public partial class Node
    {
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_SetPosition(IntPtr self, ref Vector3 pos);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_SetRotation(IntPtr self, ref Quaternion rot);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_SetDirection(IntPtr self, ref Vector3 dir);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_SetScale(IntPtr self, ref Vector3 s);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_Translate(IntPtr self, ref Vector3 t, TransformSpace ts);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_Rotate(IntPtr self, ref Quaternion r, TransformSpace ts);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_Pitch(IntPtr self, float pitch, TransformSpace ts);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_Yaw(IntPtr self, float yaw, TransformSpace ts);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_Roll(IntPtr self, float roll, TransformSpace ts);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_LookAt(IntPtr self, ref Vector3 target, ref Vector3 up, TransformSpace ts);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Node_Scale(IntPtr self, ref Vector3 s);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Node_CreateChild(IntPtr self, ref StringID name, bool temp);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Node_GetChild(IntPtr self, ref StringID name, bool recursive);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Node_GetChildNum(IntPtr self, bool recursive);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Node_GetChildAt(IntPtr self, int index);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Node_CreateComponent(IntPtr self, ref StringID type);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Node_GetOrCreateComponent(IntPtr self, ref StringID type);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Node_GetComponent(IntPtr self, ref StringID type, bool recursive);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Node_GetParentComponent(IntPtr self, ref StringID type, bool fullTraversal);

        
    }
}
