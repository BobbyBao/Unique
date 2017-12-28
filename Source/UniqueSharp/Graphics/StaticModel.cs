using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public class StaticModel : Drawable
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
            StaticModel_SetModelAttr(nativePtr, ref model.data);
            return this;
        }

        public StaticModel Material(ResourceRefList mat)
        {
            StaticModel_SetMaterialAttr(nativePtr, ref mat.data);
            return this;
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetModel(IntPtr self, IntPtr model);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetMaterial(IntPtr self, IntPtr material);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetMaterialAt(IntPtr self, int index, IntPtr material);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetModelAttr(IntPtr self, ref ResourceRef.Data model);
          
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void StaticModel_SetMaterialAttr(IntPtr self, ref ResourceRefList.Data material);

    }
}
