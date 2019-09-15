/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Memory module impl.
*********************************************************/
#include "mperf_header.h"
#include <psapi.h>

mp_retcode
mpGetAvailableMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
)
{
	mp_retcode ret = 0;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	if (!GlobalMemoryStatusEx(&statex)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	*pMemoryBytes = statex.ullAvailVirtual;
	return ret;
}

mp_retcode
mpGetLoadedMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
)
{
	mp_retcode ret = 0;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	if (!GlobalMemoryStatusEx(&statex)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	*pMemoryBytes = statex.ullTotalVirtual - statex.ullAvailVirtual;
	return ret;
}

mp_retcode
mpGetLoadedMemoryByProcessWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
)
{
	mp_retcode ret = 0;
	PROCESS_MEMORY_COUNTERS ProcessCounters;
	memset(&ProcessCounters, 0, sizeof(PROCESS_MEMORY_COUNTERS));

	if (!GetProcessMemoryInfo(GetCurrentProcess(), &ProcessCounters, sizeof(PROCESS_MEMORY_COUNTERS))) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	*pMemoryBytes = ProcessCounters.WorkingSetSize;
	return ret;
}

mp_retcode
mpGetAvailableSwapMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
)
{
	mp_retcode ret = 0;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	if (!GlobalMemoryStatusEx(&statex)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	*pMemoryBytes = statex.ullAvailPageFile;
	return ret;
}

mp_retcode
mpGetLoadedSwapMemoryWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
)
{
	mp_retcode ret = 0;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	if (!GlobalMemoryStatusEx(&statex)) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	*pMemoryBytes = statex.ullTotalPageFile - statex.ullAvailPageFile;
	return ret;
}

mp_retcode
mpGetLoadedSwapMemoryByProcessWindows(
	MPERF_CONTEXT* pContext,
	mp_u64* pMemoryBytes
)
{
	mp_retcode ret = 0;
	PROCESS_MEMORY_COUNTERS ProcessCounters;
	memset(&ProcessCounters, 0, sizeof(PROCESS_MEMORY_COUNTERS));

	if (!GetProcessMemoryInfo(GetCurrentProcess(), &ProcessCounters, sizeof(PROCESS_MEMORY_COUNTERS))) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	*pMemoryBytes = ProcessCounters.PagefileUsage;
	return ret;
}
