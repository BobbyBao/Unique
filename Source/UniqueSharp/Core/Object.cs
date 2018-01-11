using System;
using System.Collections.Generic;
using System.Text;

namespace Unique.Engine
{
    using System.Runtime.CompilerServices;
    using static Native;

    public class Object : RefCounted
    {
        public Object()
        {
        }

        public Object(IntPtr ptr) : base(ptr)
        {
        }

        public void Subscribe<T>(Action<T> action)
        {
            Object_Subscribe(native_, ref TypeOf<T>(), (receiver, eventType, eventData) =>
            {
                action(Utilities.As<T>(eventData));
            });
        }

        public void SubscribeTo<T>(Object sender, Action<T> action)
        {
            Object_SubscribeTo(native_, sender.native_, ref TypeOf<T>(), (r, et, ed) =>
            {
                action(Utilities.As<T>(ed));
            });
        }

        public void Subscribe<T>(ref StringID eventType, Action<T> action)
        {
            Object_Subscribe(native_, ref eventType, (r, et, ed) =>
            {
                action(Utilities.As<T>(ed));
            });
        }

        public void SubscribeTo<T>(Object sender, ref StringID eventType, Action<T> action)
        {
            Object_SubscribeTo(native_, sender.native_, ref eventType, (r, et, ed) =>
            {
                action(Utilities.As<T>(ed));
            });
        }
        
        public void SendEvent<T>(ref T eventData) where T : struct
        {
            Object_SendEvent(native_, ref TypeOf<T>(), Utilities.As(ref eventData));
        }
   
        public void SendEvent<T>(ref StringID eventType, ref T eventData) where T : struct
        {
            Object_SendEvent(native_, ref eventType, Utilities.As(ref eventData));
        }

        public T New<T>(ref T val) where T : Object, new()
        {
            val = new T();
            return val;
        }

        public T Store<T>(ref T val) where T : Object
        {
            val = (T)this;
            return val;
        }

        class TypeID<T>
        {
            public static StringID typeID = typeof(T).Name;
        }

        public static ref StringID TypeOf<T>()
        {
            return ref TypeID<T>.typeID;
        }

        public static Object CreateObject(StringID typeID, IntPtr nativePtr)
        {
            Type type = Type.GetType("Unique.Engine." + typeID);
            if(type == null)
            {
                return null;
            }
            Object obj = Activator.CreateInstance(type) as Object;
            if(!obj)
            {
                return null;
            }

            obj.Attach(nativePtr);
            return obj;
        }

        public static StringID GetNativeType(IntPtr nativePtr)
        {
            return new StringID(Object_GetType(nativePtr));
        }

        public static IntPtr CreateNative<T>()
        {
            return Context.CreateObject(ref TypeOf<T>());
        }

        public static RefCounted PtrToObject(IntPtr nativePtr)
        {
            if(ptrToObject_.TryGetValue(nativePtr, out var weakRef))
            {
                if(weakRef.TryGetTarget(out var obj))
                    return obj;
            }

            StringID type = GetNativeType(nativePtr);

            return CreateObject(type, nativePtr);
        }

        public static T LookUpObject<T>(IntPtr nativePtr) where T : RefCounted, new()
        {
            if(ptrToObject_.TryGetValue(nativePtr, out var weakRef))
            {
                if(weakRef.TryGetTarget(out var obj))
                    return obj as T;
            }

            return null;
        }

        public static T PtrToObject<T>(IntPtr nativePtr) where T : RefCounted, new()
        {
            if(ptrToObject_.TryGetValue(nativePtr, out var weakRef))
            {
                if(weakRef.TryGetTarget(out var obj))
                    return obj as T;
            }

            StringID type = GetNativeType(nativePtr);
            T ret = new T();
            ret.Attach(nativePtr);
            return ret;
        }

        public static T GetSubsystem<T>() where T : Object
        {
            return Context.Get(TypeOf<T>()) as T;
        }

    }

    public class Object<T> : Object
    {
        public Object() : base(CreateNative<T>())
        {
        }

        protected override void Destroy()
        {
            base.Destroy();

            if(native_ != IntPtr.Zero)
                Object_ReleaseRef(native_);
        }
    }

}
