/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Memory module impl.
*********************************************************/
#pragma once

mp_retcode
MPERF_API
mpGetAvailableMemory(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetLoadedMemory(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetLoadedMemoryByProcess(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetAvailableSwapMemory(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetLoadedSwapMemory(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetLoadedSwapMemoryByProcess(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);
