/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Graphics impl.
*********************************************************/
#include "mperf_header.h"

mp_retcode
MPERF_API
mpInitializeGraphics(
	MPERF_CONTEXT* pContext
)
{
	return mpInitGraphics(pContext);
}

mp_retcode
MPERF_API
mpUnitializeGraphics(
	MPERF_CONTEXT* pContext
)
{
	return mpDestroyGraphics(pContext);
}

mp_retcode
MPERF_API
mpGetGraphicsAdapterLoad(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_f32* pFloatLoad
)
{
	return mpGetGPULoad(pContext, GpuVendorType, AdapterIndex, pFloatLoad);
}

mp_retcode
MPERF_API
mpGetGraphicsAdaptersCount(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32* pAdaptersCount
)
{
	return mpGetGPUCount(pContext, GpuVendorType, pAdaptersCount);
}

mp_retcode
MPERF_API
mpGetGraphicsAdapterMemoryLoad(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pCountInKBytes
)
{
	return mpGetGPUMemoryLoad(pContext, GpuVendorType, AdapterIndex, pCountInKBytes);
}

mp_retcode
MPERF_API
mpGetGraphicsAdapterEngineFrequency(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pFrequency
)
{
	return mpGetGPUEngineFrequency(pContext, GpuVendorType, AdapterIndex, pFrequency);
}

mp_retcode
MPERF_API
mpGetGraphicsAdapterMemoryFrequency(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pFrequency
)
{
	return mpGetGPUMemoryFrequency(pContext, GpuVendorType, AdapterIndex, pFrequency);
}

mp_retcode
MPERF_API
mpGetGraphicsAdapterCoreVoltage(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pVoltage
)
{
	return mpGetGPUCoreVoltage(pContext, GpuVendorType, AdapterIndex, pVoltage);
}



mp_retcode
MPERF_API
mpGetGraphicsAdapterDescription(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_utf8* pGPUString,
	mp_i32 StringSize
)
{
	return mpGetGPUDescription(pContext, GpuVendorType, pGPUString, StringSize);
}

mp_retcode
MPERF_API
mpGetGraphicsAdapterVendorName(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_utf8* pGPUString,
	mp_i32 StringSize
)
{
	return mpGetGPUVendor(pContext, GpuVendorType, pGPUString, StringSize);
}

mp_retcode
MPERF_API
mpGetGraphicsAdapterDriverVersion(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_utf8* pGPUString,
	mp_i32 StringSize
)
{
	return mpGetGPUDriver(pContext, GpuVendorType, pGPUString, StringSize);
}
