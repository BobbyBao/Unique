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

    public partial class Application : Object
    {
        public static OS OS => os_;
        private static OS os_;
                
        public Application(string[] args)
        {
            InitOS();

            native_ = Unique_Setup(args.Length, args);

            Setup();
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
        
        public void Run()
        {
            Unique_Start(native_, DeviceType.D3D11, IntPtr.Zero);

        }

        public static void Quit()
        {
            Unique_Shutdown();
        }
        
        protected virtual void Setup()
        {
            Subscribe((Startup e) =>
            {
                Start();
            });

            Subscribe((Shutdown e) =>
            {
                Shutdown();
            });

            Subscribe((GUI e) =>
            {
                OnGUI();
            });

            Subscribe((Update e) =>
            {
                UpdateFrame(e.timeStep);
            });
        }

        protected virtual void Start()
        {
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
        
        
    }
}
