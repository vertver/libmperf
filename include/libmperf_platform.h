/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Platform impl.
*********************************************************/
#pragma once

#ifdef WINDOWS_PLATFORM
#include <windows.h>
#include "libmperf_context_win32.h"
#include "libmperf_graphics_win32.h"
#include "libmperf_cpu_win32.h"
#include "libmperf_processes_win32.h"
#include "libmperf_memory_win32.h"

#ifdef _WIN64
#define X64_PLATFORM 1
#endif

#define SET_MPERF_ERROR(x, y) (x, y)
#define SET_MPERF_ERROR_CONTEXT(x) (x, pContext)

#define mpInitPlatform(x) mpInitializeWindows(x)
#define mpDestroyPlatform(x) mpUnitializeWindows(x)

#define mpGetCPUDescription(x, y, z) mpGetCpuDescriptionWindows(x, y, z)
#define mpGetCPUCount(x, y) mpGetProcessorsCountWindows(x, y)
#define mpGetCPULoadPerCore(x, y) mpGetProcessorsLoadPerCoreWindows(x, y)
#define mpGetCPULoad(x, y) mpGetProcessorsLoadWindows(x, y)

#define mpInitGraphics(x) mpInitializeGraphicsWindows(x)
#define mpDestroyGraphics(x) mpUnitializeGraphicsWindows(x)
#define mpGetGPULoad(x, y, z, a) mpGetGraphicsAdapterLoadWindows(x, y, z, a)
#define mpGetGPUCount(x, y, z) mpGetGraphicsAdaptersCountWindows(x, y, z)
#define mpGetGPUMemoryLoad(x, y, z, a) mpGetGraphicsAdapterMemoryLoadWindows(x, y, z, a)
#define mpGetGPUEngineFrequency(x, y, z, a) mpGetGraphicsAdapterEngineFrequencyWindows(x, y, z, a)
#define mpGetGPUMemoryFrequency(x, y, z, a) mpGetGraphicsAdapterMemoryFrequencyWindows(x, y, z, a) 
#define mpGetGPUCoreVoltage(x, y, z, a) mpGetGraphicsAdapterCoreVoltage(x, y, z, a)
#define mpGetGPUDescription(x, y, z, a) mpGetGraphicsAdapterDescription(x, y, z, a)
#define mpGetGPUVendor(x, y, z, a) mpGetGraphicsAdapterVendorName(x, y, z, a)
#define mpGetGPUDriver(x, y, z, a) mpGetGraphicsAdapterDriverVersion(x, y, z, a)

#define mpGetProcessId(x, y) mpGetCurrentProcessIdWindows(x, y)
#define mpGetProcesses(x, y) mpGetProcessorsCountWindows(x, y)

#define mpGetSwapLoad(x, y) mpGetLoadedSwapMemoryWindows(x, y)
#define mpGetSwapLoadByProcess(x, y) mpGetLoadedSwapMemoryByProcessWindows(x, y)
#define mpGetMemoryLoad(x, y) mpGetLoadedMemoryWindows(x, y)
#define mpGetMemoryLoadByProcess(x, y) mpGetLoadedMemoryByProcessWindows(x, y)
#define mpGetSwapAvail(x, y) mpGetAvailableSwapMemoryWindows(x, y)
#define mpGetMemoryAvail(x, y) mpGetAvailableMemoryWindows(x, y)
#else
#include <dirent.h>

#ifdef LINUX_PLATFORM
#include <pthread.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#endif

#ifdef MAC_PLATFORM
#include <sys/sysctl.h>
#endif
#endif