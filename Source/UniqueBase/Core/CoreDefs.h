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
#endif // BX_CHECK

#define UNIQUE_UNUSED(x) (void)(sizeof((x), 0))

template<typename T, typename U> constexpr size_t OffsetOf(U T::*member)
{
	return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

template <typename T, int count> int LengthOf(const T(&)[count])
{
	return count;
};
