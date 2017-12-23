using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace UniqueStudio
{  
    /// Device type
    enum DeviceType : Int32
    {
        Undefined = 0,  ///< Undefined device
        D3D11,      ///< D3D11 device
        D3D12,      ///< D3D12 device
        OpenGL,     ///< OpenGL device 
        OpenGLES    ///< OpenGLES device
    }

    public class Application
    {
        public Application()
        {
        }
        public Application(string[] args)
        {
            Unique_Setup(args.Length, args);
        }

        public void Run()
        {
            Unique_Start(DeviceType.D3D11, IntPtr.Zero);
        }

        public static void Quit()
        {
            Unique_Shutdown();
        }

#if DEBUG
        public const string DllName = "UniqueNativeD.dll";
#else
        public const string DllName = "UniqueNative.dll";
#endif

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Unique_Setup(int argc, [MarshalAs(UnmanagedType.LPStr)]string[] argv);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Unique_Start(DeviceType deviceType, IntPtr window);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Unique_Shutdown();
    }
}
