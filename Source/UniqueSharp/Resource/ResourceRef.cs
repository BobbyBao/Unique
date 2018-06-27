using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{   
    public partial class ResourceRef : DisposeBase
    {
        public IntPtr native_;
        public StringID type_;
        public string name_;

        public ResourceRef(StringID type, string name)
        {
            type_ = type;
            name_ = name;
            native_ = ResourceRef_new(type, name);
        }
        
    }

    public class ResourceRef<T> : ResourceRef
    {
        public ResourceRef(string name) : base(Object.TypeOf<T>(), name)
        {
        }
    }

    public partial class ResourceRefList : DisposeBase
    {
        public IntPtr native_;
        public StringID type_;
        public string names_;
        public ResourceRefList(StringID type, string names)
        {
            type_ = type;
            names_ = names;
            native_ = ResourceRefList_new(type, names);
        }        
        
    }

    public class ResourceRefList<T> : ResourceRefList
    {
        public ResourceRefList(string name) : base(Object.TypeOf<T>(), name)
        {
        }
    }
}
