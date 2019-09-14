/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Process module impl.
*********************************************************/
#include "mperf_header.h"
#include <tlhelp32.h>

mp_retcode
MPERF_API
mpGetProcessesCountWindows(
	MPERF_CONTEXT* pContext,
	mp_i32* pProcessesCount
)
{
	mp_retcode ret = 0;
	mp_i32 ProcessesCount = 0;
	PROCESSENTRY32W ProcessInfo = { NULL };
	ProcessInfo.dwSize = sizeof(PROCESSENTRY32W);
	HANDLE hSnapshot = NULL;
	BOOL bSearch = FALSE;

	/*
		Try to check all processes by snapshot
	*/
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (IsInvalidHandle(hSnapshot)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	/*
		Firstly, we must open first process (in this case - System Process)
	*/
	bSearch = Process32FirstW(hSnapshot, &ProcessInfo);
	if (!bSearch) {
		CloseHandle(hSnapshot);
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	/*
		1 success - 1 process. So easy and simple
	*/
	while (bSearch) {
		ProcessesCount++;
		bSearch = Process32NextW(hSnapshot, &ProcessInfo);
	}

	CloseHandle(hSnapshot);
	*pProcessesCount = ProcessesCount;
	return ret;
}

mp_retcode
MPERF_API
mpGetCurrentProcessIdWindows(
	MPERF_CONTEXT* pContext,
	mp_u32* pPID
)
{
	mp_retcode ret = 0;	
	mp_u32 dwProcessId = 0;
	
	if (!pPID) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	/*
		We can't be a system process
	*/
	dwProcessId = GetCurrentProcessId();
	if (!dwProcessId) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	*pPID = dwProcessId;
	return ret;
}

