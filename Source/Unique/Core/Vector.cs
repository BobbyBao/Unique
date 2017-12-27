using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct VectorBase
    {
        /// Size of vector.
        public int size;
        /// Buffer capacity.
        public int capacity;
        /// Buffer.
        public IntPtr buffer;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct Vector<T> where T : struct
    {
        /// Size of vector.
        public int size;
        /// Buffer capacity.
        public int capacity;
        /// Buffer.
        public IntPtr buffer;
        
        public ref T this[int index]
        {
            get
            {
                if (index >= size)
                {
                    throw new ArgumentOutOfRangeException();
                }

                return ref Utilities.At<T>(buffer, index);
            }
        }

        public bool Empty
        {
            get
            {
                return size == 0;
            }
        }

        public void Dispose()
        {
            if (buffer != IntPtr.Zero)
            {
                Utilities.Free(buffer);
                buffer = IntPtr.Zero;
            }
        }
        
        public ref T Front()
        {
            return ref Utilities.As<T>(buffer);
        }

        public ref T Back()
        {
            return ref Utilities.At<T>(buffer, size - 1);
        }

        public ref T At(int index)
        {
            return ref Utilities.At<T>(buffer, index);
        }

        public unsafe void Add(T val)
        {
            if (size == capacity)
            {
                Grow();
            }

            At(size) = val;
            //Unsafe.Write((void*)(buffer + size * Unsafe.SizeOf<T>()), val);
            size++;
        }

        public void AddRange(IEnumerable<T> values)
        {
            foreach (T v in values)
                Add(v);
        }

        public void RemoveAt(int idx)
        {
            this[idx] = Back();
            size--;
        }

        public void Clear()
        {
            size = 0;
        }

        public void Resize(int sz)
        {
            int cap = capacity;
            if (sz > cap)
            {
                while (sz > cap)
                {
                    cap *= 2;
                }

                Grow(cap);
            }

            size = sz;
        }

        void Grow()
        {
            int newCap = ((capacity == 0) ? 4 : capacity * 2);
            Grow(newCap);
        }

        void Grow(int cap)
        {
            if(buffer == IntPtr.Zero)
            {
                buffer = Utilities.AllocArray<T>(cap);
            }
            else
            {
                buffer = Utilities.Resize<T>(buffer, capacity);
            }

            capacity = cap;
        }
    }
    
}
