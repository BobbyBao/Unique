using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public class ResourceRef : UnmanagedObject<ResourceRef.Data>
    {
        string name_;
        public ResourceRef(string name)
        {
            name_ = name;
            data.name.Set(name);
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Data : IDisposable
        {
            public StringID type;
            public String.Data name;

            public void Dispose()
            {
                name.Dispose();
            }
        }

    }

    public class ResourceRefList : UnmanagedObject<ResourceRefList.Data>
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Data : IDisposable
        {
            public StringID type;
            public Vector<String.Data> names;

            public void Dispose()
            {
                names.Dispose();
            }
        }
        public ResourceRefList(List<string> n)
        {
            names = n;
        }

        List<string> names_;
        public List<string> names
        {
            get => names_;
            set
            {
                names_ = value;

                //此处有内存泄露
                foreach(string name in names_)
                {
                    data.names.Add(new String.Data(name));
                }
            }
        }
        
    }
}
