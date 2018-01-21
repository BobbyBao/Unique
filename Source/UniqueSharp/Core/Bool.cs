using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Bool
    {
        public byte value;

        public Bool(bool v)
        {
            this.value = (byte)(v ? 1 : 0);
        }

        public Bool(byte value)
        {
            this.value = value;
        }
        
        /// <summary>
        /// Determines whether the specified <see cref="System.Object"/> is equal to this instance.
        /// </summary>
        /// <param name="other">The <see cref="System.Object"/> to compare with this instance.</param>
        /// <returns>
        ///   <c>true</c> if the specified <see cref="System.Object"/> is equal to this instance; otherwise, <c>false</c>.
        /// </returns>
        [MethodImpl((MethodImplOptions)0x100)] // MethodImplOptions.AggressiveInlining
        public bool Equals(ref Bool other)
        {
            return other.value == value;
        }

        /// <summary>
        /// Determines whether the specified <see cref="System.Object"/> is equal to this instance.
        /// </summary>
        /// <param name="other">The <see cref="System.Object"/> to compare with this instance.</param>
        /// <returns>
        ///   <c>true</c> if the specified <see cref="System.Object"/> is equal to this instance; otherwise, <c>false</c>.
        /// </returns>
        [MethodImpl((MethodImplOptions)0x100)] // MethodImplOptions.AggressiveInlining
        public bool Equals(Bool other)
        {
            return Equals(ref other);
        }

        /// <inheritdoc/>
        public override bool Equals(object obj)
        {
            if (!(obj is Bool))
                return false;

            var strongValue = (Bool)obj;
            return Equals(ref strongValue);
        }
        
        /// <summary>
        /// Implements the operator ==.
        /// </summary>
        /// <param name="left">The left.</param>
        /// <param name="right">The right.</param>
        /// <returns>
        /// The result of the operator.
        /// </returns>
        [MethodImpl((MethodImplOptions)0x100)] // MethodImplOptions.AggressiveInlining
        public static bool operator ==(Bool left, Bool right)
        {
            return left.Equals(ref right);
        }

        /// <summary>
        /// Implements the operator !=.
        /// </summary>
        /// <param name="left">The left.</param>
        /// <param name="right">The right.</param>
        /// <returns>
        /// The result of the operator.
        /// </returns>
        [MethodImpl((MethodImplOptions)0x100)] // MethodImplOptions.AggressiveInlining
        public static bool operator !=(Bool left, Bool right)
        {
            return !left.Equals(ref right);
        }

        public override string ToString()
        {
            return string.Format(value == 0 ? "false" : "true");
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="bool"/> to <see cref="bool"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator Bool(bool value)
        {
            return new Bool(value);
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="Bool"/> to <see cref="Bool"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator bool(Bool value)
        {
            return value.value == 1;
        }
    }
}
