#region --- License ---
/* Licensed under the MIT/X11 license.
 * Copyright (c) 2006-2008 the OpenTK Team.
 * This notice may not be removed from any source distribution.
 * See license.txt for licensing detailed licensing details.
 * 
 * Contributions by Andy Gill, James Talton and Georg Wächter.
 */
#endregion

using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    /// Intersection test result.
    public enum Intersection
    {
        OUTSIDE,
        INTERSECTS,
        INSIDE
    }
    /// <summary>
    /// Contains common mathematical functions and constants.
    /// </summary>
    public static class MathHelper
	{

        #region Fields

        /// <summary>
        /// Defines the value of Pi as a <see cref="System.Single"/>.
        /// </summary>
        public const float Pi = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930382f;

		/// <summary>
		/// Defines the value of Pi divided by two as a <see cref="System.Single"/>.
		/// </summary>
		public const float PiOver2 = Pi / 2;

		/// <summary>
		/// Defines the value of Pi divided by three as a <see cref="System.Single"/>.
		/// </summary>
		public const float PiOver3 = Pi / 3;

		/// <summary>
		/// Definesthe value of  Pi divided by four as a <see cref="System.Single"/>.
		/// </summary>
		public const float PiOver4 = Pi / 4;

		/// <summary>
		/// Defines the value of Pi divided by six as a <see cref="System.Single"/>.
		/// </summary>
		public const float PiOver6 = Pi / 6;

		/// <summary>
		/// Defines the value of Pi multiplied by two as a <see cref="System.Single"/>.
		/// </summary>
		public const float TwoPi = 2 * Pi;

		/// <summary>
		/// Defines the value of Pi multiplied by 3 and divided by two as a <see cref="System.Single"/>.
		/// </summary>
		public const float ThreePiOver2 = 3 * Pi / 2;

		/// <summary>
		/// Defines the value of E as a <see cref="System.Single"/>.
		/// </summary>
		public const float E = 2.71828182845904523536f;

		/// <summary>
		/// Defines the base-10 logarithm of E.
		/// </summary>
		public const float Log10E = 0.434294482f;

		/// <summary>
		/// Defines the base-2 logarithm of E.
		/// </summary>
		public const float Log2E = 1.442695041f;

		public static readonly float DTORF = Pi / 180.0f;

		#endregion

		#region Public Members

		#region NextPowerOfTwo

		/// <summary>
		/// Returns the next power of two that is larger than the specified number.
		/// </summary>
		/// <param name="n">The specified number.</param>
		/// <returns>The next power of two.</returns>
		public static long NextPowerOfTwo(long n)
		{
			if (n < 0) throw new ArgumentOutOfRangeException("n", "Must be positive.");
			return (long)System.Math.Pow(2, System.Math.Ceiling(System.Math.Log((double)n, 2)));
		}

		/// <summary>
		/// Returns the next power of two that is larger than the specified number.
		/// </summary>
		/// <param name="n">The specified number.</param>
		/// <returns>The next power of two.</returns>
		public static int NextPowerOfTwo(int n)
		{
			if (n < 0) throw new ArgumentOutOfRangeException("n", "Must be positive.");
			return (int)System.Math.Pow(2, System.Math.Ceiling(System.Math.Log((double)n, 2)));
		}

		/// <summary>
		/// Returns the next power of two that is larger than the specified number.
		/// </summary>
		/// <param name="n">The specified number.</param>
		/// <returns>The next power of two.</returns>
		public static float NextPowerOfTwo(float n)
		{
			if (n < 0) throw new ArgumentOutOfRangeException("n", "Must be positive.");
			return (float)System.Math.Pow(2, System.Math.Ceiling(System.Math.Log((double)n, 2)));
		}

		/// <summary>
		/// Returns the next power of two that is larger than the specified number.
		/// </summary>
		/// <param name="n">The specified number.</param>
		/// <returns>The next power of two.</returns>
		public static double NextPowerOfTwo(double n)
		{
			if (n < 0) throw new ArgumentOutOfRangeException("n", "Must be positive.");
			return System.Math.Pow(2, System.Math.Ceiling(System.Math.Log((double)n, 2)));
		}

		#endregion

		#region Factorial

		/// <summary>Calculates the factorial of a given natural number.
		/// </summary>
		/// <param name="n">The number.</param>
		/// <returns>n!</returns>
		public static long Factorial(int n)
		{
			long result = 1;

			for (; n > 1; n--)
				result *= n;

			return result;
		}

		#endregion

		#region BinomialCoefficient

		/// <summary>
		/// Calculates the binomial coefficient <paramref name="n"/> above <paramref name="k"/>.
		/// </summary>
		/// <param name="n">The n.</param>
		/// <param name="k">The k.</param>
		/// <returns>n! / (k! * (n - k)!)</returns>
		public static long BinomialCoefficient(int n, int k)
		{
			return Factorial(n) / (Factorial(k) * Factorial(n - k));
		}

		#endregion

		#region InverseSqrtFast

		/// <summary>
		/// Returns an approximation of the inverse square root of left number.
		/// </summary>
		/// <param name="x">A number.</param>
		/// <returns>An approximation of the inverse square root of the specified number, with an upper error bound of 0.001</returns>
		/// <remarks>
		/// This is an improved implementation of the the method known as Carmack's inverse square root
		/// which is found in the Quake III source code. This implementation comes from
		/// http://www.codemaestro.com/reviews/review00000105.html. For the history of this method, see
		/// http://www.beyond3d.com/content/articles/8/
		/// </remarks>
		public static float InverseSqrtFast(float x)
		{
			unsafe
			{
				float xhalf = 0.5f * x;
				int i = *(int*)&x;              // Read bits as integer.
				i = 0x5f375a86 - (i >> 1);      // Make an initial guess for Newton-Raphson approximation
				x = *(float*)&i;                // Convert bits back to float
				x = x * (1.5f - xhalf * x * x); // Perform left single Newton-Raphson step.
				return x;
			}
		}

		/// <summary>
		/// Returns an approximation of the inverse square root of left number.
		/// </summary>
		/// <param name="x">A number.</param>
		/// <returns>An approximation of the inverse square root of the specified number, with an upper error bound of 0.001</returns>
		/// <remarks>
		/// This is an improved implementation of the the method known as Carmack's inverse square root
		/// which is found in the Quake III source code. This implementation comes from
		/// http://www.codemaestro.com/reviews/review00000105.html. For the history of this method, see
		/// http://www.beyond3d.com/content/articles/8/
		/// </remarks>
		public static double InverseSqrtFast(double x)
		{
			return InverseSqrtFast((float)x);
			// TODO: The following code is wrong. Fix it, to improve precision.
#if false
			unsafe
			{
				double xhalf = 0.5f * x;
				int i = *(int*)&x;              // Read bits as integer.
				i = 0x5f375a86 - (i >> 1);      // Make an initial guess for Newton-Raphson approximation
				x = *(float*)&i;                // Convert bits back to float
				x = x * (1.5f - xhalf * x * x); // Perform left single Newton-Raphson step.
				return x;
			}
#endif
		}

		#endregion

		#region DegreesToRadians

		/// <summary>
		/// Convert degrees to radians
		/// </summary>
		/// <param name="degrees">An angle in degrees</param>
		/// <returns>The angle expressed in radians</returns>
		public static float DegreesToRadians(float degrees)
		{
			const float degToRad = (float)System.Math.PI / 180.0f;
			return degrees * degToRad;
		}

		/// <summary>
		/// Convert radians to degrees
		/// </summary>
		/// <param name="radians">An angle in radians</param>
		/// <returns>The angle expressed in degrees</returns>
		public static float RadiansToDegrees(float radians)
		{
			const float radToDeg = 180.0f / (float)System.Math.PI;
			return radians * radToDeg;
		}

		/// <summary>
		/// Convert degrees to radians
		/// </summary>
		/// <param name="degrees">An angle in degrees</param>
		/// <returns>The angle expressed in radians</returns>
		public static double DegreesToRadians(double degrees)
		{
			const double degToRad = System.Math.PI / 180.0;
			return degrees * degToRad;
		}

		/// <summary>
		/// Convert radians to degrees
		/// </summary>
		/// <param name="radians">An angle in radians</param>
		/// <returns>The angle expressed in degrees</returns>
		public static double RadiansToDegrees(double radians)
		{
			const double radToDeg = 180.0 / System.Math.PI;
			return radians * radToDeg;
		}

		#endregion

		#region Swap

		/// <summary>
		/// Swaps two double values.
		/// </summary>
		/// <param name="a">The first value.</param>
		/// <param name="b">The second value.</param>
		public static void Swap(ref double a, ref double b)
		{
			double temp = a;
			a = b;
			b = temp;
		}

		/// <summary>
		/// Swaps two float values.
		/// </summary>
		/// <param name="a">The first value.</param>
		/// <param name="b">The second value.</param>
		public static void Swap(ref float a, ref float b)
		{
			float temp = a;
			a = b;
			b = temp;
		}

		#endregion

		#endregion

		/// <summary>
		/// Clamp a float to a range.
		/// </summary>
		public static float Clamp(float value, float min, float max)
		{
			if (value < min) return min;
			if (value > max) return max;
			return value;
		}

		/// <summary>
		/// Clamp an integer to a range.
		/// </summary>
		public static int Clamp(int value, int min, int max)
		{
			if (value < min) return min;
			if (value > max) return max;
			return value;
		}

		public static float Lerp(float lhs, float rhs, float t)
		{
			return lhs * (1.0f - t) + rhs * t;
		}

        /// Return a random float between 0.0 (inclusive) and 1.0 (exclusive.)
        public static float Random() { return Rand() / 32768.0f; }
        /// Return a random float between 0.0 and range, inclusive from both ends.
        public static float Random(float range) { return Rand() * range / 32767.0f; }
        /// Return a random float between min and max, inclusive from both ends.
        public static float Random(float min, float max) { return Rand() * (max - min) / 32767.0f + min; }
        /// Return a random integer between 0 and range - 1.
        public static int Random(int range) { return (int)(Random() * range); }
        /// Return a random integer between min and max - 1.
        public static int Random(int min, int max) { float range = (float)(max - min); return (int)(Random() * range) + min; }
        /// Return a random normal distributed number with the given mean value and variance.
        public static float RandomNormal(float meanValue, float variance) { return RandStandardNormal() * (float)Math.Sqrt(variance) + meanValue; }

        public static int randomSeed { get; set; } = 1;

        public static int Rand()
        {
            randomSeed = randomSeed * 214013 + 2531011;
            return (randomSeed >> 16) & 32767;
        }

        public static float RandStandardNormal()
        {
            float val = 0.0f;
            for (int i = 0; i < 12; i++)
                val += Rand() / 32768.0f;
            val -= 6.0f;

            // Now val is approximatly standard normal distributed
            return val;
        }
    }
}
