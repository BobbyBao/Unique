#pragma once
#include <assert.h>

#ifndef UNIQUE_SHARED_LIB_BUILD
#    define UNIQUE_SHARED_LIB_BUILD 0
#endif // UNIQUE_SHARED_LIB_BUILD

#ifndef UNIQUE_SHARED_LIB_USE
#    define UNIQUE_SHARED_LIB_USE 0
#endif // UNIQUE_SHARED_LIB_USE

#if WIN32
#   define UNIQUE_SYMBOL_EXPORT __declspec(dllexport)
#   define UNIQUE_SYMBOL_IMPORT __declspec(dllimport)
#else
#   define UNIQUE_SYMBOL_EXPORT __attribute__((visibility("default")))
#   define UNIQUE_SYMBOL_IMPORT
#endif // BX_PLATFORM_WINDOWS

#if UNIQUE_SHARED_LIB_BUILD
#   define UNIQUE_API UNIQUE_SYMBOL_EXPORT
#elif UNIQUE_SHARED_LIB_USE
#   define UNIQUE_API UNIQUE_SYMBOL_IMPORT
#else
#   define UNIQUE_API
#endif // UNIQUE_SHARED_LIB_*

#if defined(__cplusplus)
#   define UNIQUE_C_API extern "C" UNIQUE_API
#else
#   define UNIQUE_C_API UNIQUE_API
#endif // defined(__cplusplus)


typedef unsigned uint;
typedef unsigned short ushort;
typedef unsigned char byte;

#define UNIQUE_LOGGING 1