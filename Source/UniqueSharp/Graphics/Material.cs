using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public partial class Material : Resource
    {
        public Material ShaderAttr(ResourceRef shaderAttr)
        {
            Material_SetShaderAttr(native_, shaderAttr.native_);
            return this;
        }
    }
}
