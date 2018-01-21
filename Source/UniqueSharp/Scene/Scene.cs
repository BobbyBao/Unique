using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public class Scene : Node
    {
        public Scene() : this(CreateNative<Scene>())
        {
        }

        public Scene(IntPtr nativePtr) : base(nativePtr)
        {
        }
    }
}
