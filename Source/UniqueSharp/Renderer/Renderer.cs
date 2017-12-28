using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public class Renderer : Object
    {
        static Viewport[] viewports_ = new Viewport[0];
        public static Viewport Viewport(int index)
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
                Renderer_SetViewport(index, viewport.nativePtr);
            }

            return viewport;
        }

        public static void SetViewport(int index, Viewport viewport)
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
            Renderer_SetViewport(index, viewport.nativePtr);
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Render_GetViewport(int index);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Renderer_SetViewport(int index, IntPtr viewport);


    }
}
