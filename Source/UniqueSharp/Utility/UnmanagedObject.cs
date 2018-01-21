using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public class UnmanagedObject<T> : DisposeBase where T : struct, IDisposable
    {
        IntPtr dataPtr_;
        public ref T data => ref Utilities.As<T>(dataPtr_);

        public UnmanagedObject()
        {
            dataPtr_ = Utilities.Alloc<T>();
        }

        protected override void Destroy()
        {
            data.Dispose();

            Utilities.Free(dataPtr_);
        }
    }
}
