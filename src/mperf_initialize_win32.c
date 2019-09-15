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
	mp_retcode ret = 0;
	HRESULT hr = 0;
	HMODULE hModule = NULL;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	/*
		Get current module of this library
	*/
	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&hModule, &hModule)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pContext->pLibInstance = hModule;
	pContext->pInitedThreadHandle = GetCurrentThread();

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
