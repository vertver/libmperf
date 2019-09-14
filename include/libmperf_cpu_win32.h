#pragma once

mp_retcode
MPERF_API
mpGetCpuDescriptionWindows(
	MPERF_CONTEXT* pContext,
	mp_utf8* pCpuString,
	mp_i32 StringSize
);

mp_retcode
MPERF_API
mpGetProcessorsCountWindows(
	MPERF_CONTEXT* pContext,
	mp_i32* pProcessorsCount
);

mp_retcode
MPERF_API
mpGetProcessorsLoadPerCoreWindows(
	MPERF_CONTEXT* pContext,
	mp_f32** pFloatLoad
);

mp_retcode
MPERF_API
mpGetProcessorsLoadWindows(
	MPERF_CONTEXT* pContext,
	mp_f32* pFloatLoad
);
