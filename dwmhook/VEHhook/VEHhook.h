#pragma once
//veh.h
#include <windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <iostream>
typedef PVOID(NTAPI* RtlAddVectoredExceptionHandler_t)(ULONG dwD, PVECTORED_EXCEPTION_HANDLER VEHHANDLE);
typedef ULONG(NTAPI* RtlRemoveVectoredExceptionHandler_t)(PVOID HANDLEVEH);

class VEHHook
{
private:
	bool InitDrB = false;
	bool InitDetourB = false;
	PVOID m_handlerHandle = NULL;
	RtlAddVectoredExceptionHandler_t m_pRtlAddVectoredExceptionHandler = NULL;
	RtlRemoveVectoredExceptionHandler_t m_pRtlRemoveVectoredExceptionHandler = NULL;
	// save old mem_Protect mode
	MEMORY_BASIC_INFORMATION mem_info;
public:

	uintptr_t m_original_DWM_fun = 0;
	uintptr_t m_original_MPO_fun = 0;
	uintptr_t m_hooked_DWM_fun = 0;
	uintptr_t m_hooked_MPO_fun = 0;

	DWORD old_DWM_Protection = NULL;
	DWORD old_MPO_Protection = NULL;

	DWORD hook_DWM_fun_memory_proct = NULL;
	DWORD hook_MPO_fun_memory_proct = NULL;
public:
	VEHHook();
	~VEHHook();
	// 初始化VEHHOOK
	void Init(uintptr_t _DWM_fun, uintptr_t _MPO_fun, uintptr_t hDWM_fun, uintptr_t hMPO_fun);
	// 卸载hook
	void UnHOOK();
	// 运行HOOK->其他模块线程HOOK
	int Run_HOOK();
	// 暂停hook
	void DWM_Pause_HOOK();
	void MPO_Pause_HOOK();
	// 继续hook
	void DWM_Continue_HOOK();
	void MPO_Continue_HOOK();
};
inline VEHHook g_VEHHook;
