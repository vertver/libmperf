/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Process module impl.
*********************************************************/
#pragma once

mp_retcode
MPERF_API
mpGetProcessesCount(
	MPERF_CONTEXT* pContext,
	mp_i32* pProcessesCount
);

mp_retcode
MPERF_API
mpGetCurrentProcessId(
	MPERF_CONTEXT* pContext,
	mp_u32* pPID
);
