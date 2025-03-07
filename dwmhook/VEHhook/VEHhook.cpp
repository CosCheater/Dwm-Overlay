#include "VEHhook.h"
bool AreInSamePage(const uint8_t* Addr1, const uint8_t* Addr2)
{
	MEMORY_BASIC_INFORMATION mbi1;
	if (!VirtualQuery(Addr1, &mbi1, sizeof(mbi1))) //Get Page information for Addr1
		return true;

	MEMORY_BASIC_INFORMATION mbi2;
	if (!VirtualQuery(Addr2, &mbi2, sizeof(mbi2))) //Get Page information for Addr1
		return true;

	if (mbi1.BaseAddress == mbi2.BaseAddress) //See if the two pages start at the same Base Address
		return true; //Both addresses are in the same page, abort hooking!

	return false;
}
LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
#ifdef _WIN64
#define XIP Rip
#else
#define XIP Eip
#endif
	DWORD64 page_DWM_start = ((DWORD64)(g_VEHHook.m_original_DWM_fun)) & 0xFFFFFFFFFFFFF000;
	DWORD64 page_DWM_end = page_DWM_start + 0x1000;

	DWORD64 page_MPO_start = ((DWORD64)(g_VEHHook.m_original_MPO_fun)) & 0xFFFFFFFFFFFFF000;
	DWORD64 page_MPO_end = page_MPO_start + 0x1000;

	ULONG result = 0;
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) 
	{
		if (ExceptionInfo->ContextRecord->XIP >= page_DWM_start && ExceptionInfo->ContextRecord->XIP <= page_DWM_end)
		{
			if (ExceptionInfo->ContextRecord->XIP == (uintptr_t)g_VEHHook.m_original_DWM_fun)
			{
				ExceptionInfo->ContextRecord->XIP = (uintptr_t)g_VEHHook.m_hooked_DWM_fun;
				return EXCEPTION_CONTINUE_EXECUTION;
			}
			ExceptionInfo->ContextRecord->EFlags |= 0x100; 
		}
		if (ExceptionInfo->ContextRecord->XIP >= page_MPO_start && ExceptionInfo->ContextRecord->XIP <= page_MPO_end)
		{
			if (ExceptionInfo->ContextRecord->XIP == (uintptr_t)g_VEHHook.m_original_MPO_fun)
			{
				ExceptionInfo->ContextRecord->XIP = (uintptr_t)g_VEHHook.m_hooked_MPO_fun;
				return EXCEPTION_CONTINUE_EXECUTION;
			}
			ExceptionInfo->ContextRecord->EFlags |= 0x100;
		}
		result = EXCEPTION_CONTINUE_EXECUTION; 
	}
	else if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) 
	{
		VirtualProtect((LPVOID)g_VEHHook.m_original_DWM_fun, 1, g_VEHHook.hook_DWM_fun_memory_proct | PAGE_GUARD, &g_VEHHook.old_DWM_Protection);
		VirtualProtect((LPVOID)g_VEHHook.m_original_MPO_fun, 1, g_VEHHook.hook_MPO_fun_memory_proct | PAGE_GUARD, &g_VEHHook.old_MPO_Protection);
		result = EXCEPTION_CONTINUE_EXECUTION; 
	}
	else
	{
		result = EXCEPTION_CONTINUE_SEARCH; 
	}
	return result; 
}
VEHHook::VEHHook()
{
}
VEHHook::~VEHHook()
{

}
void VEHHook::Init(uintptr_t _DWM_fun , uintptr_t _MPO_fun, uintptr_t hDWM_fun,uintptr_t hMPO_fun)
{
	this->m_original_DWM_fun = _DWM_fun;
	this->m_original_MPO_fun = _MPO_fun;

	this->m_hooked_DWM_fun = hDWM_fun;
	this->m_hooked_MPO_fun = hMPO_fun;

	// 加载 ntdll.dll
	HMODULE hNtdll = LoadLibrary(TEXT("ntdll.dll"));
	if (hNtdll == NULL) {
		std::cerr << "Failed to load ntdll.dll" << std::endl;
		return;
	}
	// 获取 RtlAddVectoredExceptionHandler 函数的地址
	m_pRtlAddVectoredExceptionHandler = (RtlAddVectoredExceptionHandler_t)GetProcAddress(hNtdll, "RtlAddVectoredExceptionHandler");
	if (m_pRtlAddVectoredExceptionHandler == NULL) {
		std::cerr << "Failed to get address of RtlAddVectoredExceptionHandler" << std::endl;
		FreeLibrary(hNtdll);
		return;
	}
	// 获取 RtlRemoveVectoredExceptionHandler 函数的地址
	m_pRtlRemoveVectoredExceptionHandler = (RtlRemoveVectoredExceptionHandler_t)GetProcAddress(hNtdll, "RtlRemoveVectoredExceptionHandler");
	if (m_pRtlRemoveVectoredExceptionHandler == NULL) {
		std::cerr << "Failed to get address of RtlRemoveVectoredExceptionHandler" << std::endl;
		FreeLibrary(hNtdll);
		return;
	}
	m_handlerHandle = m_pRtlAddVectoredExceptionHandler(1, VectoredHandler);
	if (m_handlerHandle == NULL) {
		std::cerr << "Failed to add vectored exception handler" << std::endl;
		FreeLibrary(hNtdll);
		return;
	}
}
void VEHHook::UnHOOK()
{
	DWM_Pause_HOOK();
	MPO_Pause_HOOK();
	m_pRtlRemoveVectoredExceptionHandler(VectoredHandler);
}
int VEHHook::Run_HOOK()
{

	if (AreInSamePage((const uint8_t*)this->m_original_DWM_fun, (const uint8_t*)this->m_hooked_DWM_fun))
		return false;
	if (AreInSamePage((const uint8_t*)this->m_original_MPO_fun, (const uint8_t*)this->m_hooked_MPO_fun))
		return false;

	memset(&this->mem_info, 0, sizeof(this->mem_info));
	VirtualQuery((LPVOID)this->m_original_DWM_fun, &this->mem_info, sizeof(this->mem_info));
	this->hook_DWM_fun_memory_proct = this->mem_info.Protect;

	memset(&this->mem_info, 0, sizeof(this->mem_info));
	VirtualQuery((LPVOID)this->m_original_MPO_fun, &this->mem_info, sizeof(this->mem_info));
	this->hook_MPO_fun_memory_proct = this->mem_info.Protect;

	// cheat mem_Protect mode
	if (m_handlerHandle && VirtualProtect((LPVOID)this->m_original_DWM_fun, 1, this->hook_DWM_fun_memory_proct | PAGE_GUARD, &old_DWM_Protection)
		&& VirtualProtect((LPVOID)this->m_original_MPO_fun, 1, this->hook_MPO_fun_memory_proct | PAGE_GUARD, &old_MPO_Protection))
		return true;
}
void VEHHook::DWM_Pause_HOOK()
{
	if (m_handlerHandle && VirtualProtect((LPVOID)this->m_original_DWM_fun, 1, this->hook_DWM_fun_memory_proct, &old_DWM_Protection))
		return;
}
void VEHHook::MPO_Pause_HOOK()
{
	if (m_handlerHandle && VirtualProtect((LPVOID)this->m_original_MPO_fun, 1, this->hook_MPO_fun_memory_proct, &old_MPO_Protection))
		return;
}
void VEHHook::DWM_Continue_HOOK()
{
	// cheat mem_Protect mode
	if (m_handlerHandle && VirtualProtect((LPVOID)this->m_original_DWM_fun, 1, this->hook_DWM_fun_memory_proct | PAGE_GUARD, &old_DWM_Protection))
		return;
}
void VEHHook::MPO_Continue_HOOK()
{
	// cheat mem_Protect mode
	if (m_handlerHandle && VirtualProtect((LPVOID)this->m_original_MPO_fun, 1, this->hook_MPO_fun_memory_proct | PAGE_GUARD, &old_MPO_Protection))
		return;
}