/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Memory module impl.
*********************************************************/
#pragma once

mp_retcode
mpGetAvailableMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
mpGetLoadedMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
mpGetLoadedMemoryByProcessWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);


mp_retcode
mpGetAvailableSwapMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
mpGetLoadedSwapMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);

mp_retcode
mpGetLoadedSwapMemoryByProcessWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
);
