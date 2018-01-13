using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public partial class ResourceRef : DisposeBase
    {
        IntPtr native_;
        public string name { get; set; }

        public ResourceRef(string name)
        {
            this.name = name;
        }
        
    }

    public partial class ResourceRefList : DisposeBase
    {
        IntPtr native_;
        public ResourceRefList(List<string> n)
        {
            names = n;
        }
        
        public List<string> names { get; set; }
        
        
    }
}
