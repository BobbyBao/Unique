using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace Unique.Engine
{
    public enum OS
    {
        Windows,
        Linux,
        MacOSX,
        Unknown
    }

    public class Application
    {
        public static OS OS => os_;
        private static OS os_;

        private static bool shouldClose_ = false;

        private Engine engine_;

        protected GameWindow window_;
        public Application()
        {
            InitOS();

            engine_ = new Engine();

            engine_.eventInit += Init;
            engine_.eventShutdown += Shutdown;
        }
        
        public static void Quit()
        {
            shouldClose_ = true;
        }

        private static void InitOS()
        {
            if(RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                os_ = OS.Windows;
            }
            else if(RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                os_ = OS.Linux;
            }
            else if(RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                os_ = OS.MacOSX;
            }
            else
            {
                os_ = OS.Unknown;
            }
        }

        protected virtual void Setup()
        {
        }

        protected virtual void Init()
        {
            engine_.SubscribeToEvent<BeginFrame>(Handle);

            engine_.SubscribeToEvent((GUI e) =>
            {
                OnGUI();
            });

            engine_.SubscribeToEvent((Update e) =>
            {
                UpdateFrame(e.timeStep);
            });
        }

        protected virtual void Shutdown()
        {
        }

        protected virtual void OnGUI()
        {
        }

        protected virtual void UpdateFrame(float timeStep)
        {
        }

        public void Run(GameWindow w)
        {
            window_ = w;

            Thread.CurrentThread.Name = "OS Thread";

            Setup();

            engine_.Initialize(window_.Handle, window_.Width, window_.Height);

            engine_.Start();

            OSThreadLoop();

            engine_.Stop();

            window_.Close();
        }

        private void OSThreadLoop()
        {
            while(!shouldClose_)
            {
                window_.RunMessageLoop();
            }
        }

        void Handle(BeginFrame e)
        {
            window_.ProcessEvents();
        }

        void Handle(Update e)
        {

        }
    }
}
