using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public partial class Component : Object
    {
        public Component()
        {
        }

        public Node node
        {
            get
            {
                return PtrToObject<Node>(Component_GetNode(native_));
            }
        }
    }
}
