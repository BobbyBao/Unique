using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    public partial class StaticModel : Drawable
    {
        public StaticModel()
        {
        }

        public StaticModel Model(Model model)
        {
            StaticModel_SetModel(nativePtr, model.nativePtr);
            return this;
        }

        public StaticModel Material(Material mat)
        {
            StaticModel_SetMaterial(nativePtr, mat.nativePtr);
            return this;
        }

        public StaticModel Material(int index, Material mat)
        {
            StaticModel_SetMaterialAt(nativePtr, index, mat.nativePtr);
            return this;
        }

        public StaticModel Model(ResourceRef model)
        {
            StaticModel_SetModelAttr(nativePtr, model.native_);
            return this;
        }

        public StaticModel Material(ResourceRefList mat)
        {
            StaticModel_SetMaterialsAttr(nativePtr, mat.native_);
            return this;
        }

    }
}
