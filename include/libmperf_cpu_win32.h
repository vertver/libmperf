#pragma once

mp_retcode
mpGetCpuDescriptionWindows(
	MPERF_CONTEXT* pContext,
	mp_utf8* pCpuString,
	mp_i32 StringSize
);

mp_retcode
mpGetProcessorsCountWindows(
	MPERF_CONTEXT* pContext,
	mp_i32* pProcessorsCount
);

mp_retcode
mpGetProcessorsLoadPerCoreWindows(
	MPERF_CONTEXT* pContext,
	mp_f32** pFloatLoad
);

mp_retcode
mpGetProcessorsLoadWindows(
	MPERF_CONTEXT* pContext,
	mp_f32* pFloatLoad
);
