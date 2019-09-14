/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : CPU module impl.
*********************************************************/
#include "mperf_header.h"
#include <math.h>
#include <intrin.h>

#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)
typedef LONG(WINAPI* NTQUERYSYSTEMINFORMATION)(UINT, PVOID, ULONG, PULONG);
NTQUERYSYSTEMINFORMATION pNtQuerySystemInformation = NULL;

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER IdleTime;
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER DpcTime;
	LARGE_INTEGER InterruptTime;
	ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, * PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

mp_retcode
MPERF_API
mpGetCpuDescriptionWindows(
	MPERF_CONTEXT* pContext,
	mp_utf8* pCpuString,
	mp_i32 StringSize
)
{
	mp_retcode ret = 0;
	mp_i32 CpuInfo[4] = { -1 };
	mp_utf8string64 szCPUBrandString;

	if (StringSize < 64) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	/*
		Only for x86-platform
	*/
	memset(szCPUBrandString, 0, sizeof(szCPUBrandString));
	__cpuid(CpuInfo, 0x80000002);
	memcpy(szCPUBrandString, CpuInfo, sizeof(CpuInfo));
	__cpuid(CpuInfo, 0x80000003);
	memcpy(szCPUBrandString + 16, CpuInfo, sizeof(CpuInfo));
	__cpuid(CpuInfo, 0x80000004);
	memcpy(szCPUBrandString + 32, CpuInfo, sizeof(CpuInfo));
	memcpy(pCpuString, szCPUBrandString, 64);

	return ret;
}

mp_retcode
MPERF_API
mpGetProcessorsCountWindows(
	MPERF_CONTEXT* pContext,
	mp_i32* pProcessorsCount
)
{
	mp_retcode ret = 0;
	static mp_i32 CpuCount = 0;
	SYSTEM_INFO sysInfo;

	if (!pProcessorsCount) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	if (!CpuCount) {
		memset(&sysInfo, 0, sizeof(SYSTEM_INFO));
		GetSystemInfo(&sysInfo);

		CpuCount = sysInfo.dwNumberOfProcessors;
	}

	*pProcessorsCount = CpuCount;
	return ret;
}

mp_retcode
MPERF_API
mpGetProcessorsLoadPerCoreWindows(
	MPERF_CONTEXT* pContext,
	mp_f32** pFloatLoad
)
{
	mp_retcode ret = 0;	
	mp_u64 TickCount = 0;
	static mp_u64 GlobalTicks = 0;
	static mp_i32 LocalCPUCount = 0;
	static mp_f32* pLoadArray = NULL;
	static PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION pCurrentUsages = NULL;
	static PLARGE_INTEGER pIdleTime = NULL;

	if (!pFloatLoad) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	if (!pNtQuerySystemInformation) {
		pNtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandleA("ntdll"), "NtQuerySystemInformation");
		if (!pNtQuerySystemInformation) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
	}

	if (!LocalCPUCount) {
		if (FAILED(mpGetProcessorsCountWindows(pContext, &LocalCPUCount))) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
	}

	if (!pCurrentUsages) {
		pCurrentUsages = (PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)malloc(sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * LocalCPUCount);
		if (!pCurrentUsages) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
	}

	if (!pIdleTime) {
		pIdleTime = (PLARGE_INTEGER)malloc(sizeof(LARGE_INTEGER) * LocalCPUCount);
		if (!pIdleTime) {
			free(pCurrentUsages);
			pCurrentUsages = NULL;
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
	}

	if (!pLoadArray) {
		pLoadArray = (mp_f32*)malloc(sizeof(mp_f32) * LocalCPUCount);
		if (!pLoadArray) {
			free(pIdleTime);
			free(pCurrentUsages);
			pIdleTime = NULL;
			pCurrentUsages = NULL;
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
	}

	TickCount = GetTickCount64();
	if (!GlobalTicks) GlobalTicks = TickCount;
	GlobalTicks++;		

	if (NT_ERROR(pNtQuerySystemInformation(8, pCurrentUsages, sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * (ULONG)LocalCPUCount, NULL))) {
		free(pLoadArray);
		free(pIdleTime);
		free(pCurrentUsages);
		pLoadArray = NULL;
		pIdleTime = NULL;
		pCurrentUsages = NULL;
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	for (mp_i32 i = 0; i < LocalCPUCount; i++) {
		PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION pCpuPerformanceInfo = &pCurrentUsages[i];
		pLoadArray[i] = (mp_f32)(100.0f - 0.01f * (mp_f32)(pCpuPerformanceInfo->IdleTime.QuadPart - pIdleTime[i].QuadPart) / (mp_f32)(TickCount - GlobalTicks));
		memcpy(&pIdleTime[i], &pCpuPerformanceInfo->IdleTime, sizeof(LARGE_INTEGER));
	}

	memcpy(*pFloatLoad, pLoadArray, sizeof(mp_f32) * LocalCPUCount);

	GlobalTicks = TickCount;
	++GlobalTicks;
}

mp_retcode
MPERF_API
mpGetProcessorsLoadWindows(
	MPERF_CONTEXT* pContext,
	mp_f32* pFloatLoad
)
{
	mp_retcode ret = 0;
	mp_f32 fCurrentLoad = 0.f;
	static mp_f32* pLoadArray = NULL;
	static mp_i32 LocalCPUCount = 0;

	if (!LocalCPUCount) {
		if (FAILED(mpGetProcessorsCountWindows(pContext, &LocalCPUCount))) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
	}

	if (!pLoadArray) {
		pLoadArray = (mp_f32*)malloc(sizeof(mp_f32) * LocalCPUCount);
		if (!pLoadArray) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
	}

	if (FAILED(mpGetProcessorsLoadPerCoreWindows(pContext, &pLoadArray))) {
		free(pLoadArray);
		pLoadArray = NULL;
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	for (mp_i32 i = 0; i < LocalCPUCount; i++) {
		fCurrentLoad += pLoadArray[i] * pLoadArray[i];
	}

	*pFloatLoad = sqrtf(fCurrentLoad);
	return ret;
}
