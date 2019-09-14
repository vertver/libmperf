/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Graphics impl.
*********************************************************/
#pragma once

typedef struct
{
	mp_i32 StructSize;
	mp_i32 Temperature;
} MPERF_ADL_TEMPERATURE;

typedef struct
{
	mp_i32 StructSize;
	mp_i32 AdapterIndex;
	mp_utf8string256 szStringUDID;
	mp_i32 AdapterBusNumber;
	mp_i32 DeviceNumber;
	mp_i32 FunctionNumber;
	mp_i32 VendorID;
	mp_utf8string256 szAdapterName;
	mp_utf8string256 szDisplayName;
	mp_i32 Present;
#if defined(WINDOWS_PLATFORM)
	mp_i32 IsExist;
	mp_utf8string256 szDriverPath;
	mp_utf8string256 szDriverPathExt;
	mp_utf8string256 szPNPString;
	mp_i32 DisplayIndex;
#elif defined (LINUX_PLATFORM)
	mp_i32 XDisplayNumber;
	mp_i32 iDrvIndex;
	mp_utf8string256 strXScreenConfigName;
#endif
} MPERF_ADL_ADAPTER_INFO;

typedef struct
{
	mp_i32 StructSize;
	mp_i32 EngineClock;
	mp_i32 MemoryClock;
	mp_i32 Vddc;
	mp_i32 ActivityPercent;
	mp_i32 CurrentPerformanceLevel;
	mp_i32 CurrentBusSpeed;
	mp_i32 CurrentBusLanes;
	mp_i32 MaximumBusLanes;
	mp_i32 ReservedSlot;
} MPERF_ADL_PM_ACTIVITY;

typedef enum
{
	eMPerfNone, 
	eMPerfAMD,
	eMPerfNVIDIA
} MPERF_GPU_ENUM;

typedef struct  
{
	mp_utf8* pVendorString[32];		// ex. AMD
	mp_utf8* pHostString[32];		// ex. AMD Radeon (TM) R9 380 Series
	mp_utf8* pDriverString[32];		// ex. 26.20.13001.25001
	void* pLibraryHandleAmd;		// handle to linked dynamic library
	void* pLibraryHandleNvidia;		// handle to linked dynamic library
	void* pInternalContext;			// context for internal things
	void* pInternalContextNvidia;	// other context for internal things
} MPERF_GPU_CONTEXT;

mp_retcode
MPERF_API
mpInitializeGraphics(
	MPERF_CONTEXT* pContext
);

mp_retcode
MPERF_API
mpUnitializeGraphics(
	MPERF_CONTEXT* pContext
);



mp_retcode
MPERF_API
mpGetGraphicsAdaptersCount(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32* pAdaptersCount
);

mp_retcode
MPERF_API
mpGetGraphicsAdapterLoad(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_f32* pFloatLoad
);

mp_retcode
MPERF_API
mpGetGraphicsAdapterMemoryLoad(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pCountInKBytes
);

mp_retcode
MPERF_API
mpGetGraphicsAdapterEngineFrequency(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pFrequency
);

mp_retcode
MPERF_API
mpGetGraphicsAdapterMemoryFrequency(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pFrequency
);

mp_retcode
MPERF_API
mpGetGraphicsAdapterCoreVoltage(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pVoltage
);


mp_retcode
MPERF_API
mpGetGraphicsAdapterDescription(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_utf8* pGPUString,
	mp_i32 StringSize
);

mp_retcode
MPERF_API
mpGetGraphicsAdapterVendorName(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_utf8* pGPUString,
	mp_i32 StringSize
);

mp_retcode
MPERF_API
mpGetGraphicsAdapterDriverVersion(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_utf8* pGPUString,
	mp_i32 StringSize
);
