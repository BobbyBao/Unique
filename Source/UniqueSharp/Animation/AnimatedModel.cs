using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public partial class AnimatedModel : StaticModel
    {
        public new AnimatedModel Model(ResourceRef model)
        {
            AnimatedModel_SetModelAttr(nativePtr, model.native_);
            return this;
        }
    }
}
