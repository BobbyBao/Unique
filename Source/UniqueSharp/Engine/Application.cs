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

        protected DeviceType deviceType { get; set; } = DeviceType.D3D11;
        public Application() : this(new string[0])
        {
        }
                
        public Application(string[] args)
        {
            InitOS();

            native_ = Unique_Setup(args.Length, args);

            Context.Init();

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
        
        public void Start()
        {
            Unique_Start(native_, deviceType, IntPtr.Zero);

        }

        public static void Quit()
        {
            Unique_Shutdown();
        }
        
        protected virtual void Setup()
        {
            Subscribe((Startup e) =>
            {
                Initialize();
            });

            Subscribe((Shutdown e) =>
            {
                Shutdown();
            });

            Subscribe<GUIEvent>(HandleGUIEvent);

            Subscribe<Update>(HandleUpdate);
        }

        void HandleGUIEvent(GUIEvent e) => OnGUI();
        void HandleUpdate(Update e) => UpdateFrame(e.timeStep);

        protected virtual void Initialize()
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
