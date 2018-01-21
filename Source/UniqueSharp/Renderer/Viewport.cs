using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    public class Viewport : Object<Viewport>
    {
        public Viewport()
        {
        }

        public Viewport Rect(IntRect rect)
        {
            Viewport_SetRect(native_, ref rect);
            return this;
        }

        public Viewport Scene(Scene scene)
        {
            Viewport_SetScene(native_, scene.nativePtr);
            return this;
        }

        public Viewport Camera(Camera camera)
        {
            Viewport_SetCamera(native_, camera.nativePtr);
            return this;
        }

        public Viewport Debug(bool debug)
        {
            Viewport_SetDebug(native_, debug);
            return this;
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Viewport_SetRect(IntPtr view, ref IntRect rect);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Viewport_SetScene(IntPtr view, IntPtr scene);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Viewport_SetCamera(IntPtr view, IntPtr camera);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Viewport_SetDebug(IntPtr view, Bool debug);
    }
}
