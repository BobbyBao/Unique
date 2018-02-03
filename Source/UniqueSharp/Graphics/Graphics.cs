using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public partial class Graphics : Subsystem<Graphics>
    {
        public int width => Graphics_GetWidth(native_);

        public int height => Graphics_GetHeight(native_);

        public static float renderWait => Graphics_GetRenderWait();
        public static float updateWait => Graphics_GetUpdateWait();

    }
}
