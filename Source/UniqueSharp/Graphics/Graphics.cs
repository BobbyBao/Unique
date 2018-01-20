using System;
using System.Collections.Generic;
using System.Text;

namespace Unique.Engine
{
    public partial class Graphics : Object
    {
        public int width => Graphics_GetWidth(native_);

        public int height => Graphics_GetHeight(native_);

    }
}
