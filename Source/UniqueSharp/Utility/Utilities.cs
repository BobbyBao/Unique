using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Runtime.CompilerServices;

namespace Unique.Engine
{
    public static unsafe class Utilities
    {
        /// <summary>
        /// Pins the specified source and call an action with the pinned pointer.
        /// </summary>
        /// <typeparam name="T">The type of the structure to pin</typeparam>
        /// <param name="source">The source.</param>
        /// <param name="pinAction">The pin action to perform on the pinned pointer.</param>
        public static void Pin<T>(ref T source, Action<IntPtr> pinAction) where T : struct
        {
            unsafe
            {
                pinAction((IntPtr)Unsafe.AsPointer(ref source));
            }
        }

        /// <summary>
        /// Pins the specified source and call an action with the pinned pointer.
        /// </summary>
        /// <typeparam name="T">The type of the structure to pin</typeparam>
        /// <param name="source">The source array.</param>
        /// <param name="pinAction">The pin action to perform on the pinned pointer.</param>
        public static void Pin<T>(T[] source, Action<IntPtr> pinAction) where T : struct
        {
            unsafe
            {
                pinAction(source == null ? IntPtr.Zero : (IntPtr)Unsafe.AsPointer(ref source));
            }
        }
        /*
        /// <summary>
        /// Covnerts a structured array to an equivalent byte array.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="source">The source.</param>
        /// <returns></returns>
        public static byte[] ToByteArray<T>(T[] source) where T : struct
        {
            if(source == null) return null;

            var buffer = new byte[SizeOf<T>() * source.Length];

            if(source.Length == 0)
                return buffer;

            unsafe
            {
                fixed (void* pBuffer = buffer)
                    Unsafe.CopyBlockUnaligned(pBuffer, source, (uint)source.Length);
            }
            return buffer;
        }*/

        /// <summary>
        /// Reads the specified T data from a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to read</typeparam>
        /// <param name="source">Memory location to read from.</param>
        /// <returns>The data read from the memory location</returns>
        public static T Read<T>(IntPtr source) where T : struct
        {
            unsafe
            {
                return Unsafe.ReadUnaligned<T>((void*)source);
            }
        }

        /// <summary>
        /// Reads the specified T data from a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to read</typeparam>
        /// <param name="source">Memory location to read from.</param>
        /// <param name="data">The data write to.</param>
        /// <returns>source pointer + sizeof(T)</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Read<T>(IntPtr source, ref T data) where T : struct
        {
            unsafe
            {
                Unsafe.Copy(ref data, (void*)source);
            }
        }
        /*
        /// <summary>
        /// Reads the specified T data from a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to read</typeparam>
        /// <param name="source">Memory location to read from.</param>
        /// <param name="data">The data write to.</param>
        /// <returns>source pointer + sizeof(T)</returns>
        public static void ReadOut<T>(IntPtr source, out T data) where T : struct
        {
            unsafe
            {
                Unsafe.Copy<T>(out data, (void*)source);
            }
        }*/
        /*
        /// <summary>
        /// Reads the specified T data from a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to read</typeparam>
        /// <param name="source">Memory location to read from.</param>
        /// <param name="data">The data write to.</param>
        /// <returns>source pointer + sizeof(T)</returns>
        public static IntPtr ReadAndPosition<T>(IntPtr source, ref T data) where T : struct
        {
            unsafe
            {
                return (IntPtr)Unsafe.Copy((void*)source, ref data);
            }
        }*/

        /// <summary>
        /// Reads the specified array T[] data from a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to read</typeparam>
        /// <param name="source">Memory location to read from.</param>
        /// <param name="data">The data write to.</param>
        /// <param name="offset">The offset in the array to write to.</param>
        /// <param name="count">The number of T element to read from the memory location</param>
        /// <returns>source pointer + sizeof(T) * count</returns>
        public static IntPtr Read<T>(IntPtr source, T[] data, int offset, int count) where T : struct
        {
            unsafe
            {
                unsafe
                {
                    uint byteCount = (uint)(count * Unsafe.SizeOf<T>());
                    void* dest = Unsafe.AsPointer(ref data[offset]);
                    Unsafe.CopyBlockUnaligned(dest, (void*)source, byteCount);
                    return (IntPtr)((byte*)source + byteCount);
                }
                
            }
        }

        /// <summary>
        /// Writes the specified T data to a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to write</typeparam>
        /// <param name="destination">Memory location to write to.</param>
        /// <param name="data">The data to write.</param>
        /// <returns>destination pointer + sizeof(T)</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Write<T>(IntPtr destination, ref T data) where T : struct
        {
            unsafe
            {
                Unsafe.Copy((void*)destination, ref data);
            }
        }
        /*
        /// <summary>
        /// Writes the specified T data to a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to write</typeparam>
        /// <param name="destination">Memory location to write to.</param>
        /// <param name="data">The data to write.</param>
        /// <returns>destination pointer + sizeof(T)</returns>
        public static IntPtr WriteAndPosition<T>(IntPtr destination, ref T data) where T : struct
        {
            unsafe
            {
                return (IntPtr)Interop.Write<T>((void*)destination, ref data);
            }
        }*/

        /// <summary>
        /// Writes the specified array T[] data to a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to write</typeparam>
        /// <param name="destination">Memory location to write to.</param>
        /// <param name="data">The array of T data to write.</param>
        /// <param name="offset">The offset in the array to read from.</param>
        /// <param name="count">The number of T element to write to the memory location</param>
        /// <returns>destination pointer + sizeof(T) * count</returns>
        public static void Write<T>(byte[] destination, T[] data, int offset, int count) where T : struct
        {
            unsafe
            {
                fixed (void* pDest = destination)
                {
                    Write((IntPtr)pDest, data, offset, count);
                }
            }
        }

        /// <summary>
        /// Writes the specified array T[] data to a memory location.
        /// </summary>
        /// <typeparam name="T">Type of a data to write</typeparam>
        /// <param name="destination">Memory location to write to.</param>
        /// <param name="data">The array of T data to write.</param>
        /// <param name="offset">The offset in the array to read from.</param>
        /// <param name="count">The number of T element to write to the memory location</param>
        /// <returns>destination pointer + sizeof(T) * count</returns>
        public static IntPtr Write<T>(IntPtr destination, T[] data, int offset, int count) where T : struct
        {
            unsafe
            {
                uint byteCount = (uint)(count * Unsafe.SizeOf<T>());
                void* src = Unsafe. AsPointer(ref data[offset]);
                Unsafe.CopyBlockUnaligned((void*)destination, src, byteCount);
                return (IntPtr)((byte*)destination + byteCount);
            }
        }

        /// <summary>
        /// Native memcpy.
        /// </summary>
        /// <param name="dest">The destination memory location</param>
        /// <param name="src">The source memory location.</param>
        /// <param name="sizeInBytesToCopy">The count.</param>
        /// <returns></returns>
        public static void CopyMemory(IntPtr dest, IntPtr src, int sizeInBytesToCopy)
        {
            Unsafe.CopyBlockUnaligned((void*)dest, (void*)src, (uint)sizeInBytesToCopy);
        }

        public static void ClearMemory(IntPtr dest, byte value, int sizeInBytesToClear)
        {
            Unsafe.InitBlockUnaligned((void*)dest, value, (uint)sizeInBytesToClear);
        }
        
        public static int SizeOf<T>(T[] array) where T : struct
        {
            return array == null ? 0 : array.Length * Unsafe.SizeOf<T>();
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static int SizeOf<T>()
        {
            return Unsafe.SizeOf<T>();
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static ref T As<T>(IntPtr ptr)
        {
            return ref Unsafe.AsRef<T>((void*)ptr);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static IntPtr As<T>(ref T value)
        {
            return (IntPtr)Unsafe.AsPointer(ref value);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public unsafe static IntPtr Alloc<T>()
        {
            return AllocateAndClear(Unsafe.SizeOf<T>());
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public unsafe static IntPtr AllocArray<T>(int size)
        {
            return AllocateAndClear(Unsafe.SizeOf<T>()* size);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public unsafe static IntPtr Allocate(int sizeInBytes)
        {
            return Marshal.AllocHGlobal(sizeInBytes);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static IntPtr AllocateAndClear(int sizeInBytes, byte clearValue = 0)
        {
            var ptr = Allocate(sizeInBytes);
            ClearMemory(ptr, clearValue, sizeInBytes);
            return ptr;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static IntPtr Resize<T>(IntPtr oldPointer, int newElementCount) where T : struct
        {
            return (Marshal.ReAllocHGlobal(oldPointer,
                new IntPtr(Marshal.SizeOf(typeof(T)) * newElementCount)));
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static ref T At<T>(IntPtr ptr, int index)
        {
            return ref Unsafe.Add(ref Unsafe.AsRef<T>((void*)ptr), index);            
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public unsafe static void Free(IntPtr buffer)
        {
            Marshal.FreeHGlobal(buffer);
        }
        
    }
}
