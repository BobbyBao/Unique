using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public class DisposeBase : IDisposable
    {
        private bool disposed;
        public void Dispose()
        {
            Dispose(true);
            // Tell the garbage collector not to call the finalizer
            // since all the cleanup will already be done.
            GC.SuppressFinalize(this);
        }

        // If disposing is true, it was called explicitly and we should dispose managed objects.
        // If disposing is false, it was called by the finalizer and managed objects should not be disposed.
        protected virtual void Dispose(bool disposing)
        {
            if(!disposed)
            {
                if(disposing)
                {
                    Destroy();
                }
                
                disposed = true;
            }
        }

        protected virtual void Destroy()
        {
        }
    }
}
