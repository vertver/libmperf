/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Initializes impl.
*********************************************************/
#include "mperf_header.h"

mp_retcode
mpInitializeWindows(
	MPERF_CONTEXT* pContext
)
{
	HRESULT hr = 0;
	mp_retcode ret = 0;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	ret = MPERF_SUCCESSES;
	return ret;
}

mp_retcode
mpUnitializeWindows(
	MPERF_CONTEXT* pContext
)
{
	CoUninitialize();
	return MPERF_SUCCESSES;
}
