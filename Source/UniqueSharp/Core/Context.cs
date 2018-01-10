using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public static class Context
    {
        static Dictionary<StringID, Object> subsystemDict_ = new Dictionary<StringID, Object>();
        static IntPtr context_;
        public static void Init()
        {
            VectorBase subsystems = Marshal.PtrToStructure< VectorBase >(Context_GetSubsystems(context));
            for(int i = 0; i < subsystems.size; i++)
            {
                IntPtr ss = Utilities.At<IntPtr>(subsystems.buffer_, i);
                StringID type = Object.GetNativeType(ss);
                Object subsystem = Object.CreateObject(type, ss);
                if(subsystem != null)
                {
                    subsystemDict_[type] = subsystem;
                }
            }
            
        }

        public static void Register(Object obj)
        {
            StringID type = obj.GetType().Name;
            if(subsystemDict_.TryGetValue(obj.GetType().Name, out var ss))
            {
                return;
            }

            subsystemDict_[type] = obj;
            //    Context_RegisterSubsystem(obj.nativePtr);
        }

        public static void Remove(Object obj)
        {
            StringID type = obj.GetType().Name;
            if(subsystemDict_.TryGetValue(obj.GetType().Name, out var ss))
            {
                subsystemDict_.Remove(type);
                Context_RemoveSubsystem(context_, type);
            }

        }

        public static IntPtr context
        {
            get
            {
                if(context_ == IntPtr.Zero)
                {
                    context_ = GetContext();
                }

                return context_;
            }
        }

        public static IntPtr CreateObject(ref StringID type )
        {
            return Context_CreateObject(context, ref type);            
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr GetContext();

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Context_RegisterSubsystem(IntPtr context, IntPtr obj);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Context_RemoveSubsystem(IntPtr context, StringID objectType);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Context_GetSubsystems(IntPtr context);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Context_CreateObject(IntPtr context, ref StringID type);
            

    }
}
