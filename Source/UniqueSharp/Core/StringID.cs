using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public struct StringID : IEquatable<StringID>
    {
        public static readonly StringID NONE = new StringID();
        
        IntPtr internalData_;
        unsafe struct InternalData
        {
            public InternalData* next;
            public fixed sbyte chars[1];
        }

        public StringID(string name)
        {
            internalData_ = Native.StringID_new(name);
        }

        public StringID(IntPtr nativeData)
        {
            internalData_ = nativeData;
        }

        public unsafe IntPtr cstr
        {
            get
            {
                return (IntPtr)(&(((InternalData*)internalData_)->chars[0]));
            }
        }

        [MethodImpl((MethodImplOptions)0x100)]
        public bool Equals(ref StringID other)
        {
            return other.internalData_ == internalData_;
        }
        
        [MethodImpl((MethodImplOptions)0x100)] // MethodImplOptions.AggressiveInlining
        public bool Equals(StringID other)
        {
            return Equals(ref other);
        }
        
        public override bool Equals(object obj)
        {
            if(!(obj is StringID))
                return false;

            var strongValue = (StringID)obj;
            return Equals(ref strongValue);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return cstr.GetHashCode();
            }
        }
                
        [MethodImpl((MethodImplOptions)0x100)] // MethodImplOptions.AggressiveInlining
        public static bool operator ==(StringID left, StringID right)
        {
            return left.Equals(ref right);
        }

        [MethodImpl((MethodImplOptions)0x100)] // MethodImplOptions.AggressiveInlining
        public static bool operator !=(StringID left, StringID right)
        {
            return !left.Equals(ref right);
        }

        public override string ToString()
        {
            return Marshal.PtrToStringAnsi(cstr);
        }

        public static implicit operator StringID(string value)
        {
            return new StringID(value);
        }

        public static implicit operator string(StringID value)
        {
            return value.ToString();
        }
    }
}
