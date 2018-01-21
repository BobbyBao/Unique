using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    public class String : UnmanagedObject<String.Data>
    {
        string str_;
        public String(string str)
        {
            str_ = str;
            data.Set(str);
        }

        public override string ToString()
        {
            return str_;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Data : IDisposable
        {
            public int length;
            public int capacity;
            public IntPtr buffer;

            public Data(string str)
            {
                length = 0;
                capacity = 0;
                buffer = IntPtr.Zero;

                Set(str);
            }

            public void Set(string str)
            {
                Dispose();

                buffer = Marshal.StringToHGlobalAnsi(str);
                length = str.Length;
                capacity = -1;
            }

            public void Dispose()
            {
                if (buffer != IntPtr.Zero && capacity == -1)
                {
                    Marshal.FreeHGlobal(buffer);
                }
            }

            public override string ToString()
            {
                return Marshal.PtrToStringAnsi(buffer);
            }
        }

    }
}
