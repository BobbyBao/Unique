using System;
using System.Runtime.InteropServices;


namespace Unique.Engine
{
    using System.Threading;
    using static Native;

    public class Engine : Object
    {
        private bool shouldStop_ = false;
        private Thread thread_;
        int width_;
        int height_;

        public event Action eventInit;
        public event Action eventShutdown;

        public Engine()
        {
        }

        public void Initialize(IntPtr handle, int w, int h)
        {
            width_ = w;
            height_ = h;

            var platformData = new PlatformData() { WindowHandle = handle };

            Engine_SetPlatformData(ref platformData);           
        }

        public void Start()
        {
            thread_ = new Thread(() =>
            {
                CoreThreadLoop();
            });

            thread_.Start();
        }

        public void Stop()
        {
            shouldStop_ = true;
            // wait for the render thread to finish
            thread_.Join();
        }

        private void CoreThreadLoop()
        {
            Thread.CurrentThread.Name = "Core Thread";
            
            native_ = Engine_Init( width_, height_);

            Context.Init();

            eventInit?.Invoke();
            
            while(!shouldStop_)
            {
                Runframe();
            }

            eventShutdown?.Invoke();

            Engine_Shutdown(native_);

            native_ = IntPtr.Zero;
        }
        
        public void Runframe()
        {
            Engine_RunFrame(native_);
        }

    }
}
