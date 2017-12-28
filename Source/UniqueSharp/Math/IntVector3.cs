using System;
using System.Runtime.InteropServices;

namespace Unique.Engine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct IntVector3 : IEquatable<IntVector3>
    {      
        /// Zero vector.
        public static readonly IntVector3 Zero = new IntVector3(0, 0, 0);
        /// (-1,0,0) vector.
        public static readonly IntVector3 Left = new IntVector3(-1, 0, 0);
        /// (1,0,0) vector.
        public static readonly IntVector3 Right = new IntVector3(1, 0, 0);
        /// (0,1,0) vector.
        public static readonly IntVector3 Up = new IntVector3(0, 1, 0);
        /// (0,-1,0) vector.
        public static readonly IntVector3 Down = new IntVector3(0, -1, 0);
        /// (0,0,1) vector.
        public static readonly IntVector3 Forward = new IntVector3(0, 0, 1);
        /// (0,0,-1) vector.
        public static readonly IntVector3 Back = new IntVector3(0, 0, -1);
        /// (1,1,1) vector.
        public static readonly IntVector3 One = new IntVector3(1, 1, 1);

        public int x, y, z;

        public IntVector3(int x, int y, int z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public bool Equals(ref IntVector3 other)
        {
            return x == other.x && y == other.y && z == other.z;
        }

        public bool Equals(IntVector3 other)
        {
            return Equals(ref other);
        }

        public override bool Equals(object obj)
        {
            if (!(obj is IntVector3))
                return false;

            return this.Equals((IntVector3)obj);
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() ^ z.GetHashCode();
        }

        public override string ToString()
        {
            return string.Format("({0}, {1}, {2})", x, y, z);
        }

        /// Return length.
        float Length() { return (float)Math.Sqrt((float)(x* x + y* y + z* z)); }

    #region Operators

        /// <summary>
        /// Adds the specified instances.
        /// </summary>
        /// <param name="left">Left operand.</param>
        /// <param name="right">Right operand.</param>
        /// <returns>Result of addition.</returns>
        public static IntVector3 operator +(IntVector3 left, IntVector3 right)
        {
            left.x += right.x;
            left.y += right.y;
            left.z += right.z;
            return left;
        }

        /// <summary>
        /// Subtracts the specified instances.
        /// </summary>
        /// <param name="left">Left operand.</param>
        /// <param name="right">Right operand.</param>
        /// <returns>Result of subtraction.</returns>
        public static IntVector3 operator -(IntVector3 left, IntVector3 right)
        {
            left.x -= right.x;
            left.y -= right.y;
            left.z -= right.z;
            return left;
        }

        /// <summary>
        /// Negates the specified instance.
        /// </summary>
        /// <param name="vec">Operand.</param>
        /// <returns>Result of negation.</returns>
        public static IntVector3 operator -(IntVector3 vec)
        {
            vec.x = -vec.x;
            vec.y = -vec.y;
            vec.z = -vec.z;
            return vec;
        }

        /// <summary>
        /// Multiplies the specified instance by a scalar.
        /// </summary>
        /// <param name="vec">Left operand.</param>
        /// <param name="scale">Right operand.</param>
        /// <returns>Result of multiplication.</returns>
        public static IntVector3 operator *(IntVector3 vec, int scale)
        {
            vec.x *= scale;
            vec.y *= scale;
            vec.z *= scale;
            return vec;
        }

        /// <summary>
        /// Multiplies the specified instance by a scalar.
        /// </summary>
        /// <param name="scale">Left operand.</param>
        /// <param name="vec">Right operand.</param>
        /// <returns>Result of multiplication.</returns>
        public static IntVector3 operator *(int scale, IntVector3 vec)
        {
            vec.x *= scale;
            vec.y *= scale;
            vec.z *= scale;
            return vec;
        }

        /// <summary>
        /// Divides the specified instance by a scalar.
        /// </summary>
        /// <param name="vec">Left operand</param>
        /// <param name="scale">Right operand</param>
        /// <returns>Result of the division.</returns>
        public static IntVector3 operator /(IntVector3 vec, int scale)
        {
            vec.x = vec.x / scale;
            vec.y = vec.y / scale;
            vec.z = vec.z / scale;
            return vec;
        }

        /// <summary>
        /// Compares the specified instances for equality.
        /// </summary>
        /// <param name="left">Left operand.</param>
        /// <param name="right">Right operand.</param>
        /// <returns>True if both instances are equal; false otherwise.</returns>
        public static bool operator ==(IntVector3 left, IntVector3 right)
        {
            return left.Equals(right);
        }

        /// <summary>
        /// Compares the specified instances for inequality.
        /// </summary>
        /// <param name="left">Left operand.</param>
        /// <param name="right">Right operand.</param>
        /// <returns>True if both instances are not equal; false otherwise.</returns>
        public static bool operator !=(IntVector3 left, IntVector3 right)
        {
            return !left.Equals(right);
        }

        #endregion
    }
}
