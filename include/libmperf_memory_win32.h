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
mpGetAvailableMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetLoadedMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetLoadedMemoryByProcessWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);


mp_retcode
MPERF_API
mpGetAvailableSwapMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetLoadedSwapMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
MPERF_API
mpGetLoadedSwapMemoryByProcessWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);
