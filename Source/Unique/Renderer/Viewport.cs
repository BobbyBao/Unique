using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public class Viewport : Object<Viewport>
    {
        public Viewport()
        {
        }

        public Viewport Rect(IntRect rect)
        {
            viewportSetRect(native_, ref rect);
            return this;
        }

        public Viewport Scene(Scene scene)
        {
            viewportSetScene(native_, scene.nativePtr);
            return this;
        }

        public Viewport Camera(Camera camera)
        {
            viewportSetCamera(native_, camera.nativePtr);
            return this;
        }

        public Viewport Debug(bool debug)
        {
            viewportSetDebug(native_, debug);
            return this;
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void viewportSetRect(IntPtr view, ref IntRect rect);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void viewportSetScene(IntPtr view, IntPtr scene);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void viewportSetCamera(IntPtr view, IntPtr camera);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void viewportSetDebug(IntPtr view, Bool debug);
    }
}
