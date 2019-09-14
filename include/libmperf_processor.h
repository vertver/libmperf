/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : CPU module impl.
*********************************************************/
#pragma once

mp_retcode
MPERF_API
mpGetCpuDescription(
	MPERF_CONTEXT* pContext,
	mp_utf8* pCpuString,
	mp_i32 StringSize
);

mp_retcode
MPERF_API
mpGetProcessorsCount(
	MPERF_CONTEXT* pContext,
	mp_i32* pProcessorsCount
);

mp_retcode
MPERF_API
mpGetProcessorsLoadPerCore(
	MPERF_CONTEXT* pContext,
	mp_f32** pFloatLoad
);

mp_retcode
MPERF_API
mpGetProcessorsLoad(
	MPERF_CONTEXT* pContext,
	mp_f32* pFloatLoad
);
