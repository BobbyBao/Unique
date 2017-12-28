using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{

    /// Two-dimensional bounding rectangle with integer values.
    [StructLayout(LayoutKind.Sequential)]
    public struct IntRect : IEquatable<IntRect>
    {
        /// Left coordinate.
        public int left;
        /// Top coordinate.
        public int top;
        /// Right coordinate.
        public int right;
        /// Bottom coordinate.
        public int bottom;

        /// Zero-sized rect.
        public static readonly IntRect ZERO = new IntRect();

        /// Construct from coordinates.
        public IntRect(int left, int top, int right, int bottom)
        {
            this.left = left;
            this.top = top;
            this.right = right;
            this.bottom = bottom;

        }
        
        /// Test for equality with another rect.
        public static bool operator ==(IntRect lhs, IntRect rhs)
        {
            return lhs.left == rhs.left && lhs.top == rhs.top && lhs.right == rhs.right && lhs.bottom == rhs.bottom;
        }

        /// Test for inequality with another rect.
        public static bool operator !=(IntRect lhs, IntRect rhs)
        {
            return lhs.left != rhs.left || lhs.top != rhs.top || lhs.right != rhs.right || lhs.bottom != rhs.bottom;
        }

        /// Return size.
        public IntVector2 Size() { return new IntVector2(Width(), Height()); }

        /// Return width.
        public int Width() { return right - left; }

        /// Return height.
        public int Height() { return bottom - top; }

        /// Test whether a point is inside.
        public Intersection IsInside(ref IntVector2 point)
        {
            if (point.x < left || point.y < top || point.x >= right || point.y >= bottom)
                return Intersection.OUTSIDE;
            else
                return Intersection.INSIDE;
        }

        /// Return integer data.
        //const int* Data() const { return &left_; }

        /// Return as string.
        public override string ToString()
        {
            return string.Format("{0} {1} {2} {3}", left, top, right, bottom);
        }

        public override bool Equals(object obj)
        {
            if(obj is IntRect)
            {
                return Equals((IntRect)obj);
            }

            return false;
        }

        public override int GetHashCode()
        {
            return left.GetHashCode() ^ top.GetHashCode() ^ right.GetHashCode() ^ bottom.GetHashCode();
        }

        public bool Equals(IntRect other)
        {
            return this == other;
        }
    }

}
