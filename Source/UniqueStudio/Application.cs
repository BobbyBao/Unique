using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace UniqueStudio
{
    public class Application
    {
        public Application()
        {
        }

        public void Run()
        {
            Unique_Start("Direct3D11", IntPtr.Zero);
        }

        public static void Quit()
        {
            Unique_Shutdown();
        }

#if DEBUG
        public const string DllName = "UniqueD.dll";
#else
        public const string DllName = "Unique.dll";
#endif

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Unique_Setup(int argc, [MarshalAs(UnmanagedType.LPStr)]string[] argv);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern int Unique_Start([MarshalAs(UnmanagedType.LPStr)]string renderer, IntPtr window);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Unique_Shutdown();
    }
}
