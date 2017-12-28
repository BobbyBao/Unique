using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public static class Context
    {
        static Dictionary<StringID, Object> subsystemDict_ = new Dictionary<StringID, Object>();

        public static void Init()
        {
            VectorBase subsystems = Context_GetSubsystems();
            for(int i = 0; i < subsystems.size; i++)
            {
                IntPtr ss = Utilities.At<IntPtr>(subsystems.buffer, i);
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
                Context_RemoveSubsystem(type);
            }

        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Context_RegisterSubsystem(IntPtr obj);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Context_RemoveSubsystem(StringID objectType);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern ref VectorBase Context_GetSubsystems();
    }
}
