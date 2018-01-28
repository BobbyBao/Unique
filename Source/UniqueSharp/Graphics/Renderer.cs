using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    public class Renderer : Object
    {
        static Viewport[] viewports_ = new Viewport[0];
        public Viewport CreateViewport(int index)
        {
            if(index >= viewports_.Length)
            {
                Array.Resize(ref viewports_, index + 1);
            }

            Viewport viewport = viewports_[index];
            if(!viewport)
            {
                viewport = new Viewport();
                viewports_[index] = viewport;
                Renderer_SetViewport(native_, index, viewport.nativePtr);
            }

            return viewport;
        }

        public void SetViewport(int index, Viewport viewport)
        {
            if(index >= viewports_.Length)
            {
                Array.Resize(ref viewports_, index + 1);
            }

            for(int i = 0; i < viewports_.Length; i++)
            {
                if(viewports_[i] == viewport)
                {
                    viewports_[i] = null;
                }
            }

            viewports_[index] = viewport;
            Renderer_SetViewport(native_, index, viewport.nativePtr);
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Renderer_GetViewport(IntPtr self, int index);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Renderer_SetViewport(IntPtr self, int index, IntPtr viewport);


    }
}
