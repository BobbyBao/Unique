using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public partial class Engine : Subsystem<Engine>
    {
        public int maxFps
        {
            get => Engine_GetMaxFps(native_);
            set => Engine_SetMaxFps(native_, value);
        }

        public float fps => 1 / timeStep;
        public float timeStep => Engine_GetNextTimeStep(native_);
    }
}
