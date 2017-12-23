#pragma once
#include "../Unique.h"

#include "Platform.h"


#if UNIQUE_COMPILER_GCC || UNIQUE_COMPILER_CLANG
#	define UNIQUE_ALIGN_DECL(_align, _decl) _decl __attribute__( (aligned(_align) ) )
#	define UNIQUE_ALLOW_UNUSED __attribute__( (unused) )
#	define UNIQUE_FORCE_INLINE inline __attribute__( (__always_inline__) )
#	define UNIQUE_FUNCTION __PRETTY_FUNCTION__
#	define UNIQUE_LIKELY(_x)   __builtin_expect(!!(_x), 1)
#	define UNIQUE_UNLIKELY(_x) __builtin_expect(!!(_x), 0)
#	define UNIQUE_NO_INLINE   __attribute__( (noinline) )
#	define UNIQUE_NO_RETURN   __attribute__( (noreturn) )
#	if UNIQUE_COMPILER_GCC >= 70000
#		define UNIQUE_FALLTHROUGH __attribute__( (fallthrough) )
#	else
#		define UNIQUE_FALLTHROUGH UNIQUE_NOOP()
#	endif // UNIQUE_COMPILER_GCC >= 70000
#	define UNIQUE_NO_VTABLE
#	define UNIQUE_PRINTF_ARGS(_format, _args) __attribute__( (format(__printf__, _format, _args) ) )
#	if UNIQUE_CLANG_HAS_FEATURE(cxx_thread_local)
#		define UNIQUE_THREAD_LOCAL __thread
#	endif // UNIQUE_COMPILER_CLANG
#	if (!UNIQUE_PLATFORM_OSX && (UNIQUE_COMPILER_GCC >= 40200)) || (UNIQUE_COMPILER_GCC >= 40500)
#		define UNIQUE_THREAD_LOCAL __thread
#	endif // UNIQUE_COMPILER_GCC
#	define UNIQUE_ATTRIBUTE(_x) __attribute__( (_x) )
#	if UNIQUE_CRT_MSVC
#		define __stdcall
#	endif // UNIQUE_CRT_MSVC
#elif UNIQUE_COMPILER_MSVC
#	define UNIQUE_ALIGN_DECL(_align, _decl) __declspec(align(_align) ) _decl
#	define UNIQUE_ALLOW_UNUSED
#	define UNIQUE_FORCE_INLINE __forceinline
#	define UNIQUE_FUNCTION __FUNCTION__
#	define UNIQUE_LIKELY(_x)   (_x)
#	define UNIQUE_UNLIKELY(_x) (_x)
#	define UNIQUE_NO_INLINE __declspec(noinline)
#	define UNIQUE_NO_RETURN
#	define UNIQUE_FALLTHROUGH UNIQUE_NOOP()
#	define UNIQUE_NO_VTABLE __declspec(novtable)
#	define UNIQUE_PRINTF_ARGS(_format, _args)
#	define UNIQUE_THREAD_LOCAL __declspec(thread)
#	define UNIQUE_ATTRIBUTE(_x)
#else
#	error "Unknown UNIQUE_COMPILER_?"
#endif

#define UNIQUE_NOOP()

#ifndef UNIQUE_CHECK
#	define UNIQUE_CHECK(_condition, ...) UNIQUE_NOOP()
#endif // UNIQUE_CHECK

///
#if UNIQUE_COMPILER_MSVC
// Workaround MSVS bug...
#	define UNIQUE_VA_ARGS_PASS(...) UNIQUE_VA_ARGS_PASS_1_ __VA_ARGS__ UNIQUE_VA_ARGS_PASS_2_
#	define UNIQUE_VA_ARGS_PASS_1_ (
#	define UNIQUE_VA_ARGS_PASS_2_ )
#else
#	define UNIQUE_VA_ARGS_PASS(...) (__VA_ARGS__)
#endif // UNIQUE_COMPILER_MSVC

#define UNIQUE_VA_ARGS_COUNT(...) UNIQUE_VA_ARGS_COUNT_ UNIQUE_VA_ARGS_PASS(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define UNIQUE_VA_ARGS_COUNT_(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11, _a12, _a13, _a14, _a15, _a16, _last, ...) _last

///
#define UNIQUE_MACRO_DISPATCHER(_func, ...) UNIQUE_MACRO_DISPATCHER_1_(_func, UNIQUE_VA_ARGS_COUNT(__VA_ARGS__) )
#define UNIQUE_MACRO_DISPATCHER_1_(_func, _argCount) UNIQUE_MACRO_DISPATCHER_2_(_func, _argCount)
#define UNIQUE_MACRO_DISPATCHER_2_(_func, _argCount) UNIQUE_CONCATENATE(_func, _argCount)

///
#define UNIQUE_STRINGIZE(_x) UNIQUE_STRINGIZE_(_x)
#define UNIQUE_STRINGIZE_(_x) #_x

///
#define UNIQUE_CONCATENATE(_x, _y) UNIQUE_CONCATENATE_(_x, _y)
#define UNIQUE_CONCATENATE_(_x, _y) _x ## _y

///
#define UNIQUE_UNUSED_1(_a1) (void)(sizeof((_a1), 0))

#define UNIQUE_UNUSED_2(_a1, _a2) UNIQUE_UNUSED_1(_a1); UNIQUE_UNUSED_1(_a2)
#define UNIQUE_UNUSED_3(_a1, _a2, _a3) UNIQUE_UNUSED_2(_a1, _a2); UNIQUE_UNUSED_1(_a3)
#define UNIQUE_UNUSED_4(_a1, _a2, _a3, _a4) UNIQUE_UNUSED_3(_a1, _a2, _a3); UNIQUE_UNUSED_1(_a4)
#define UNIQUE_UNUSED_5(_a1, _a2, _a3, _a4, _a5) UNIQUE_UNUSED_4(_a1, _a2, _a3, _a4); UNIQUE_UNUSED_1(_a5)
#define UNIQUE_UNUSED_6(_a1, _a2, _a3, _a4, _a5, _a6) UNIQUE_UNUSED_5(_a1, _a2, _a3, _a4, _a5); UNIQUE_UNUSED_1(_a6)
#define UNIQUE_UNUSED_7(_a1, _a2, _a3, _a4, _a5, _a6, _a7) UNIQUE_UNUSED_6(_a1, _a2, _a3, _a4, _a5, _a6); UNIQUE_UNUSED_1(_a7)
#define UNIQUE_UNUSED_8(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8) UNIQUE_UNUSED_7(_a1, _a2, _a3, _a4, _a5, _a6, _a7); UNIQUE_UNUSED_1(_a8)
#define UNIQUE_UNUSED_9(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9) UNIQUE_UNUSED_8(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8); UNIQUE_UNUSED_1(_a9)
#define UNIQUE_UNUSED_10(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10) UNIQUE_UNUSED_9(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9); UNIQUE_UNUSED_1(_a10)
#define UNIQUE_UNUSED_11(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11) UNIQUE_UNUSED_10(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10); UNIQUE_UNUSED_1(_a11)
#define UNIQUE_UNUSED_12(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11, _a12) UNIQUE_UNUSED_11(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11); UNIQUE_UNUSED_1(_a12)

#if UNIQUE_COMPILER_MSVC
// Workaround MSVS bug...
#	define UNIQUE_UNUSED(...) UNIQUE_MACRO_DISPATCHER(UNIQUE_UNUSED_, __VA_ARGS__) UNIQUE_VA_ARGS_PASS(__VA_ARGS__)
#else
#	define UNIQUE_UNUSED(...) UNIQUE_MACRO_DISPATCHER(UNIQUE_UNUSED_, __VA_ARGS__)(__VA_ARGS__)
#endif // UNIQUE_COMPILER_MSVC

#include "Export.h"