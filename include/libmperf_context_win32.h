/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Context impl.
*********************************************************/
#pragma once
#include "libmperf_types.h"

mp_retcode
mpInitializeWindows(
	MPERF_CONTEXT* pContext
);

mp_retcode
mpUnitializeWindows(
	MPERF_CONTEXT* pContext
);
