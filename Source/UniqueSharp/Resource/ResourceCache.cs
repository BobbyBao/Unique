using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public partial class ResourceCache : Object
    {
        public T GetResource<T>(string name) where T : Resource
        {
            return GetResource(TypeOf<T>(), name) as T;
        }

        public Resource GetResource(StringID type, string name)
        {
            IntPtr obj = ResourceCache_GetResource(native_, ref type, name, false);
            return PtrToObject(obj) as Resource;
        }
    }
}
