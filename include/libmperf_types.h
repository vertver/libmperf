/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Base types
*********************************************************/
#pragma once

#include <stdlib.h>
#include <stdint.h>
#define MPERF_VERSION 0.1f

#ifdef DLL_PLATFORM
#ifdef LIB_EXPORTS
#define MPERF_API __declspec(dllexport)
#else
#define MPERF_API __declspec(dllimport)
#endif
#else
#define MPERF_API 
#endif

#ifdef WINDOWS_PLATFORM
#define IsInvalidHandle(x) (x == INVALID_HANDLE_VALUE || x == 0)
#endif

typedef char mp_i8;
typedef short mp_i16;
typedef long mp_i32;
typedef long long mp_i64;
typedef unsigned char mp_u8;
typedef unsigned short mp_u16;
typedef unsigned long mp_u32;
typedef unsigned long long mp_u64;

#ifdef X64_PLATFORM
typedef mp_u64 mp_size;
#else
typedef mp_u32 mp_size;
#endif

typedef char mp_utf8;

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#endif

#ifndef USE_SHORT_TYPE_FOR_UTF16
typedef wchar_t mp_utf16;
#else
typedef short mp_utf16;
#endif

typedef float mp_f32;
typedef double mp_f64;

typedef mp_i32 mp_retcode;
typedef mp_i32 mp_errcode;

typedef mp_utf8 mp_utf8string32[32];
typedef mp_utf8 mp_utf8string64[64];
typedef mp_utf8 mp_utf8string128[128];
typedef mp_utf8 mp_utf8string256[256];
typedef mp_utf8 mp_utf8string512[512];
typedef mp_utf8 mp_utf8string1024[1024];
typedef mp_utf8 mp_utf8string2048[2048];
typedef mp_utf8 mp_utf8string4096[4096];
typedef mp_utf8 mp_utf8string8192[8192];

typedef mp_utf16 mp_utf16string32[32];
typedef mp_utf16 mp_utf16string64[64];
typedef mp_utf16 mp_utf16string128[128];
typedef mp_utf16 mp_utf16string256[256];
typedef mp_utf16 mp_utf16string512[512];
typedef mp_utf16 mp_utf16string1024[1024];
typedef mp_utf16 mp_utf16string2048[2048];
typedef mp_utf16 mp_utf16string4096[4096];
typedef mp_utf16 mp_utf16string8192[8192];

typedef struct
{
	mp_errcode ErrorCode;
	mp_utf8string128 ErrorString;
} MPERF_ERROR_STRUCT;

typedef struct
{
	void* pLibInstance;
	void* pInitedThreadHandle;
	void* pGpuMperfContext;
	MPERF_ERROR_STRUCT ErrorStruct;
} MPERF_CONTEXT;

#define MPERF_FAILED -1
#define MPERF_SUCCESSES 0

#define MPERF_ERROR_UNDEFINED 0xFFFFFFFF

#define MPSUCCEEDED(x) (x != MPERF_FAILED)
#define MPFAILED(x) (!MPSUCCEEDED(x))
