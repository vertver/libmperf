#pragma once

mp_retcode
MPERF_API
mpGetDisksCount(
	MPERF_CONTEXT* pContext,
	mp_i32* pDisksCount
);

mp_retcode
MPERF_API
mpGetAvailableDiskMemory(
	MPERF_CONTEXT* pContext,
	mp_utf8* pDiskString,
	mp_i32 StringSize
);

mp_retcode
MPERF_API
mpGetLoadedDiskMemory(
	MPERF_CONTEXT* pContext,
	mp_utf8* pDiskString,
	mp_i32 StringSize
);

mp_retcode
MPERF_API
mpGetFreeDiskMemory(
	MPERF_CONTEXT* pContext,
	mp_utf8* pDiskString,
	mp_i32 StringSize
);
