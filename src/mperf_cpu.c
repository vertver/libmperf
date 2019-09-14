/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : CPU module impl.
*********************************************************/
#include "mperf_header.h"

mp_retcode
MPERF_API
mpGetCpuDescription(
	MPERF_CONTEXT* pContext,
	mp_utf8* pCpuString,
	mp_i32 StringSize
)
{
	return mpGetCPUDescription(pContext, pCpuString, StringSize);
}

mp_retcode
MPERF_API
mpGetProcessorsCount(
	MPERF_CONTEXT* pContext,
	mp_i32* pProcessorsCount
)
{
	return mpGetCPUCount(pContext, pProcessorsCount);
}

mp_retcode
MPERF_API
mpGetProcessorsLoadPerCore(
	MPERF_CONTEXT* pContext,
	mp_f32** pFloatLoad
)
{
	return mpGetCPULoadPerCore(pContext, pFloatLoad);
}

mp_retcode
MPERF_API
mpGetProcessorsLoad(
	MPERF_CONTEXT* pContext,
	mp_f32* pFloatLoad
)
{
	return mpGetCPULoad(pContext, pFloatLoad);
}
