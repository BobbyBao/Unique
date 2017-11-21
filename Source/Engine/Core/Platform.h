/*
 * Copyright 2010-2017 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx#license-bsd-2-clause
 */

#ifndef UNIQUE_PLATFORM_H_HEADER_GUARD
#define UNIQUE_PLATFORM_H_HEADER_GUARD

// Architecture
#define UNIQUE_ARCH_32BIT 0
#define UNIQUE_ARCH_64BIT 0

// Compiler
#define UNIQUE_COMPILER_CLANG           0
#define UNIQUE_COMPILER_CLANG_ANALYZER  0
#define UNIQUE_COMPILER_GCC             0
#define UNIQUE_COMPILER_MSVC            0

// Endianess
#define UNIQUE_CPU_ENDIAN_BIG    0
#define UNIQUE_CPU_ENDIAN_LITTLE 0

// CPU
#define UNIQUE_CPU_ARM   0
#define UNIQUE_CPU_JIT   0
#define UNIQUE_CPU_MIPS  0
#define UNIQUE_CPU_PPC   0
#define UNIQUE_CPU_RISCV 0
#define UNIQUE_CPU_X86   0

// C Runtime
#define UNIQUE_CRT_BIONIC 0
#define UNIQUE_CRT_GLIBC  0
#define UNIQUE_CRT_LIBCXX 0
#define UNIQUE_CRT_MINGW  0
#define UNIQUE_CRT_MSVC   0
#define UNIQUE_CRT_NEWLIB 0

#ifndef UNIQUE_CRT_MUSL
#	define UNIQUE_CRT_MUSL 0
#endif // UNIQUE_CRT_MUSL

#ifndef UNIQUE_CRT_NONE
#	define UNIQUE_CRT_NONE 0
#endif // UNIQUE_CRT_NONE

// Platform
#define UNIQUE_PLATFORM_ANDROID    0
#define UNIQUE_PLATFORM_EMSCRIPTEN 0
#define UNIQUE_PLATFORM_BSD        0
#define UNIQUE_PLATFORM_HURD       0
#define UNIQUE_PLATFORM_IOS        0
#define UNIQUE_PLATFORM_LINUX      0
#define UNIQUE_PLATFORM_OSX        0
#define UNIQUE_PLATFORM_PS4        0
#define UNIQUE_PLATFORM_QNX        0
#define UNIQUE_PLATFORM_RPI        0
#define UNIQUE_PLATFORM_STEAMLINK  0
#define UNIQUE_PLATFORM_WINDOWS    0
#define UNIQUE_PLATFORM_WINRT      0
#define UNIQUE_PLATFORM_XBOXONE    0

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(__clang__)
// clang defines __GNUC__ or _MSC_VER
#	undef  UNIQUE_COMPILER_CLANG
#	define UNIQUE_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#	if defined(__clang_analyzer__)
#		undef  UNIQUE_COMPILER_CLANG_ANALYZER
#		define UNIQUE_COMPILER_CLANG_ANALYZER 1
#	endif // defined(__clang_analyzer__)
#elif defined(_MSC_VER)
#	undef  UNIQUE_COMPILER_MSVC
#	define UNIQUE_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#	undef  UNIQUE_COMPILER_GCC
#	define UNIQUE_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#	error "UNIQUE_COMPILER_* is not defined!"
#endif //

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
#if defined(__arm__)     || \
	defined(__aarch64__) || \
	defined(_M_ARM)
#	undef  UNIQUE_CPU_ARM
#	define UNIQUE_CPU_ARM 1
#	define UNIQUE_CACHE_LINE_SIZE 64
#elif defined(__MIPSEL__)     || \
	  defined(__mips_isa_rev) || \
	  defined(__mips64)
#	undef  UNIQUE_CPU_MIPS
#	define UNIQUE_CPU_MIPS 1
#	define UNIQUE_CACHE_LINE_SIZE 64
#elif defined(_M_PPC)        || \
	  defined(__powerpc__)   || \
	  defined(__powerpc64__)
#	undef  UNIQUE_CPU_PPC
#	define UNIQUE_CPU_PPC 1
#	define UNIQUE_CACHE_LINE_SIZE 128
#elif defined(__riscv)   || \
	  defined(__riscv__) || \
	  defined(RISCVEL)
#	undef  UNIQUE_CPU_RISCV
#	define UNIQUE_CPU_RISCV 1
#	define UNIQUE_CACHE_LINE_SIZE 64
#elif defined(_M_IX86)    || \
	  defined(_M_X64)     || \
	  defined(__i386__)   || \
	  defined(__x86_64__)
#	undef  UNIQUE_CPU_X86
#	define UNIQUE_CPU_X86 1
#	define UNIQUE_CACHE_LINE_SIZE 64
#else // PNaCl doesn't have CPU defined.
#	undef  UNIQUE_CPU_JIT
#	define UNIQUE_CPU_JIT 1
#	define UNIQUE_CACHE_LINE_SIZE 64
#endif //

#if defined(__x86_64__)    || \
	defined(_M_X64)        || \
	defined(__aarch64__)   || \
	defined(__64BIT__)     || \
	defined(__mips64)      || \
	defined(__powerpc64__) || \
	defined(__ppc64__)     || \
	defined(__LP64__)
#	undef  UNIQUE_ARCH_64BIT
#	define UNIQUE_ARCH_64BIT 64
#else
#	undef  UNIQUE_ARCH_32BIT
#	define UNIQUE_ARCH_32BIT 32
#endif //

#if UNIQUE_CPU_PPC
#	undef  UNIQUE_CPU_ENDIAN_BIG
#	define UNIQUE_CPU_ENDIAN_BIG 1
#else
#	undef  UNIQUE_CPU_ENDIAN_LITTLE
#	define UNIQUE_CPU_ENDIAN_LITTLE 1
#endif // UNIQUE_PLATFORM_

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Operating_Systems
#if defined(_DURANGO) || defined(_XBOX_ONE)
#	undef  UNIQUE_PLATFORM_XBOXONE
#	define UNIQUE_PLATFORM_XBOXONE 1
#elif defined(_WIN32) || defined(_WIN64)
// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif // NOMINMAX
//  If _USING_V110_SDK71_ is defined it means we are using the v110_xp or v120_xp toolset.
#	if defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#		include <winapifamily.h>
#	endif // defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#	if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#		undef  UNIQUE_PLATFORM_WINDOWS
#		if !defined(WINVER) && !defined(_WIN32_WINNT)
#			if UNIQUE_ARCH_64BIT
//				When building 64-bit target Win7 and above.
#				define WINVER 0x0601
#				define _WIN32_WINNT 0x0601
#			else
//				Windows Server 2003 with SP1, Windows XP with SP2 and above
#				define WINVER 0x0502
#				define _WIN32_WINNT 0x0502
#			endif // UNIQUE_ARCH_64BIT
#		endif // !defined(WINVER) && !defined(_WIN32_WINNT)
#		define UNIQUE_PLATFORM_WINDOWS _WIN32_WINNT
#	else
#		undef  UNIQUE_PLATFORM_WINRT
#		define UNIQUE_PLATFORM_WINRT 1
#	endif
#elif defined(__ANDROID__)
// Android compiler defines __linux__
#	include <sys/cdefs.h> // Defines __BIONIC__ and includes android/api-level.h
#	undef  UNIQUE_PLATFORM_ANDROID
#	define UNIQUE_PLATFORM_ANDROID __ANDROID_API__
#elif defined(__STEAMLINK__)
// SteamLink compiler defines __linux__
#	undef  UNIQUE_PLATFORM_STEAMLINK
#	define UNIQUE_PLATFORM_STEAMLINK 1
#elif defined(__VCCOREVER__)
// RaspberryPi compiler defines __linux__
#	undef  UNIQUE_PLATFORM_RPI
#	define UNIQUE_PLATFORM_RPI 1
#elif  defined(__linux__)
#	undef  UNIQUE_PLATFORM_LINUX
#	define UNIQUE_PLATFORM_LINUX 1
#elif  defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) \
	|| defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
#	undef  UNIQUE_PLATFORM_IOS
#	define UNIQUE_PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#	undef  UNIQUE_PLATFORM_OSX
#	define UNIQUE_PLATFORM_OSX __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#elif defined(__EMSCRIPTEN__)
#	undef  UNIQUE_PLATFORM_EMSCRIPTEN
#	define UNIQUE_PLATFORM_EMSCRIPTEN 1
#elif defined(__ORBIS__)
#	undef  UNIQUE_PLATFORM_PS4
#	define UNIQUE_PLATFORM_PS4 1
#elif defined(__QNX__)
#	undef  UNIQUE_PLATFORM_QNX
#	define UNIQUE_PLATFORM_QNX 1
#elif  defined(__FreeBSD__)        \
	|| defined(__FreeBSD_kernel__) \
	|| defined(__NetBSD__)         \
	|| defined(__OpenBSD__)        \
	|| defined(__DragonFly__)
#	undef  UNIQUE_PLATFORM_BSD
#	define UNIQUE_PLATFORM_BSD 1
#elif defined(__GNU__)
#	undef  UNIQUE_PLATFORM_HURD
#	define UNIQUE_PLATFORM_HURD 1
#endif //

#if !UNIQUE_CRT_NONE
// https://sourceforge.net/p/predef/wiki/Libraries/
#	if defined(__BIONIC__)
#		undef  UNIQUE_CRT_BIONIC
#		define UNIQUE_CRT_BIONIC 1
#	elif defined(_MSC_VER)
#		undef  UNIQUE_CRT_MSVC
#		define UNIQUE_CRT_MSVC 1
#	elif defined(__GLIBC__)
#		undef  UNIQUE_CRT_GLIBC
#		define UNIQUE_CRT_GLIBC (__GLIBC__ * 10000 + __GLIBC_MINOR__ * 100)
#	elif defined(__MINGW32__) || defined(__MINGW64__)
#		undef  UNIQUE_CRT_MINGW
#		define UNIQUE_CRT_MINGW 1
#	elif defined(__apple_build_version__) || defined(__ORBIS__) || defined(__EMSCRIPTEN__) || defined(__llvm__)
#		undef  UNIQUE_CRT_LIBCXX
#		define UNIQUE_CRT_LIBCXX 1
#	endif //

#	if !UNIQUE_CRT_BIONIC \
	&& !UNIQUE_CRT_GLIBC  \
	&& !UNIQUE_CRT_LIBCXX \
	&& !UNIQUE_CRT_MINGW  \
	&& !UNIQUE_CRT_MSVC   \
	&& !UNIQUE_CRT_MUSL   \
	&& !UNIQUE_CRT_NEWLIB
#		undef  UNIQUE_CRT_NONE
#		define UNIQUE_CRT_NONE 1
#	endif // UNIQUE_CRT_*
#endif // !UNIQUE_CRT_NONE

#define UNIQUE_PLATFORM_POSIX (0      \
		|| UNIQUE_PLATFORM_ANDROID    \
		|| UNIQUE_PLATFORM_EMSCRIPTEN \
		|| UNIQUE_PLATFORM_BSD        \
		|| UNIQUE_PLATFORM_HURD       \
		|| UNIQUE_PLATFORM_IOS        \
		|| UNIQUE_PLATFORM_LINUX      \
		|| UNIQUE_PLATFORM_OSX        \
		|| UNIQUE_PLATFORM_QNX        \
		|| UNIQUE_PLATFORM_STEAMLINK  \
		|| UNIQUE_PLATFORM_PS4        \
		|| UNIQUE_PLATFORM_RPI        \
		)

#define UNIQUE_PLATFORM_NONE !(0      \
		|| UNIQUE_PLATFORM_ANDROID    \
		|| UNIQUE_PLATFORM_EMSCRIPTEN \
		|| UNIQUE_PLATFORM_BSD        \
		|| UNIQUE_PLATFORM_HURD       \
		|| UNIQUE_PLATFORM_IOS        \
		|| UNIQUE_PLATFORM_LINUX      \
		|| UNIQUE_PLATFORM_OSX        \
		|| UNIQUE_PLATFORM_PS4        \
		|| UNIQUE_PLATFORM_QNX        \
		|| UNIQUE_PLATFORM_RPI        \
		|| UNIQUE_PLATFORM_STEAMLINK  \
		|| UNIQUE_PLATFORM_WINDOWS    \
		|| UNIQUE_PLATFORM_WINRT      \
		|| UNIQUE_PLATFORM_XBOXONE    \
		)

#if UNIQUE_COMPILER_GCC
#	define UNIQUE_COMPILER_NAME "GCC " \
				UNIQUE_STRINGIZE(__GNUC__) "." \
				UNIQUE_STRINGIZE(__GNUC_MINOR__) "." \
				UNIQUE_STRINGIZE(__GNUC_PATCHLEVEL__)
#elif UNIQUE_COMPILER_CLANG
#	define UNIQUE_COMPILER_NAME "Clang " \
				UNIQUE_STRINGIZE(__clang_major__) "." \
				UNIQUE_STRINGIZE(__clang_minor__) "." \
				UNIQUE_STRINGIZE(__clang_patchlevel__)
#elif UNIQUE_COMPILER_MSVC
#	if UNIQUE_COMPILER_MSVC >= 1910 // Visual Studio 2017
#		define UNIQUE_COMPILER_NAME "MSVC 15.0"
#	elif UNIQUE_COMPILER_MSVC >= 1900 // Visual Studio 2015
#		define UNIQUE_COMPILER_NAME "MSVC 14.0"
#	elif UNIQUE_COMPILER_MSVC >= 1800 // Visual Studio 2013
#		define UNIQUE_COMPILER_NAME "MSVC 12.0"
#	elif UNIQUE_COMPILER_MSVC >= 1700 // Visual Studio 2012
#		define UNIQUE_COMPILER_NAME "MSVC 11.0"
#	elif UNIQUE_COMPILER_MSVC >= 1600 // Visual Studio 2010
#		define UNIQUE_COMPILER_NAME "MSVC 10.0"
#	elif UNIQUE_COMPILER_MSVC >= 1500 // Visual Studio 2008
#		define UNIQUE_COMPILER_NAME "MSVC 9.0"
#	else
#		define UNIQUE_COMPILER_NAME "MSVC"
#	endif //
#endif // UNIQUE_COMPILER_

#if UNIQUE_PLATFORM_ANDROID
#	define UNIQUE_PLATFORM_NAME "Android " \
				UNIQUE_STRINGIZE(UNIQUE_PLATFORM_ANDROID)
#elif UNIQUE_PLATFORM_EMSCRIPTEN
#	define UNIQUE_PLATFORM_NAME "asm.js " \
				UNIQUE_STRINGIZE(__EMSCRIPTEN_major__) "." \
				UNIQUE_STRINGIZE(__EMSCRIPTEN_minor__) "." \
				UNIQUE_STRINGIZE(__EMSCRIPTEN_tiny__)
#elif UNIQUE_PLATFORM_BSD
#	define UNIQUE_PLATFORM_NAME "BSD"
#elif UNIQUE_PLATFORM_HURD
#	define UNIQUE_PLATFORM_NAME "Hurd"
#elif UNIQUE_PLATFORM_IOS
#	define UNIQUE_PLATFORM_NAME "iOS"
#elif UNIQUE_PLATFORM_LINUX
#	define UNIQUE_PLATFORM_NAME "Linux"
#elif UNIQUE_PLATFORM_OSX
#	define UNIQUE_PLATFORM_NAME "OSX"
#elif UNIQUE_PLATFORM_PS4
#	define UNIQUE_PLATFORM_NAME "PlayStation 4"
#elif UNIQUE_PLATFORM_QNX
#	define UNIQUE_PLATFORM_NAME "QNX"
#elif UNIQUE_PLATFORM_RPI
#	define UNIQUE_PLATFORM_NAME "RaspberryPi"
#elif UNIQUE_PLATFORM_STEAMLINK
#	define UNIQUE_PLATFORM_NAME "SteamLink"
#elif UNIQUE_PLATFORM_WINDOWS
#	define UNIQUE_PLATFORM_NAME "Windows"
#elif UNIQUE_PLATFORM_WINRT
#	define UNIQUE_PLATFORM_NAME "WinRT"
#elif UNIQUE_PLATFORM_XBOXONE
#	define UNIQUE_PLATFORM_NAME "Xbox One"
#elif UNIQUE_PLATFORM_NONE
#	define UNIQUE_PLATFORM_NAME "None"
#else
#	error "Unknown UNIQUE_PLATFORM!"
#endif // UNIQUE_PLATFORM_

#if UNIQUE_CPU_ARM
#	define UNIQUE_CPU_NAME "ARM"
#elif UNIQUE_CPU_JIT
#	define UNIQUE_CPU_NAME "JIT-VM"
#elif UNIQUE_CPU_MIPS
#	define UNIQUE_CPU_NAME "MIPS"
#elif UNIQUE_CPU_PPC
#	define UNIQUE_CPU_NAME "PowerPC"
#elif UNIQUE_CPU_RISCV
#	define UNIQUE_CPU_NAME "RISC-V"
#elif UNIQUE_CPU_X86
#	define UNIQUE_CPU_NAME "x86"
#endif // UNIQUE_CPU_

#if UNIQUE_CRT_BIONIC
#	define UNIQUE_CRT_NAME "Bionic libc"
#elif UNIQUE_CRT_GLIBC
#	define UNIQUE_CRT_NAME "GNU C Library"
#elif UNIQUE_CRT_MSVC
#	define UNIQUE_CRT_NAME "MSVC C Runtime"
#elif UNIQUE_CRT_MINGW
#	define UNIQUE_CRT_NAME "MinGW C Runtime"
#elif UNIQUE_CRT_LIBCXX
#	define UNIQUE_CRT_NAME "Clang C Library"
#elif UNIQUE_CRT_NEWLIB
#	define UNIQUE_CRT_NAME "Newlib"
#elif UNIQUE_CRT_MUSL
#	define UNIQUE_CRT_NAME "musl libc"
#elif UNIQUE_CRT_NONE
#	define UNIQUE_CRT_NAME "None"
#else
#	error "Unknown UNIQUE_CRT!"
#endif // UNIQUE_CRT_

#if UNIQUE_ARCH_32BIT
#	define UNIQUE_ARCH_NAME "32-bit"
#elif UNIQUE_ARCH_64BIT
#	define UNIQUE_ARCH_NAME "64-bit"
#endif // UNIQUE_ARCH_

#endif // UNIQUE_PLATFORM_H_HEADER_GUARD
