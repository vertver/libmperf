/*********************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* libmperf - perfomance counting library
* Apache-2 License
**********************************************************
* Module Name : Graphics impl.
*********************************************************/
#include "mperf_header.h"

typedef void* (__stdcall *ADL_MAIN_MALLOC_CALLBACK)(mp_i32);
typedef mp_i32(*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, mp_i32);
typedef mp_i32(*ADL_ADAPTER_ACTIVE_GET) (mp_i32, mp_i32*);
typedef mp_i32(*ADL_OVERDRIVE5_TEMPERATURE_GET) (mp_i32 AdapterIndex, mp_i32 ThermalControllerIndex, MPERF_ADL_TEMPERATURE* pTemperature);
typedef mp_i32(*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (mp_i32*);
typedef mp_i32(*ADL_ADAPTER_ADAPTERINFO_GET) (MPERF_ADL_ADAPTER_INFO*, mp_i32);
typedef mp_i32(*ADL_OVERDRIVE5_CURRENTACTIVITY_GET) (mp_i32 AdapterIndex, MPERF_ADL_PM_ACTIVITY *pActivity);
typedef mp_i32(*ADL_MAIN_CONTROL_DESTROY)();
typedef int *(*NVAPI_QUERYINTERFACE)(unsigned int offset);
typedef int(*NVAPI_INITIALIZE)();
typedef int(*NVAPI_UNLOAD)();
typedef int(*NVAPI_ENUMPHYSICALGPUS)(int **handles, int *count);
typedef int(*NVAPI_GPU_GETUSAGES)(int *handle, unsigned int *usages);

/*
	ADL functions
*/
ADL_MAIN_CONTROL_CREATE	ADL_Main_Control_Create = NULL;
ADL_MAIN_CONTROL_DESTROY ADL_Main_Control_Destroy = NULL;
ADL_ADAPTER_ADAPTERINFO_GET	ADL_GetAdapter_AdapterInfo = NULL;
ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_GetAdapter_NumberOfAdapters = NULL;
ADL_ADAPTER_ACTIVE_GET ADL_GetAdapter_Active = NULL;
ADL_OVERDRIVE5_CURRENTACTIVITY_GET ADL_GetOverdrive5_CurrentActivity = NULL;
ADL_OVERDRIVE5_TEMPERATURE_GET ADL_GetTemperatureGPU = NULL;

/*
	NVAPI functions
*/
NVAPI_QUERYINTERFACE NvAPI_QueryInterface = NULL;
NVAPI_INITIALIZE NvAPI_Initialize = NULL;
NVAPI_UNLOAD NvAPI_Unload = NULL;
NVAPI_ENUMPHYSICALGPUS NvAPI_EnumPhysicalGPUs = NULL;
NVAPI_GPU_GETUSAGES NvAPI_GPU_GetUsages = NULL;

void* 
__stdcall
ADL_Main_Memory_Alloc(
	mp_size iSize
)
{
	void* lpBuffer = malloc(iSize);
	return lpBuffer;
}

void 
__stdcall
ADL_Main_Memory_Free(
	void* lpBuffer
)
{
	if (NULL != lpBuffer) {
		free(lpBuffer);
		lpBuffer = NULL;
	}
}

mp_retcode
mpInitializeGraphicsWindows(
	MPERF_CONTEXT* pContext
)
{
	mp_i32 GpuCount = 0;
	mp_i32 AmdGpuCount = 0;
	mp_i32 RetCode = 0;
	mp_retcode ret = 0;
	HMODULE hModule = NULL;
	HMODULE hModuleNvidia = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;
	MPERF_GPU_NVIDIA_CONTEXT_WINDOWS* pNvidiaGpuContext = NULL;
	MPERF_ADL_ADAPTER_INFO* pTempAdapterInfo = NULL;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	mp_i32 TempIds[64] = { 0 };

	if (!pContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	/*
		Allocate additional struct for context
	*/
	pGpuContext = (MPERF_GPU_CONTEXT*)malloc(sizeof(MPERF_GPU_CONTEXT));
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}
	memset(pGpuContext, 0, sizeof(MPERF_GPU_CONTEXT));

tryNvidia:
#ifdef X64_PLATFORM
	hModuleNvidia = LoadLibraryW(L"nvapi64.dll");
#else
	hModuleNvidia = LoadLibraryW(L"nvapi.dll");
#endif
	if (IsInvalidHandle(hModuleNvidia)) {
		goto tryAmd;
	}

	/*
		Query all needy NVAPI interfaces
	*/
	NvAPI_QueryInterface = (NVAPI_QUERYINTERFACE)(GetProcAddress(hModuleNvidia, "nvapi_QueryInterface"));
	if (!NvAPI_QueryInterface) {
		FreeLibrary(hModuleNvidia);
		goto tryAmd;
	}

	NvAPI_Initialize = (NVAPI_INITIALIZE)(*NvAPI_QueryInterface)(0x0150E828);
	if (!NvAPI_Initialize) {
		FreeLibrary(hModuleNvidia);
		goto tryAmd;
	}

	/*
		Try to initialize NVAPI library
	*/
	if (NvAPI_Initialize() != 0) {
		FreeLibrary(hModuleNvidia);
		goto tryAmd;
	}

	NvAPI_Unload = (NVAPI_UNLOAD)(*NvAPI_QueryInterface)(0xD22BDD7E);
	NvAPI_EnumPhysicalGPUs = (NVAPI_ENUMPHYSICALGPUS)(*NvAPI_QueryInterface)(0xE5AC921F);
	NvAPI_GPU_GetUsages = (NVAPI_GPU_GETUSAGES)(*NvAPI_QueryInterface)(0x189A1FDF);
	if (!NvAPI_Unload || !NvAPI_EnumPhysicalGPUs || !NvAPI_GPU_GetUsages) {
		FreeLibrary(hModuleNvidia);
		goto tryAmd;
	}

	pNvidiaGpuContext = malloc(sizeof(MPERF_GPU_NVIDIA_CONTEXT_WINDOWS));
	if (!pNvidiaGpuContext) {
		if (NvAPI_Unload) NvAPI_Unload();
		FreeLibrary(hModuleNvidia);
		goto tryAmd;
	}

	pNvidiaGpuContext->AdaptersHandles[0] = (mp_i32)(256 | 0x10000);

	/*
		Enumerate all NVAPI devices
	*/
	if (NvAPI_EnumPhysicalGPUs(pNvidiaGpuContext->AdaptersHandles, &pNvidiaGpuContext->AdaptersCount) != 0) {
		if (NvAPI_Unload) NvAPI_Unload();
		FreeLibrary(hModuleNvidia);
		goto tryAmd;
	}

	pNvidiaGpuContext->GpuContextType = eMPerfNVIDIA;
	pGpuContext->pInternalContextNvidia = pNvidiaGpuContext;
	pGpuContext->pLibraryHandleNvidia = hModuleNvidia;

tryAmd:
	/*
		Load ADL library 
	*/
	hModule = LoadLibraryW(L"atiadlxx.dll");
	if (IsInvalidHandle(hModule)) {
		hModule = LoadLibraryW(L"atiadlxy.dll");
		if (IsInvalidHandle(hModule)) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			goto FunctionEnd;
		}
	}

	/*
		Get ADL functions procs
	*/
	ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)GetProcAddress(hModule, "ADL_Main_Control_Create");
	ADL_GetAdapter_NumberOfAdapters = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(hModule, "ADL_Adapter_NumberOfAdapters_Get");
	ADL_GetAdapter_AdapterInfo = (ADL_ADAPTER_ADAPTERINFO_GET)GetProcAddress(hModule, "ADL_Adapter_AdapterInfo_Get");
	ADL_GetAdapter_Active = (ADL_ADAPTER_ACTIVE_GET)GetProcAddress(hModule, "ADL_Adapter_Active_Get");
	ADL_GetOverdrive5_CurrentActivity = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(hModule, "ADL_Overdrive5_CurrentActivity_Get");
	ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)GetProcAddress(hModule, "ADL_Main_Control_Destroy");
	ADL_GetTemperatureGPU = (ADL_OVERDRIVE5_TEMPERATURE_GET)GetProcAddress(hModule, "ADL_Overdrive5_Temperature_Get");

	if (!ADL_Main_Control_Create) {
		FreeLibrary(hModule);
		goto FunctionEnd;
	}

	/*
		Create main ADL Control for manipulate with GPU information
	*/
	RetCode = ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1);
	if (RetCode != 0) {
		FreeLibrary(hModule);
		goto FunctionEnd;
	}

	RetCode = ADL_GetAdapter_NumberOfAdapters(&GpuCount);
	if (RetCode != 0 || GpuCount < 1) {
		if (ADL_Main_Control_Destroy) { 
			ADL_Main_Control_Destroy(); 
		}
		FreeLibrary(hModule);
		goto FunctionEnd;
	}

	pAmdGpuContext = malloc(sizeof(MPERF_GPU_AMD_CONTEXT_WINDOWS));
	if (!pAmdGpuContext) {
		if (ADL_Main_Control_Destroy) {
			ADL_Main_Control_Destroy();
		}
		FreeLibrary(hModule);
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}
	memset(pAmdGpuContext, 0, sizeof(MPERF_GPU_AMD_CONTEXT_WINDOWS));

	/*
		Allocate space for all information about devices
	*/
	pAmdGpuContext->AdaptersCount = 0;
	pTempAdapterInfo = malloc(sizeof(MPERF_ADL_ADAPTER_INFO) * (GpuCount));
	if (!pTempAdapterInfo) {
		/*
			Free all stuff
		*/
		free(pAmdGpuContext);

		if (ADL_Main_Control_Destroy) {
			ADL_Main_Control_Destroy();
		}

		FreeLibrary(hModule);
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}
	memset(pTempAdapterInfo, 0, sizeof(MPERF_ADL_ADAPTER_INFO) * (GpuCount));

	/*
		Get adapter information of all devices in system
	*/
	RetCode = ADL_GetAdapter_AdapterInfo(pTempAdapterInfo, sizeof(MPERF_ADL_ADAPTER_INFO) * GpuCount);
	if (RetCode != 0) {
		/*
			Free all stuff
		*/
		free(pTempAdapterInfo);
		free(pAmdGpuContext);

		if (ADL_Main_Control_Destroy) {
			ADL_Main_Control_Destroy();
		}

		FreeLibrary(hModule);
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	/*
		Parse adapter list and get AMD GPU
	*/
	for (mp_i32 i = 0; i < GpuCount; i++) {
		mp_i32 ActiveAdapter = 0;
		MPERF_ADL_ADAPTER_INFO* pLocalInfo = (MPERF_ADL_ADAPTER_INFO*)&pTempAdapterInfo[i];
		RetCode = ADL_GetAdapter_Active(pLocalInfo->AdapterIndex, &ActiveAdapter);
		if (RetCode != 0) {
			continue;
		}

		/*
			Check for AMD Vendor and copy index of adapter
		*/
		if (ActiveAdapter != 0 && pLocalInfo->VendorID == 1002) {
			TempIds[pAmdGpuContext->AdaptersCount] = i;
			pAmdGpuContext->AdaptersIds[pAmdGpuContext->AdaptersCount] = pLocalInfo->AdapterIndex;
			pAmdGpuContext->AdaptersCount++;
		}
	}

	/*
		If we can't find any AMD device - try open device info by NVAPI
	*/
	if (!pAmdGpuContext->AdaptersCount) {
		free(pTempAdapterInfo);
		free(pAmdGpuContext);

		if (ADL_Main_Control_Destroy) {
			ADL_Main_Control_Destroy();
		}

		FreeLibrary(hModule);
		goto FunctionEnd;
	}

	/*
		Allocate second adapter info struct
	*/
	pAmdGpuContext->pAdaptersInfo = (MPERF_ADL_ADAPTER_INFO**)malloc(sizeof(MPERF_ADL_ADAPTER_INFO*) * pAmdGpuContext->AdaptersCount);
	if (!pAmdGpuContext->pAdaptersInfo) {
		free(pTempAdapterInfo);
		free(pAmdGpuContext);

		if (ADL_Main_Control_Destroy) {
			ADL_Main_Control_Destroy();
		}

		FreeLibrary(hModule);
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}
	memset(pAmdGpuContext->pAdaptersInfo, 0, sizeof(MPERF_ADL_ADAPTER_INFO*) * pAmdGpuContext->AdaptersCount);
	
	/*
		Try to copy only AMD adapter info
	*/
	for (mp_i32 i = 0; i < pAmdGpuContext->AdaptersCount; i++) {
		MPERF_ADL_ADAPTER_INFO** ppTemp = (MPERF_ADL_ADAPTER_INFO**)pAmdGpuContext->pAdaptersInfo;
		ppTemp[i] = malloc(sizeof(MPERF_ADL_ADAPTER_INFO));
		memset(ppTemp[i], 0, sizeof(MPERF_ADL_ADAPTER_INFO));

		if (!ppTemp[i]) {
			/*
				Free all stuff 
			*/
			for (mp_i32 i = 0; i < pAmdGpuContext->AdaptersCount; i++) {
				MPERF_ADL_ADAPTER_INFO** pppTemp = (MPERF_ADL_ADAPTER_INFO**)pAmdGpuContext->pAdaptersInfo;
				if (pppTemp[i]) free(pppTemp[i]);
			}

			free(pTempAdapterInfo);
			free(pAmdGpuContext);

			if (ADL_Main_Control_Destroy) {
				ADL_Main_Control_Destroy();
			}

			FreeLibrary(hModule);
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		memcpy(ppTemp[i], &pTempAdapterInfo[TempIds[i]], sizeof(MPERF_ADL_ADAPTER_INFO));
	}
	
	pAmdGpuContext->GpuContextType = 1;
	pGpuContext->pLibraryHandleAmd = hModule;
	for (mp_i32 i = 0; i < pAmdGpuContext->AdaptersCount; i++) {
		/*
			Allocate GPU context and strings
		*/
		MPERF_ADL_ADAPTER_INFO** ppTemp = (MPERF_ADL_ADAPTER_INFO**)pAmdGpuContext->pAdaptersInfo;
		pGpuContext->pDriverString[i] = malloc(strlen(ppTemp[i]->szDriverPathExt) + 1);
		pGpuContext->pVendorString[i] = malloc(strlen(ppTemp[i]->szPNPString) + 1);
		pGpuContext->pHostString[i] = malloc(strlen(ppTemp[i]->szAdapterName) + 1);

		if (!pGpuContext->pDriverString[i] || !pGpuContext->pVendorString[i] || !pGpuContext->pHostString[i]) {
			/*
				Free all stuff
			*/
			for (size_t i = 0; i < pAmdGpuContext->AdaptersCount; i++) {
				MPERF_ADL_ADAPTER_INFO** pppTemp = (MPERF_ADL_ADAPTER_INFO**)pAmdGpuContext->pAdaptersInfo;
				if (pppTemp[i]) free(pppTemp[i]);
			}

			free(pTempAdapterInfo);
			free(pAmdGpuContext);

			if (ADL_Main_Control_Destroy) {
				ADL_Main_Control_Destroy();
			}

			FreeLibrary(hModule);
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		/*
			Copy string data to local GPU context
		*/
		memset(pGpuContext->pDriverString[i], 0, strlen(ppTemp[i]->szDriverPathExt) + 1);
		memcpy(pGpuContext->pDriverString[i], ppTemp[i]->szDriverPathExt, strlen(ppTemp[i]->szDriverPathExt));
		memset(pGpuContext->pVendorString[i], 0, strlen(ppTemp[i]->szPNPString) + 1);
		memcpy(pGpuContext->pVendorString[i], ppTemp[i]->szPNPString, strlen(ppTemp[i]->szPNPString));
		memset(pGpuContext->pHostString[i], 0, strlen(ppTemp[i]->szAdapterName) + 1);
		memcpy(pGpuContext->pHostString[i], ppTemp[i]->szAdapterName, strlen(ppTemp[i]->szAdapterName));
	}

	if (!pAmdGpuContext->AdaptersCount) {
		/*
			Free all stuff
		*/
		for (mp_i32 i = 0; i < pAmdGpuContext->AdaptersCount; i++) {
			MPERF_ADL_ADAPTER_INFO** pppTemp = (MPERF_ADL_ADAPTER_INFO**)pAmdGpuContext->pAdaptersInfo;
			if (pppTemp[i]) free(pppTemp[i]);
		}

		free(pTempAdapterInfo);
		free(pAmdGpuContext);

		if (ADL_Main_Control_Destroy) {
			ADL_Main_Control_Destroy();
		}

		FreeLibrary(hModule);
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	if (pTempAdapterInfo) free(pTempAdapterInfo);
	pAmdGpuContext->GpuContextType = eMPerfAMD;
	pGpuContext->pInternalContext = pAmdGpuContext;
	pContext->pGpuMperfContext = pGpuContext;

FunctionEnd:
	return ret;
}

mp_retcode
mpUnitializeGraphicsWindows(
	MPERF_CONTEXT* pContext
)
{
	mp_retcode ret = 0;
	mp_i32 ThisType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	if (!pGpuContext->pInternalContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	ThisType = *(mp_i32*)pGpuContext->pInternalContext;

	if (pGpuContext->pInternalContextNvidia) {
		/*
			#TODO: delete NVIDIA stuff
		*/
		if (NvAPI_Unload) NvAPI_Unload();
		if (pGpuContext->pInternalContextNvidia) free(pGpuContext->pInternalContextNvidia);
	}

	switch (ThisType) {
	case eMPerfAMD:
		pAmdGpuContext = pGpuContext->pInternalContext;	
		for (mp_i32 i = 0; i < pAmdGpuContext->AdaptersCount; i++) {
			MPERF_ADL_ADAPTER_INFO** pppTemp = (MPERF_ADL_ADAPTER_INFO**)pAmdGpuContext->pAdaptersInfo;
			if (pGpuContext->pDriverString[i]) free(pGpuContext->pDriverString[i]);
			if (pGpuContext->pVendorString[i]) free(pGpuContext->pVendorString[i]);
			if (pGpuContext->pHostString[i]) free(pGpuContext->pHostString[i]);
			if (pppTemp[i]) free(pppTemp[i]);
		}

		if (pAmdGpuContext->pAdaptersInfo) free(pAmdGpuContext->pAdaptersInfo);		
		if (!IsInvalidHandle(pGpuContext->pLibraryHandleAmd)) FreeLibrary(pGpuContext->pLibraryHandleAmd);
		if (!IsInvalidHandle(pGpuContext->pLibraryHandleNvidia)) FreeLibrary(pGpuContext->pLibraryHandleNvidia);
		break;
	case eMPerfNVIDIA:
	case eMPerfNone:
	default:
		break;
	}

	if (pContext->pGpuMperfContext) {
		free(pContext->pGpuMperfContext);
		pContext->pGpuMperfContext = NULL;
	}

	return ret;
}

mp_retcode
mpGetGraphicsAdaptersCountWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32* pAdaptersCount
)
{
	mp_retcode ret = 0;
	mp_i32 ThisAmdType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext || !pAdaptersCount) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}
	
	switch (GpuVendorType)
	{
	case eMPerfAMD:
		pAmdGpuContext = pGpuContext->pInternalContext;
		if (!pAmdGpuContext) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
		
		*pAdaptersCount = pAmdGpuContext->AdaptersCount;  
		break;
	case eMPerfNVIDIA:
	{
		if (!pGpuContext->pInternalContextNvidia) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		MPERF_GPU_NVIDIA_CONTEXT_WINDOWS* pNvidiaContext = pGpuContext->pInternalContextNvidia;
		*pAdaptersCount = pNvidiaContext->AdaptersCount;
	}
		break;
	case eMPerfNone:
	default:
		break;
	}

	return ret;
}

mp_retcode
mpGetGraphicsAdapterLoadWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_f32* pFloatLoad
)
{
	mp_retcode ret = 0;
	mp_i32 RetCode = 0;
	mp_i32 ThisAmdType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext || !pFloatLoad || AdapterIndex > 63 || AdapterIndex < 0) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	switch (GpuVendorType)
	{
	case eMPerfAMD:
	{
		MPERF_ADL_PM_ACTIVITY pmActivity = { 0 };
		pAmdGpuContext = pGpuContext->pInternalContext;
		if (!pAmdGpuContext) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		/*
			We must verify our function
		*/
		if (!ADL_GetOverdrive5_CurrentActivity) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		/*
			Get usage of GPU by Id
		*/
		RetCode = ADL_GetOverdrive5_CurrentActivity(pAmdGpuContext->AdaptersIds[AdapterIndex], &pmActivity);
		if (RetCode != 0) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		*pFloatLoad = (mp_f32)pmActivity.ActivityPercent / 100.f;
	}
		break;
	case eMPerfNVIDIA:
	{
		unsigned int CurrentUsages[34] = { 0 };
		MPERF_GPU_NVIDIA_CONTEXT_WINDOWS* pNvidiaContext = pGpuContext->pInternalContextNvidia;
		if (!pNvidiaContext) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}
		
		/*
			Get usages of needy GPU
		*/
		if (NvAPI_GPU_GetUsages(pNvidiaContext->AdaptersHandles[AdapterIndex], CurrentUsages) != 0) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		/*
			It's fucking magic number
		*/
		*pFloatLoad = (mp_f32)CurrentUsages[3] / 100.f;
	}
	case eMPerfNone:
	default:
		break;
	}

	return ret;
}


mp_retcode
mpGetGraphicsAdapterMemoryLoadWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pCountInKBytes
)
{
	mp_retcode ret = 0;

	/*
		Not implemented yet
	*/
	SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
	ret = MPERF_FAILED;
	return ret;
}

mp_retcode
mpGetGraphicsAdapterEngineFrequencyWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pFrequency
)
{
	mp_retcode ret = 0;
	mp_i32 RetCode = 0;
	mp_i32 ThisAmdType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext || !pFrequency || AdapterIndex > 63 || AdapterIndex < 0) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	switch (GpuVendorType)
	{
	case eMPerfAMD:
	{
		MPERF_ADL_PM_ACTIVITY pmActivity = { 0 };
		pAmdGpuContext = pGpuContext->pInternalContext;
		if (!pAmdGpuContext) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		if (!ADL_GetOverdrive5_CurrentActivity) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		RetCode = ADL_GetOverdrive5_CurrentActivity(pAmdGpuContext->AdaptersIds[AdapterIndex], &pmActivity);
		if (RetCode != 0) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		*pFrequency = pmActivity.EngineClock;
	}
	break;
	case eMPerfNVIDIA:
		/*
			Now only for AMD
		*/
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
		break;
	case eMPerfNone:
	default:
		break;
	}

	return ret;
}

mp_retcode
mpGetGraphicsAdapterMemoryFrequencyWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pFrequency
)
{
	mp_retcode ret = 0;
	mp_i32 RetCode = 0;
	mp_i32 ThisAmdType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext || !pFrequency || AdapterIndex > 63 || AdapterIndex < 0) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	switch (GpuVendorType)
	{
	case eMPerfAMD:
	{
		MPERF_ADL_PM_ACTIVITY pmActivity = { 0 };
		pAmdGpuContext = pGpuContext->pInternalContext;
		if (!pAmdGpuContext) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		if (!ADL_GetOverdrive5_CurrentActivity) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		RetCode = ADL_GetOverdrive5_CurrentActivity(pAmdGpuContext->AdaptersIds[AdapterIndex], &pmActivity);
		if (RetCode != 0) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		*pFrequency = pmActivity.MemoryClock;
	}
	break;
	case eMPerfNVIDIA:
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	case eMPerfNone:
	default:
		break;
	}

	return ret;
}

mp_retcode
mpGetGraphicsAdapterCoreVoltageWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_i32* pVoltage
)
{
	mp_retcode ret = 0;
	mp_i32 RetCode = 0;
	mp_i32 ThisAmdType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext || !pVoltage || AdapterIndex > 63 || AdapterIndex < 0) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	switch (GpuVendorType)
	{
	case eMPerfAMD:
	{
		MPERF_ADL_PM_ACTIVITY pmActivity = { 0 };
		pAmdGpuContext = pGpuContext->pInternalContext;
		if (!pAmdGpuContext) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		if (!ADL_GetOverdrive5_CurrentActivity) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		RetCode = ADL_GetOverdrive5_CurrentActivity(pAmdGpuContext->AdaptersIds[AdapterIndex], &pmActivity);
		if (RetCode != 0) {
			SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
			ret = MPERF_FAILED;
			return ret;
		}

		*pVoltage = pmActivity.Vddc;
	}
	break;
	case eMPerfNVIDIA:
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	case eMPerfNone:
	default:
		break;
	}

	return ret;
}

mp_retcode
mpGetGraphicsAdapterDescriptionWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_utf8* pGPUString,
	mp_i32 StringSize
)
{
	mp_retcode ret = 0;
	mp_i32 RetCode = 0;
	mp_i32 ThisAmdType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext || AdapterIndex > 63 || AdapterIndex < 0) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	switch (GpuVendorType)
	{
	case eMPerfAMD:
	{
		strcpy_s(pGPUString, StringSize, pGpuContext->pHostString[AdapterIndex]);
	}
	break;
	case eMPerfNVIDIA:
	{
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}
	case eMPerfNone:
	default:
		break;
	}

	return ret;
}

mp_retcode
mpGetGraphicsAdapterVendorNameWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_utf8* pGPUString,
	mp_i32 StringSize
)
{
	mp_retcode ret = 0;
	mp_i32 RetCode = 0;
	mp_i32 ThisAmdType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext || AdapterIndex > 63 || AdapterIndex < 0) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	switch (GpuVendorType)
	{
	case eMPerfAMD:
	{	
		strcpy_s(pGPUString, StringSize, pGpuContext->pVendorString[AdapterIndex]);
	}
	break;
	case eMPerfNVIDIA:
	case eMPerfNone:
	default:
		break;
	}

	return ret;
}

mp_retcode
mpGetGraphicsAdapterDriverVersionWindows(
	MPERF_CONTEXT* pContext,
	mp_i32 GpuVendorType,
	mp_i32 AdapterIndex,
	mp_utf8* pGPUString,
	mp_i32 StringSize
)
{
	mp_retcode ret = 0;
	mp_i32 RetCode = 0;
	mp_i32 ThisAmdType = 0;
	MPERF_GPU_CONTEXT* pGpuContext = NULL;
	MPERF_GPU_AMD_CONTEXT_WINDOWS* pAmdGpuContext = NULL;

	if (!pContext || AdapterIndex > 63 || AdapterIndex < 0) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	pGpuContext = pContext->pGpuMperfContext;
	if (!pGpuContext) {
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	}

	switch (GpuVendorType)
	{
	case eMPerfAMD:
	{
		strcpy_s(pGPUString, StringSize, pGpuContext->pDriverString[AdapterIndex]);
	}
	break;
	case eMPerfNVIDIA:
		SET_MPERF_ERROR_CONTEXT(MPERF_ERROR_UNDEFINED);
		ret = MPERF_FAILED;
		return ret;
	case eMPerfNone:
	default:
		break;
	}

	return ret;
}
