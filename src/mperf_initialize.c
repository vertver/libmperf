/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Initialize impl.
*********************************************************/
#include "mperf_header.h"

mp_retcode
MPERF_API
mpInitialize(
	MPERF_CONTEXT** ppContext
)
{
	mp_retcode ret = 0;
	void* pOutContext = NULL;

	if (!ppContext) {
		SET_MPERF_ERROR(MPERF_ERROR_UNDEFINED, NULL);
		return MPERF_FAILED;
	}

	pOutContext = (MPERF_CONTEXT*)malloc(sizeof(MPERF_CONTEXT));
	if (!pOutContext) {
		SET_MPERF_ERROR(MPERF_ERROR_UNDEFINED, NULL);
		return MPERF_FAILED;
	}
	memset(pOutContext, 0, sizeof(MPERF_CONTEXT));

	ret = mpInitPlatform(pOutContext);
	if (MPFAILED(ret)) {
		SET_MPERF_ERROR(MPERF_ERROR_UNDEFINED, pOutContext);
		free(pOutContext);
		return ret;
	}

	*ppContext = pOutContext;

	return MPERF_SUCCESSES;
}

mp_retcode
MPERF_API
mpUnitialize(
	MPERF_CONTEXT* pContext
)
{
	mp_retcode ret = 0;

	if (!pContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		return MPERF_FAILED;
	}

	ret = mpDestroyPlatform(pContext);
	if (MPFAILED(ret)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		free(pContext);
		return ret;
	}

	if (pContext->pGpuMperfContext) {
		ret = mpUnitializeGraphics(pContext);
		if (MPFAILED(ret)) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			free(pContext);
			return ret;
		}
	}

	if (pContext) free(pContext);
	return ret;
}
