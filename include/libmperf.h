/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Base header
*********************************************************/
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "libmperf_types.h"
#include "libmperf_platform.h"
#include "libmperf_processor.h"
#include "libmperf_memory.h"
#include "libmperf_graphics.h"
#include "libmperf_processes.h"

mp_retcode
MPERF_API
mpInitialize(
	MPERF_CONTEXT** ppContext
);

mp_retcode
MPERF_API
mpUnitialize(
	MPERF_CONTEXT* pContext
);

#ifdef __cplusplus
}
#endif
