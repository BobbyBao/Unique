using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct VectorBase
    {
        /// Buffer start.
        public IntPtr buffer_;
        /// Buffer end.
        public IntPtr end_;
        /// Buffer capacity.
        public IntPtr capacity_;

        public long size => end_.ToInt64() - buffer_.ToInt64();
        public long capacity => capacity_.ToInt64() - buffer_.ToInt64();
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct Vector<T> where T : struct
    {
        /// Buffer start.
        public IntPtr buffer_;
        /// Buffer end.
        public IntPtr end_;
        /// Buffer capacity.
        public IntPtr capacity_;

        public long size => (end_.ToInt64() - buffer_.ToInt64()) / Unsafe.SizeOf<T>();
        public long capacity => (capacity_.ToInt64() - buffer_.ToInt64()) / Unsafe.SizeOf<T>();

        public ref T this[int index]
        {
            get
            {
                if (index >= size)
                {
                    throw new ArgumentOutOfRangeException();
                }

                return ref Utilities.At<T>(buffer_, index);
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
            if (buffer_ != IntPtr.Zero)
            {
                Utilities.Free(buffer_);
                buffer_ = IntPtr.Zero;
            }
        }
        
        public ref T Front()
        {
            return ref Utilities.As<T>(buffer_);
        }

        public ref T Back()
        {            
            return ref Unsafe.Subtract(ref Utilities.As<T>(buffer_), 1);
        }

        public ref T At(int index)
        {
            return ref Utilities.At<T>(buffer_, index);
        }

        public unsafe void Add(T val)
        {
            if (end_ == capacity_)
            {
                Grow();
            }
            
            Utilities.Write(end_, ref val);
            end_ = end_ + Unsafe.SizeOf<T>();
        }

        public void AddRange(IEnumerable<T> values)
        {
            foreach (T v in values)
                Add(v);
        }

        public void RemoveAt(int idx)
        {
            this[idx] = Back();
            end_ = end_ - Unsafe.SizeOf<T>();
        }

        public void Clear()
        {
            end_ = buffer_;
        }

        public void Resize(int sz)
        {
            int cap = (int)capacity;
            if (sz > cap)
            {
                while (sz > cap)
                {
                    cap *= 2;
                }

                Grow(cap);
            }

            end_ = buffer_ + sz * Unsafe.SizeOf<T>();
        }

        void Grow()
        {
            int cap = (int)capacity;
            int newCap = ((cap == 0) ? 4 : cap * 2);
            Grow(newCap);
        }

        void Grow(int cap)
        {
            if(buffer_ == IntPtr.Zero)
            {
                buffer_ = Utilities.AllocArray<T>(cap);
                end_ = buffer_;
            }
            else
            {
                int sz = (int)size;
                buffer_ = Utilities.Resize<T>(buffer_, cap);
                end_ = buffer_ + sz * Unsafe.SizeOf<T>();
            }

            capacity_ = buffer_ + cap * Unsafe.SizeOf<T>();
        }
    }
    
}
