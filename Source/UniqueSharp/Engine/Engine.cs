using System;
using System.Collections.Generic;
using System.Text;

namespace Unique.Engine
{
    public partial class Engine : Subsystem<Engine>
    {
        public float timeStep => Engine_GetNextTimeStep(native_);
    }
}
