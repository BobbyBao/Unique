using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Text;

namespace UniqueEngine
{
    public class RefCounted : DisposeBase
    {
        protected static Dictionary<IntPtr, WeakReference<RefCounted>> ptrToObject_ = new Dictionary<IntPtr, WeakReference<RefCounted>>();

        public IntPtr nativePtr => native_;
        internal ref Data refCount => ref Utilities.As<Data>(refCount_);
        public int refs => refCount.refs_;
        public int weakRefs => refCount.weakRefs_ - 1;
        public bool expired => (refCount_ != IntPtr.Zero) ? refs < 0 : true;
        protected IntPtr native_;
        protected IntPtr refCount_;

        public RefCounted()
        {
        }

        public RefCounted(IntPtr ptr)
        {
            Attach(ptr);
        }

        public void Attach(IntPtr ptr)
        {
            native_ = ptr;

            unsafe
            {
                refCount_ = *(IntPtr*)(native_ + 8);
            }

            ptrToObject_[native_] = new WeakReference<RefCounted>(this);

            AddRef();
        }

        private void AddRef()
        {
            System.Diagnostics.Debug.Assert(refCount.weakRefs_ >= 0);

            ++(refCount.weakRefs_);

        }

        private void ReleaseRef()
        {
            System.Diagnostics.Debug.Assert(refCount.weakRefs_ > 0);
            --(refCount.weakRefs_);

            if(expired && 0 == refCount.weakRefs_)
            {
                Native.Object_Delete(refCount_);
                refCount_ = IntPtr.Zero;
            }

        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static implicit operator bool(RefCounted obj)
        {
            return obj != null;
        }

        internal struct Data
        {
            /// Reference count. If below zero, the Object_ has been destroyed.
            public int refs_;
            /// Weak reference count.
            public int weakRefs_;
        }

    }
}
