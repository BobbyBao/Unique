#region --- License ---
/*
Copyright (c) 2006 - 2008 The Open Toolkit library.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#endregion

using System;
using System.Runtime.InteropServices;
namespace Unique.Engine
{
	/// <summary>Represents a 2D vector using two single-precision inting-point numbers.</summary>
	/// <remarks>
	/// The IntVector2 structure is suitable for interoperation with unmanaged code requiring two consecutive ints.
	/// </remarks>
	[StructLayout(LayoutKind.Sequential)]
	public struct IntVector2 : IEquatable<IntVector2>
	{
		#region Fields

		/// <summary>
		/// The X component of the IntVector2.
		/// </summary>
		public int x;

		/// <summary>
		/// The Y component of the IntVector2.
		/// </summary>
		public int y;

		#endregion

		#region Constructors

		/// <summary>
		/// Constructs a new IntVector2.
		/// </summary>
		/// <param name="x">The x coordinate of the net IntVector2.</param>
		/// <param name="y">The y coordinate of the net IntVector2.</param>
		public IntVector2(int x, int y)
		{
			this.x = x;
			this.y = y;
		}

		/// <summary>
		/// Constructs a new IntVector2 from the given IntVector2.
		/// </summary>
		/// <param name="v">The IntVector2 to copy components from.</param>
		[Obsolete]
		public IntVector2(IntVector2 v)
		{
			x = v.x;
			y = v.y;
		}

		/// <summary>
		/// Constructs a new IntVector2 from the given Vector2.
		/// </summary>
		/// <param name="v">The IntVector2 to copy components from.</param>
		[Obsolete]
		public IntVector2(Vector2 v)
		{
			x = (int) v.X;
			y = (int) v.Y;
		}

		#endregion

		#region Public Members

		#region Instance
                

		#region public void Div()

		/// <summary>Divide this instance by a scalar.</summary>
		/// <param name="f">Scalar operand.</param>
		[Obsolete("Use static Divide() method instead.")]
		public void Div(int f)
		{
			this.x =  x / f;
			this.y = y / f;
		}

		#endregion public void Div()

		#region public int Length

		/// <summary>
		/// Gets the length (magnitude) of the vector.
		/// </summary>
		/// <see cref="LengthFast"/>
		/// <seealso cref="LengthSquared"/>
		public int Length
		{
			get
			{
				return (int)System.Math.Sqrt(x * x + y * y);
			}
		}

		#endregion

		#region public int LengthFast

		/// <summary>
		/// Gets an approximation of the vector length (magnitude).
		/// </summary>
		/// <remarks>
		/// This property uses an approximation of the square root function to calculate vector magnitude, with
		/// an upper error bound of 0.001.
		/// </remarks>
		/// <see cref="Length"/>
		/// <seealso cref="LengthSquared"/>
		public int LengthFast
		{
			get
			{
				return (int) (1.0f / MathHelper.InverseSqrtFast(x * x + y * y));
			}
		}

		#endregion

		#region public int LengthSquared

		/// <summary>
		/// Gets the square of the vector length (magnitude).
		/// </summary>
		/// <remarks>
		/// This property avoids the costly square root operation required by the Length property. This makes it more suitable
		/// for comparisons.
		/// </remarks>
		/// <see cref="Length"/>
		/// <seealso cref="LengthFast"/>
		public int LengthSquared
		{
			get
			{
				return x * x + y * y;
			}
		}

		#endregion

		#region public IntVector2 PerpendicularRight

		/// <summary>
		/// Gets the perpendicular vector on the right side of this vector.
		/// </summary>
		public IntVector2 PerpendicularRight
		{
			get
			{
				return new IntVector2(y, -x);
			}
		}

		#endregion

		#region public IntVector2 PerpendicularLeft

		/// <summary>
		/// Gets the perpendicular vector on the left side of this vector.
		/// </summary>
		public IntVector2 PerpendicularLeft
		{
			get
			{
				return new IntVector2(-y, x);
			}
		}

		#endregion

		#region public void Normalize()

		/// <summary>
		/// Scales the IntVector2 to unit length.
		/// </summary>
		public void Normalize()
		{
			x =  x / this.Length;
			y *= y / this.Length;
		}

		#endregion

		#region public void NormalizeFast()

		/// <summary>
		/// Scales the IntVector2 to approximately unit length.
		/// </summary>
		public void NormalizeFast()
		{
			int scale = (int) MathHelper.InverseSqrtFast(x * x + y * y);
			x *= scale;
			y *= scale;
		}

		#endregion

		#region public void Scale()

		/// <summary>
		/// Scales the current IntVector2 by the given amounts.
		/// </summary>
		/// <param name="sx">The scale of the X component.</param>
		/// <param name="sy">The scale of the Y component.</param>
		[Obsolete("Use static Multiply() method instead.")]
		public void Scale(int sx, int sy)
		{
			this.x = x * sx;
			this.y = y * sy;
		}

		/// <summary>Scales this instance by the given parameter.</summary>
		/// <param name="scale">The scaling of the individual components.</param>
		[Obsolete("Use static Multiply() method instead.")]
		public void Scale(IntVector2 scale)
		{
			this.x *= scale.x;
			this.y *= scale.y;
		}
        
		#endregion public void Scale()

		#endregion

		#region Static

		#region Fields

		/// <summary>
		/// Defines a unit-length IntVector2 that points towards the X-axis.
		/// </summary>
		public static readonly IntVector2 UnitX = new IntVector2(1, 0);

		/// <summary>
		/// Defines a unit-length IntVector2 that points towards the Y-axis.
		/// </summary>
		public static readonly IntVector2 UnitY = new IntVector2(0, 1);

		/// <summary>
		/// Defines a zero-length IntVector2.
		/// </summary>
		public static readonly IntVector2 Zero = new IntVector2(0, 0);

		/// <summary>
		/// Defines an instance with all components set to 1.
		/// </summary>
		public static readonly IntVector2 One = new IntVector2(1, 1);

		/// <summary>
		/// Defines the size of the IntVector2 struct in bytes.
		/// </summary>
		public static readonly int SizeInBytes = Marshal.SizeOf(new IntVector2());

		#endregion

		#region Add

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="a">Left operand.</param>
		/// <param name="b">Right operand.</param>
		/// <returns>Result of operation.</returns>
		public static IntVector2 Add(IntVector2 a, IntVector2 b)
		{
			Add(ref a, ref b, out a);
			return a;
		}

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="a">Left operand.</param>
		/// <param name="b">Right operand.</param>
		/// <param name="result">Result of operation.</param>
		public static void Add(ref IntVector2 a, ref IntVector2 b, out IntVector2 result)
		{
			result = new IntVector2(a.x + b.x, a.y + b.y);
		}

		#endregion

		#region Subtract

		/// <summary>
		/// Subtract one Vector from another
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <returns>Result of subtraction</returns>
		public static IntVector2 Subtract(IntVector2 a, IntVector2 b)
		{
			Subtract(ref a, ref b, out a);
			return a;
		}

		/// <summary>
		/// Subtract one Vector from another
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <param name="result">Result of subtraction</param>
		public static void Subtract(ref IntVector2 a, ref IntVector2 b, out IntVector2 result)
		{
			result = new IntVector2(a.x - b.x, a.y - b.y);
		}

		#endregion

		#region Multiply

		/// <summary>
		/// Multiplies a vector by a scalar.
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of the operation.</returns>
		public static IntVector2 Multiply(IntVector2 vector, int scale)
		{
			Multiply(ref vector, scale, out vector);
			return vector;
		}

		/// <summary>
		/// Multiplies a vector by a scalar.
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <param name="result">Result of the operation.</param>
		public static void Multiply(ref IntVector2 vector, int scale, out IntVector2 result)
		{
			result = new IntVector2(vector.x * scale, vector.y * scale);
		}

		/// <summary>
		/// Multiplies a vector by the components a vector (scale).
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of the operation.</returns>
		public static IntVector2 Multiply(IntVector2 vector, IntVector2 scale)
		{
			Multiply(ref vector, ref scale, out vector);
			return vector;
		}

		/// <summary>
		/// Multiplies a vector by the components of a vector (scale).
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <param name="result">Result of the operation.</param>
		public static void Multiply(ref IntVector2 vector, ref IntVector2 scale, out IntVector2 result)
		{
			result = new IntVector2(vector.x * scale.x, vector.y * scale.y);
		}

		#endregion

		#region Divide

		/// <summary>
		/// Divides a vector by a scalar.
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of the operation.</returns>
		public static IntVector2 Divide(IntVector2 vector, int scale)
		{
			Divide(ref vector, scale, out vector);
			return vector;
		}

		/// <summary>
		/// Divides a vector by a scalar.
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <param name="result">Result of the operation.</param>
		public static void Divide(ref IntVector2 vector, int scale, out IntVector2 result)
		{
			Multiply(ref vector, 1 / scale, out result);
		}

		/// <summary>
		/// Divides a vector by the components of a vector (scale).
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of the operation.</returns>
		public static IntVector2 Divide(IntVector2 vector, IntVector2 scale)
		{
			Divide(ref vector, ref scale, out vector);
			return vector;
		}

		/// <summary>
		/// Divide a vector by the components of a vector (scale).
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <param name="result">Result of the operation.</param>
		public static void Divide(ref IntVector2 vector, ref IntVector2 scale, out IntVector2 result)
		{
			result = new IntVector2(vector.x / scale.x, vector.y / scale.y);
		}

		#endregion

		#region ComponentMin

		/// <summary>
		/// Calculate the component-wise minimum of two vectors
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <returns>The component-wise minimum</returns>
		public static IntVector2 ComponentMin(IntVector2 a, IntVector2 b)
		{
			a.x = a.x < b.x ? a.x : b.x;
			a.y = a.y < b.y ? a.y : b.y;
			return a;
		}

		/// <summary>
		/// Calculate the component-wise minimum of two vectors
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <param name="result">The component-wise minimum</param>
		public static void ComponentMin(ref IntVector2 a, ref IntVector2 b, out IntVector2 result)
		{
			result.x = a.x < b.x ? a.x : b.x;
			result.y = a.y < b.y ? a.y : b.y;
		}

		#endregion

		#region ComponentMax

		/// <summary>
		/// Calculate the component-wise maximum of two vectors
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <returns>The component-wise maximum</returns>
		public static IntVector2 ComponentMax(IntVector2 a, IntVector2 b)
		{
			a.x = a.x > b.x ? a.x : b.x;
			a.y = a.y > b.y ? a.y : b.y;
			return a;
		}

		/// <summary>
		/// Calculate the component-wise maximum of two vectors
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <param name="result">The component-wise maximum</param>
		public static void ComponentMax(ref IntVector2 a, ref IntVector2 b, out IntVector2 result)
		{
			result.x = a.x > b.x ? a.x : b.x;
			result.y = a.y > b.y ? a.y : b.y;
		}

		#endregion

		#region Min

		/// <summary>
		/// Returns the Vector3 with the minimum magnitude
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>The minimum Vector3</returns>
		public static IntVector2 Min(IntVector2 left, IntVector2 right)
		{
			return left.LengthSquared < right.LengthSquared ? left : right;
		}

		#endregion

		#region Max

		/// <summary>
		/// Returns the Vector3 with the minimum magnitude
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>The minimum Vector3</returns>
		public static IntVector2 Max(IntVector2 left, IntVector2 right)
		{
			return left.LengthSquared >= right.LengthSquared ? left : right;
		}

		#endregion

		#region Clamp

		/// <summary>
		/// Clamp a vector to the given minimum and maximum vectors
		/// </summary>
		/// <param name="vec">Input vector</param>
		/// <param name="min">Minimum vector</param>
		/// <param name="max">Maximum vector</param>
		/// <returns>The clamped vector</returns>
		public static IntVector2 Clamp(IntVector2 vec, IntVector2 min, IntVector2 max)
		{
			vec.x = vec.x < min.x ? min.x : vec.x > max.x ? max.x : vec.x;
			vec.y = vec.y < min.y ? min.y : vec.y > max.y ? max.y : vec.y;
			return vec;
		}

		/// <summary>
		/// Clamp a vector to the given minimum and maximum vectors
		/// </summary>
		/// <param name="vec">Input vector</param>
		/// <param name="min">Minimum vector</param>
		/// <param name="max">Maximum vector</param>
		/// <param name="result">The clamped vector</param>
		public static void Clamp(ref IntVector2 vec, ref IntVector2 min, ref IntVector2 max, out IntVector2 result)
		{
			result.x = vec.x < min.x ? min.x : vec.x > max.x ? max.x : vec.x;
			result.y = vec.y < min.y ? min.y : vec.y > max.y ? max.y : vec.y;
		}

		#endregion

		#region Normalize

		/// <summary>
		/// Scale a vector to unit length
		/// </summary>
		/// <param name="vec">The input vector</param>
		/// <returns>The normalized vector</returns>
		public static IntVector2 Normalize(IntVector2 vec)
		{
			vec.x = vec.x / vec.Length;
			vec.y = vec.y / vec.Length;
			return vec;
		}

		/// <summary>
		/// Scale a vector to unit length
		/// </summary>
		/// <param name="vec">The input vector</param>
		/// <param name="result">The normalized vector</param>
		public static void Normalize(ref IntVector2 vec, out IntVector2 result)
		{
			result.x = vec.x / vec.Length;
			result.y = vec.y / vec.Length;
		}

		#endregion

		#region NormalizeFast

		/// <summary>
		/// Scale a vector to approximately unit length
		/// </summary>
		/// <param name="vec">The input vector</param>
		/// <returns>The normalized vector</returns>
		public static IntVector2 NormalizeFast(IntVector2 vec)
		{
			int scale = (int) MathHelper.InverseSqrtFast(vec.x * vec.x + vec.y * vec.y);
			vec.x *= scale;
			vec.y *= scale;
			return vec;
		}

		/// <summary>
		/// Scale a vector to approximately unit length
		/// </summary>
		/// <param name="vec">The input vector</param>
		/// <param name="result">The normalized vector</param>
		public static void NormalizeFast(ref IntVector2 vec, out IntVector2 result)
		{
			int scale = (int) MathHelper.InverseSqrtFast(vec.x * vec.x + vec.y * vec.y);
			result.x = vec.x * scale;
			result.y = vec.y * scale;
		}

		#endregion

		#region Dot

		/// <summary>
		/// Calculate the dot (scalar) product of two vectors
		/// </summary>
		/// <param name="left">First operand</param>
		/// <param name="right">Second operand</param>
		/// <returns>The dot product of the two inputs</returns>
		public static int Dot(IntVector2 left, IntVector2 right)
		{
			return left.x * right.x + left.y * right.y;
		}

		/// <summary>
		/// Calculate the dot (scalar) product of two vectors
		/// </summary>
		/// <param name="left">First operand</param>
		/// <param name="right">Second operand</param>
		/// <param name="result">The dot product of the two inputs</param>
		public static void Dot(ref IntVector2 left, ref IntVector2 right, out int result)
		{
			result = left.x * right.x + left.y * right.y;
		}

		#endregion

		#region Lerp

		/// <summary>
		/// Returns a new Vector that is the linear blend of the 2 given Vectors
		/// </summary>
		/// <param name="a">First input vector</param>
		/// <param name="b">Second input vector</param>
		/// <param name="blend">The blend factor. a when blend=0, b when blend=1.</param>
		/// <returns>a when blend=0, b when blend=1, and a linear combination otherwise</returns>
		public static IntVector2 Lerp(IntVector2 a, IntVector2 b, int blend)
		{
			a.x = blend * (b.x - a.x) + a.x;
			a.y = blend * (b.y - a.y) + a.y;
			return a;
		}

		/// <summary>
		/// Returns a new Vector that is the linear blend of the 2 given Vectors
		/// </summary>
		/// <param name="a">First input vector</param>
		/// <param name="b">Second input vector</param>
		/// <param name="blend">The blend factor. a when blend=0, b when blend=1.</param>
		/// <param name="result">a when blend=0, b when blend=1, and a linear combination otherwise</param>
		public static void Lerp(ref IntVector2 a, ref IntVector2 b, int blend, out IntVector2 result)
		{
			result.x = blend * (b.x - a.x) + a.x;
			result.y = blend * (b.y - a.y) + a.y;
		}

		#endregion

		#region Barycentric

		/// <summary>
		/// Interpolate 3 Vectors using Barycentric coordinates
		/// </summary>
		/// <param name="a">First input Vector</param>
		/// <param name="b">Second input Vector</param>
		/// <param name="c">Third input Vector</param>
		/// <param name="u">First Barycentric Coordinate</param>
		/// <param name="v">Second Barycentric Coordinate</param>
		/// <returns>a when u=v=0, b when u=1,v=0, c when u=0,v=1, and a linear combination of a,b,c otherwise</returns>
		public static IntVector2 BaryCentric(IntVector2 a, IntVector2 b, IntVector2 c, int u, int v)
		{
			return a + u * (b - a) + v * (c - a);
		}

		/// <summary>Interpolate 3 Vectors using Barycentric coordinates</summary>
		/// <param name="a">First input Vector.</param>
		/// <param name="b">Second input Vector.</param>
		/// <param name="c">Third input Vector.</param>
		/// <param name="u">First Barycentric Coordinate.</param>
		/// <param name="v">Second Barycentric Coordinate.</param>
		/// <param name="result">Output Vector. a when u=v=0, b when u=1,v=0, c when u=0,v=1, and a linear combination of a,b,c otherwise</param>
		public static void BaryCentric(ref IntVector2 a, ref IntVector2 b, ref IntVector2 c, int u, int v, out IntVector2 result)
		{
			result = a; // copy

			IntVector2 temp = b; // copy
			Subtract(ref temp, ref a, out temp);
			Multiply(ref temp, u, out temp);
			Add(ref result, ref temp, out result);

			temp = c; // copy
			Subtract(ref temp, ref a, out temp);
			Multiply(ref temp, v, out temp);
			Add(ref result, ref temp, out result);
		}

		#endregion

		#endregion

		#region Operators

		/// <summary>
		/// Adds the specified instances.
		/// </summary>
		/// <param name="left">Left operand.</param>
		/// <param name="right">Right operand.</param>
		/// <returns>Result of addition.</returns>
		public static IntVector2 operator +(IntVector2 left, IntVector2 right)
		{
			left.x += right.x;
			left.y += right.y;
			return left;
		}

		/// <summary>
		/// Subtracts the specified instances.
		/// </summary>
		/// <param name="left">Left operand.</param>
		/// <param name="right">Right operand.</param>
		/// <returns>Result of subtraction.</returns>
		public static IntVector2 operator -(IntVector2 left, IntVector2 right)
		{
			left.x -= right.x;
			left.y -= right.y;
			return left;
		}

		/// <summary>
		/// Negates the specified instance.
		/// </summary>
		/// <param name="vec">Operand.</param>
		/// <returns>Result of negation.</returns>
		public static IntVector2 operator -(IntVector2 vec)
		{
			vec.x = -vec.x;
			vec.y = -vec.y;
			return vec;
		}

		/// <summary>
		/// Multiplies the specified instance by a scalar.
		/// </summary>
		/// <param name="vec">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of multiplication.</returns>
		public static IntVector2 operator *(IntVector2 vec, int scale)
		{
			vec.x *= scale;
			vec.y *= scale;
			return vec;
		}

		/// <summary>
		/// Multiplies the specified instance by a scalar.
		/// </summary>
		/// <param name="scale">Left operand.</param>
		/// <param name="vec">Right operand.</param>
		/// <returns>Result of multiplication.</returns>
		public static IntVector2 operator *(int scale, IntVector2 vec)
		{
			vec.x *= scale;
			vec.y *= scale;
			return vec;
		}

		/// <summary>
		/// Divides the specified instance by a scalar.
		/// </summary>
		/// <param name="vec">Left operand</param>
		/// <param name="scale">Right operand</param>
		/// <returns>Result of the division.</returns>
		public static IntVector2 operator /(IntVector2 vec, int scale)
		{
			vec.x = vec.x / scale;
			vec.y = vec.y / scale;
			return vec;
		}

		/// <summary>
		/// Compares the specified instances for equality.
		/// </summary>
		/// <param name="left">Left operand.</param>
		/// <param name="right">Right operand.</param>
		/// <returns>True if both instances are equal; false otherwise.</returns>
		public static bool operator ==(IntVector2 left, IntVector2 right)
		{
			return left.Equals(right);
		}

		/// <summary>
		/// Compares the specified instances for inequality.
		/// </summary>
		/// <param name="left">Left operand.</param>
		/// <param name="right">Right operand.</param>
		/// <returns>True if both instances are not equal; false otherwise.</returns>
		public static bool operator !=(IntVector2 left, IntVector2 right)
		{
			return !left.Equals(right);
		}

		#endregion

		#region Overrides

		#region public override string ToString()

		/// <summary>
		/// Returns a System.String that represents the current IntVector2.
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			return string.Format("({0}, {1})", x, y);
		}

		#endregion

		#region public override int GetHashCode()

		/// <summary>
		/// Returns the hashcode for this instance.
		/// </summary>
		/// <returns>A System.Int32 containing the unique hashcode for this instance.</returns>
		public override int GetHashCode()
		{
			return x.GetHashCode() ^ y.GetHashCode();
		}

		#endregion

		#region public override bool Equals(object obj)

		/// <summary>
		/// Indicates whether this instance and a specified object are equal.
		/// </summary>
		/// <param name="obj">The object to compare to.</param>
		/// <returns>True if the instances are equal; false otherwise.</returns>
		public override bool Equals(object obj)
		{
			if (!(obj is IntVector2))
				return false;

			return this.Equals((IntVector2)obj);
		}

		#endregion

		#endregion

		#endregion

		#region IEquatable<IntVector2> Members

		/// <summary>Indicates whether the current vector is equal to another vector.</summary>
		/// <param name="other">A vector to compare with this vector.</param>
		/// <returns>true if the current vector is equal to the vector parameter; otherwise, false.</returns>
		public bool Equals(IntVector2 other)
		{
			return
				x == other.x &&
				y == other.y;
		}

		#endregion


		/// <summary>
		/// Distance between two 2D points (should be moved to IntVector2).
		/// </summary>
		public static float Distance(IntVector2 v1, IntVector2 v2)
		{
			return (float)Math.Sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
		}

		public bool IsEmpty => x == 0 && y == 0;
	}
}
