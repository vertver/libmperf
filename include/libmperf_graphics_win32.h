/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Graphics impl.
*********************************************************/
#pragma once
#ifdef WINDOWS_PLATFORM

typedef struct  
{
	mp_i32 GpuContextType;			// ex. eMPerfAMD
	mp_i32 AdaptersCount;
	mp_i32 AdaptersIds[64];			// Impossible to get more GPU
	void* pAdaptersInfo;
} MPERF_GPU_AMD_CONTEXT_WINDOWS;

typedef struct
{
	mp_i32 GpuContextType;			// ex. eMPerfNVIDIA
	mp_i32 AdaptersCount;
	mp_i32* AdaptersHandles[64];	// Impossible to get more GPU
} MPERF_GPU_NVIDIA_CONTEXT_WINDOWS;

mp_retcode
mpInitializeGraphicsWindows(
	MPERF_CONTEXT* pContext
);

mp_retcode
mpUnitializeGraphicsWindows(
	MPERF_CONTEXT* pContext
);

mp_retcode
mpGetGraphicsAdaptersCountWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32* pAdaptersCount
);

mp_retcode
mpGetGraphicsAdapterLoadWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_f32* pFloatLoad
);

mp_retcode
mpGetGraphicsAdapterMemoryLoadWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pCountInKBytes
);

mp_retcode
mpGetGraphicsAdapterEngineFrequencyWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pFrequency
);

mp_retcode
mpGetGraphicsAdapterMemoryFrequencyWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pFrequency
);

mp_retcode
mpGetGraphicsAdapterCoreVoltageWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pVoltage
);



mp_retcode
mpGetGraphicsAdapterDescriptionWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_utf8* pGPUString,
	mp_i32 StringSize
);


mp_retcode
mpGetGraphicsAdapterVendorNameWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_utf8* pGPUString,
	mp_i32 StringSize
);

mp_retcode
mpGetGraphicsAdapterDriverVersionWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_utf8* pGPUString,
	mp_i32 StringSize
);
#endif