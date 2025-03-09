#include "includes.hpp"

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
RECT ProgramRect;
ImFont* Font;

UINT64 PresentCall = 0;

bool debug = false;

void SetMouse()
{
	POINT MousePos;
	GetCursorPos(&MousePos);
	ImGui::GetIO().MousePos.x = static_cast<float>(MousePos.x);
	ImGui::GetIO().MousePos.y = static_cast<float>(MousePos.y);
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		ImGui::GetIO().MouseDown[0] = true;
	}
	else
	{
		ImGui::GetIO().MouseDown[0] = false;
	}
}

extern "C" void DrawEverything(IDXGISwapChain* pDxgiSwapChain)
{
	static bool b = true;
	if (b)
	{
		ID3D11Device* pDevice = NULL;
		pDxgiSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);

		g_pSwapChain = pDxgiSwapChain;
		g_pd3dDevice = pDevice;
		g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);

		// 创建渲染目标
		ID3D11Texture2D* p_Buffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&p_Buffer));
		if (p_Buffer == nullptr)
			return;
		g_pd3dDevice->CreateRenderTargetView(p_Buffer, NULL, &g_mainRenderTargetView);
		p_Buffer->Release();

		// 创建imgui上下文
		ImGui::CreateContext();

		// 设置风格
		ImGui::StyleColorsLight();

		// 初始化ImGui
		HWND hProgram = FindWindow("Progman", "Program Manager");
		GetClientRect(hProgram, &ProgramRect);
		ImGui_ImplWin32_Init(hProgram);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

		// 加载字体
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
		Font = io.Fonts->AddFontFromMemoryTTF(fonts, sizeof(fonts), 15, NULL, io.Fonts->GetGlyphRangesChineseFull());

		b = false;
	}
	else
	{
		SetMouse();
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin(u8"DWM测试");
		{
			ImGui::Text(u8"DWM测试");

			ImGui::Checkbox(u8"DWM测试", &debug);
		}
		ImGui::End();
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(100, 50), ImColor({ 255,0,0,255 }), u8"DWM测试");
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(100, 100), ImColor({ 255,255,0,255 }), u8"DWM测试");
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(100, 150), ImColor({ 255,255,255,255 }), u8"DWM测试");
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(100, 200), ImColor({ 0,255,0,255 }), u8"DWM测试");
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

using PresentMPO_ = __int64(__fastcall*)(void*, IDXGISwapChain*, unsigned int, unsigned int, int, __int64, __int64, int);
PresentMPO_ oPresentMPO = NULL;
__int64 __fastcall hkPresentMPO(void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, int a5, __int64 a6, __int64 a7, int a8)
{
	DrawEverything(pDxgiSwapChain);
#ifdef VEHHOOK
	g_VEHHook.MPO_Pause_HOOK();
#endif
	auto ret = oPresentMPO(thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8);
#ifdef VEHHOOK
	g_VEHHook.MPO_Continue_HOOK();
#endif
	return ret;
}

using PresentDWM_ = __int64(__fastcall*)(void*, IDXGISwapChain*, unsigned int, unsigned int, const struct tagRECT*, unsigned int, const struct DXGI_SCROLL_RECT*, unsigned int, struct IDXGIResource*, unsigned int);
PresentDWM_ oPresentDWM = NULL;
__int64 __fastcall hkPresentDWM(void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, unsigned int a8, struct IDXGIResource* a9, unsigned int a10)
{
	DrawEverything(pDxgiSwapChain);
#ifdef VEHHOOK
	g_VEHHook.DWM_Pause_HOOK();
#endif
	auto ret = oPresentDWM(thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8, a9, a10);
#ifdef VEHHOOK
	g_VEHHook.DWM_Continue_HOOK();
#endif
	return ret;
}

using PresentMPO_dxgi_ = __int64(__fastcall*)(IDXGISwapChain* pDxgiSwapChain, int a2, int a3, enum DXGI_HDR_METADATA_TYPE a4, const void* a5, unsigned int a6, const struct _DXGI_PRESENT_MULTIPLANE_OVERLAY* a7);
PresentMPO_dxgi_ oPresentMPO_dxgi_ = NULL;
__int64 __fastcall hkPresentMPO_dxgi(IDXGISwapChain* pDxgiSwapChain, int a2, int a3, enum DXGI_HDR_METADATA_TYPE a4, const void* a5, unsigned int a6, const struct _DXGI_PRESENT_MULTIPLANE_OVERLAY* a7) {
	DrawEverything(pDxgiSwapChain);
#ifdef VEHHOOK
	g_VEHHook.MPO_Pause_HOOK();
#endif
	auto ret = oPresentMPO_dxgi_(pDxgiSwapChain, a2, a3, a4, a5, a6, a7);
#ifdef VEHHOOK
	g_VEHHook.MPO_Continue_HOOK();
#endif
	return ret;
}

using PresentDWM_dxgi_ = __int64(__fastcall*)(IDXGISwapChain* pDxgiSwapChain, int a2, int a3, char a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, struct IDXGIResource* a8, unsigned int a9);
PresentDWM_dxgi_ oPresentDWM_dxgi_ = NULL;
__int64 __fastcall hkPresentDWM_dxgi(IDXGISwapChain* pDxgiSwapChain, int a2, int a3, char a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, struct IDXGIResource* a8, unsigned int a9) {
	DrawEverything(pDxgiSwapChain);
#ifdef VEHHOOK
	g_VEHHook.DWM_Pause_HOOK();
#endif
	auto ret = oPresentDWM_dxgi_(pDxgiSwapChain, a2, a3, a4, a5, a6, a7, a8, a9);
#ifdef VEHHOOK
	g_VEHHook.DWM_Continue_HOOK();
#endif
	return ret;
}

UINT WINAPI MainThread(PVOID)
{
#ifndef CALLJMP
#ifdef MINHOOK
	MH_Initialize();
#endif
#endif // !CALLJMP
	while (!GetModuleHandleA("dwmcore.dll"))
		Sleep(150);

	// [ E8 ? ? ? ? ] the relative addr will be converted to absolute addr
	auto ResolveCall = [](DWORD_PTR sig)
		{
			return sig = sig + *reinterpret_cast<PULONG>(sig + 1) + 5;
		};
	// [ 48 8D 05 ? ? ? ? ] the relative addr will be converted to absolute addr
	auto ResolveRelative = [](DWORD_PTR sig)
		{
			return sig = sig + *reinterpret_cast<PULONG>(sig + 0x3) + 0x7;
		};
	// to [E8 ? ? ? ? ]
	auto AbsoluteToRelative = [](DWORD_PTR currentAddress, DWORD_PTR targetAddress) {
		return targetAddress - (currentAddress + 5);
		};
	// 判断版本
	typedef LONG(NTAPI* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
	fnRtlGetVersion pRtlGetVersion = NULL;
	while (pRtlGetVersion == 0)
	{
		pRtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
	}
	RTL_OSVERSIONINFOW osversion{};
	pRtlGetVersion(&osversion);
#ifdef CALLJMP
	DWORD64 version = osversion.dwBuildNumber;
	// win10 21h2 
	if (version <= 19044)
	{
		auto dxgi = (UINT64)GetModuleHandleA("dxgi.dll");
		// 拿到虚表
		DWORD64 DrawingContext = FindPatternImage(dxgi,
			"\x48\x8B\xC4\x41\x56\x48\x83\xEC\x00\x48\xC7\x40\x00\x00\x00\x00\x00\x48\x89\x58\x00\x48\x89\x68\x00\x48\x89\x78",
			"xxxxxxxx?xxx?????xxx?xxx?xxx");

		if (DrawingContext)
		{
			DWORD Protect = 0;
			// 定位到第四个虚表地址
			DrawingContext += 0x40;
			// rv 
			DrawingContext = ResolveRelative(DrawingContext);
			// virtual table sixteen
			PDWORD_PTR Vtbl = PDWORD_PTR(DrawingContext);
#ifdef MODEL_ONE
			if (Vtbl[16])
			{
				// presentimpl adr
				DrawingContext = FindPattern(Vtbl[16], 0xFF, "\x20\xE8", "xx") + 1;
				// get call adr == impl
				DrawingContext = ResolveCall(DrawingContext);
				// __guard_xfg_dispatch_icall_fptr
				DrawingContext = FindPattern(DrawingContext, 0xFF, "\xFF\x15", "xx");
#ifdef OUTADR
				std::cout << std::hex << (DWORD64)DrawingContext << std::endl;
#endif // OUTPUT
				// 拿到LdrpDispatchUserCallTarget的值
				PresentCall = DrawingContext + 6;
				// 拿到LdrpDispatchUserCallTarget要跳转到的地址 也就是call的地址
				DrawingContext += static_cast<unsigned long long>(+*(int*)(DrawingContext + 2)) + 6;
				// 修改这个call
				if (VirtualProtect((PVOID)DrawingContext, 8, PAGE_READWRITE, &Protect)) {

					_InterlockedExchangePointer((PVOID*)DrawingContext, AsmLdrpDispatchUserCallTarget_impl);
					VirtualProtect((PVOID)DrawingContext, 8, Protect, &Protect);
				}
			}
#endif
#ifdef MODEL_TWO
			if (Vtbl[23])
			{
				// __guard_xfg_dispatch_icall_fptr
				DrawingContext = FindPattern(Vtbl[23], 0xFF, "\xFF\x15", "xx");
#ifdef OUTADR
				std::cout << std::hex << (DWORD64)DrawingContext << std::endl;
#endif	
				// 拿到LdrpDispatchUserCallTarget的值
				PresentCall = DrawingContext + 6;
				DrawingContext += static_cast<unsigned long long>(+*(int*)(DrawingContext + 2)) + 6;
				// 修改这个call
				if (VirtualProtect((PVOID)DrawingContext, 8, PAGE_READWRITE, &Protect)) {

					_InterlockedExchangePointer((PVOID*)DrawingContext, AsmLdrpDispatchUserCallTarget_impl_ex3);
					VirtualProtect((PVOID)DrawingContext, 8, Protect, &Protect);
				}
			}
#endif // MODEL_TWO
			return true;
		}
	}
	// win10 22h2
	else if (version == 19045)
	{
		// get moudle adress
		auto d2d1 = (UINT64)GetModuleHandleA("d2d1.dll");
		// 拿到虚表
		auto DrawingContext = FindPatternImage(d2d1, "\x48\x8D\x05\x00\x00\x00\x00\x33\xED\x48\x8D\x71\x08", "xxx????xxxxxx");
		if (!DrawingContext) return FALSE;
		// 计算虚表的值
		DrawingContext += +*(int*)(DrawingContext + 3) + 7;
		// 拿到PresentMultiplaneOverlay
		auto PresentMultiplaneOverlay = ((UINT64*)DrawingContext)[7];
		// 拿到LdrpDispatchUserCallTarget //.text:00000001801620CE FF 15 D4 17 1B 00 call    cs:__guard_dispatch_icall_fptr
		auto LdrpDispatchUserCallTarget = FindPattern(PresentMultiplaneOverlay, 0x50, "\xFF\x15", "xx");
		//auto LdrpDispatchUserCallTarget = FindPatternImage(d2d1, "\xE8\x6D\x64\x1A\x00\x48\x8B\xC8\x4C\x8B\x10\x49\x8B\x02", "xxxxxxxxxxxxxx");
		if (!LdrpDispatchUserCallTarget) return FALSE;
		// 拿到LdrpDispatchUserCallTarget的值
		PresentCall = LdrpDispatchUserCallTarget + 6;
		// 拿到LdrpDispatchUserCallTarget要跳转到的地址 也就是call的地址
		LdrpDispatchUserCallTarget += static_cast<unsigned long long>(+*(int*)(LdrpDispatchUserCallTarget + 2)) + 6;
		auto Protect = 0ul;
		// 修改这个call
		if (VirtualProtect((PVOID)LdrpDispatchUserCallTarget, 8, PAGE_READWRITE, &Protect)) {

			_InterlockedExchangePointer((PVOID*)LdrpDispatchUserCallTarget, AsmLdrpDispatchUserCallTarget);

			return VirtualProtect((PVOID)LdrpDispatchUserCallTarget, 8, Protect, &Protect);
		}
	}
	// win11 21h2
	else if (version == 22000)
	{
		auto dxgi = (UINT64)GetModuleHandleA("dxgi.dll");
		// 拿到虚表
		DWORD64 DrawingContext = FindPatternImage(dxgi, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8D\x05\x00\x00\x00\x00", "xxxxxxxxxxxx????");

		if (DrawingContext)
		{
			DWORD Protect = 0;
			// 定位到第四个虚表地址
			DrawingContext += 0x29;
			// rv 
			DrawingContext = ResolveRelative(DrawingContext);
			// virtual table sixteen
			PDWORD_PTR Vtbl = PDWORD_PTR(DrawingContext);
#ifdef MODEL_ONE
			if (Vtbl[16])
			{
				// presentimpl adr
				DrawingContext = FindPattern(Vtbl[16], 0xFF, "\x20\xE8", "xx") + 1;
				// get call adr == impl
				DrawingContext = ResolveCall(DrawingContext);
				// __guard_xfg_dispatch_icall_fptr
				DrawingContext = FindPattern(DrawingContext, 0xFF, "\xFF\x15", "xx");
#ifdef OUTADR
				std::cout << std::hex << (DWORD64)DrawingContext << std::endl;
#endif
				// 拿到LdrpDispatchUserCallTarget的值
				PresentCall = DrawingContext + 6;
				// 拿到LdrpDispatchUserCallTarget要跳转到的地址 也就是call的地址
				DrawingContext += static_cast<unsigned long long>(+*(int*)(DrawingContext + 2)) + 6;
				// 修改这个call
				if (VirtualProtect((PVOID)DrawingContext, 8, PAGE_READWRITE, &Protect)) {

					_InterlockedExchangePointer((PVOID*)DrawingContext, AsmLdrpDispatchUserCallTarget_impl);
					VirtualProtect((PVOID)DrawingContext, 8, Protect, &Protect);
				}
			}
#endif
#ifdef MODEL_TWO
			if (Vtbl[23])
			{
				// __guard_xfg_dispatch_icall_fptr
				DrawingContext = FindPattern(Vtbl[23], 0xFF, "\xFF\x15", "xx");
#ifdef OUTADR
				std::cout << std::hex << (DWORD64)DrawingContext << std::endl;
#endif
				// 拿到LdrpDispatchUserCallTarget的值
				PresentCall = DrawingContext + 6;
				DrawingContext += static_cast<unsigned long long>(+*(int*)(DrawingContext + 2)) + 6;
				// 修改这个call
				if (VirtualProtect((PVOID)DrawingContext, 8, PAGE_READWRITE, &Protect)) {
					_InterlockedExchangePointer((PVOID*)DrawingContext, AsmLdrpDispatchUserCallTarget_impl);
					VirtualProtect((PVOID)DrawingContext, 8, Protect, &Protect);
				}
			}
#endif
			return true;
		}
	}
	// win11 22h2 - 23h2
	else if (version > 22000 && version < 26100)
	{
		auto dxgi = (UINT64)GetModuleHandleA("dxgi.dll");
		// 拿到虚表
		DWORD64 DrawingContext = FindPatternImage(dxgi, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8D\x05\x00\x00\x00\x00", "xxxxxxxxxxxx????");
		if (DrawingContext)
		{
			DWORD Protect = 0;
			// 定位到第四个虚表地址
			DrawingContext += 0x29;
			// rv 
			DrawingContext = ResolveRelative(DrawingContext);
			// virtual table sixteen
			PDWORD_PTR Vtbl = PDWORD_PTR(DrawingContext);
#ifdef MODEL_ONE
			if (Vtbl[16])
			{
				// presentimpl adr
				DrawingContext = FindPattern(Vtbl[16], 0xFF, "\x20\xE8", "xx") + 1;
				// get call adr == impl
				DrawingContext = ResolveCall(DrawingContext) - 0x100000000;
				// __guard_xfg_dispatch_icall_fptr
				DrawingContext = FindPattern(DrawingContext, 0xFF, "\xFF\x15", "xx");
#ifdef OUTADR
				std::cout << std::hex << (DWORD64)DrawingContext << std::endl;
#endif	
				// 拿到LdrpDispatchUserCallTarget的值
				PresentCall = DrawingContext + 6;
				// 拿到LdrpDispatchUserCallTarget要跳转到的地址 也就是call的地址
				DrawingContext += static_cast<unsigned long long>(+*(int*)(DrawingContext + 2)) + 6;
				// 修改这个call
				if (VirtualProtect((PVOID)DrawingContext, 8, PAGE_READWRITE, &Protect)) {

					_InterlockedExchangePointer((PVOID*)DrawingContext, AsmLdrpDispatchUserCallTarget_impl);
					VirtualProtect((PVOID)DrawingContext, 8, Protect, &Protect);
				}
			}
#endif
#ifdef MODEL_TWO
			if (Vtbl[23])
			{
				// __guard_xfg_dispatch_icall_fptr
				DrawingContext = FindPattern(Vtbl[23], 0xFF, "\xFF\x15", "xx");
#ifdef OUTADR
				std::cout << std::hex << (DWORD64)DrawingContext << std::endl;
#endif	
				// 拿到LdrpDispatchUserCallTarget的值
				PresentCall = DrawingContext + 6;
				DrawingContext += static_cast<unsigned long long>(+*(int*)(DrawingContext + 2)) + 6;
				// 修改这个call
				if (VirtualProtect((PVOID)DrawingContext, 8, PAGE_READWRITE, &Protect)) {
					_InterlockedExchangePointer((PVOID*)DrawingContext, AsmLdrpDispatchUserCallTarget_impl);
					VirtualProtect((PVOID)DrawingContext, 8, Protect, &Protect);
				}
			}
#endif
			return true;
		}
	}
	// win11 24h2
	else if (version == 26100)
	{
		auto dxgi = (UINT64)GetModuleHandleA("dxgi.dll");
		// 拿到虚表
		DWORD64 DrawingContext = FindPatternImage(dxgi, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8D\x05\x00\x00\x00\x00", "xxxxxxxxxxxx????");
		if (DrawingContext)
		{
			DWORD Protect = 0;
			// 定位到第四个虚表地址
			DrawingContext += 0x29;
			// rv 
			DrawingContext = ResolveRelative(DrawingContext);
			// virtual table sixteen
			PDWORD_PTR Vtbl = PDWORD_PTR(DrawingContext);
#ifdef MODEL_ONE
			if (Vtbl[16])
			{
				DWORD64 MEM_NONE = Search(dxgi, "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
				// presentimpl adr
				DrawingContext = FindPattern(Vtbl[16], 0xFF, "\x20\xE8", "xx") + 1;
				// get call adr == impl
				DrawingContext = ResolveCall(DrawingContext) - 0x100000000;
				// __guard_xfg_dispatch_icall_fptr
				DrawingContext = FindPattern(DrawingContext, 0xFF, "\x18\xE8", "xx") + 1;
#ifdef OUTADR
				std::cout << std::hex << (DWORD64)DrawingContext << std::endl;
#endif	
				// 拿到LdrpDispatchUserCallTarget的值
				PresentCall = DrawingContext + 6;
				// 构造shellcode
				BYTE jmpCode[18] = { 0x50,0x48,0xB8,0,0,0,0,0,0,0,0,0xFF,0xE0 };
				// 放入地址
				*reinterpret_cast<DWORD_PTR*>(&jmpCode[3]) = (DWORD64)&AsmLdrpDispatchUserCallTarget_impl_ex;
				// 注入shellcode
				if (VirtualProtect((PVOID)MEM_NONE, 8, PAGE_EXECUTE_READWRITE, &Protect)) {
					memcpy(((BYTE*)MEM_NONE), &jmpCode, sizeof(jmpCode));
				}

				// 再次构造shellcode
				BYTE CallCode[] = { 0xE8,0,0,0,0 };
				DWORD CALLADR = AbsoluteToRelative(DrawingContext, MEM_NONE);
				memcpy(&CallCode[1], &CALLADR, sizeof(CALLADR)); // 构造完毕

				// hook call
				if (VirtualProtect((PVOID)DrawingContext, 8, PAGE_EXECUTE_READWRITE, &Protect)) {
					memcpy(((BYTE*)DrawingContext), &CallCode, sizeof(CallCode)); // 写入shellcode
					VirtualProtect((PVOID)DrawingContext, 8, Protect, &Protect);
				}
			}
#endif
#ifdef MODEL_TWO
			if (Vtbl[23])
			{
				DWORD64 MEM_NONE2 = Search(dxgi, "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
				DrawingContext = FindPattern(Vtbl[23], 0xFF, "\x18\xE8", "xx") + 1;
#ifdef OUTADR
				std::cout << std::hex << (DWORD64)DrawingContext << std::endl;
#endif	
				// 拿到LdrpDispatchUserCallTarget的值
				PresentCall = DrawingContext + 6;
				// 构造shellcode
				BYTE jmpCode[18] = { 0x50,0x48,0xB8,0,0,0,0,0,0,0,0,0xFF,0xE0 };
				// 放入地址
				*reinterpret_cast<DWORD_PTR*>(&jmpCode[3]) = (DWORD64)&AsmLdrpDispatchUserCallTarget_impl_ex2;
				// 注入shellcode
				if (VirtualProtect((PVOID)MEM_NONE2, 8, PAGE_EXECUTE_READWRITE, &Protect)) {
					memcpy(((BYTE*)MEM_NONE2), &jmpCode, sizeof(jmpCode));
				}

				// 再次构造shellcode
				BYTE CallCode[] = { 0xE8,0,0,0,0 };
				DWORD CALLADR = AbsoluteToRelative(DrawingContext, MEM_NONE2);
				memcpy(&CallCode[1], &CALLADR, sizeof(CALLADR)); // 构造完毕
				// hook call
				if (VirtualProtect((PVOID)DrawingContext, 8, PAGE_EXECUTE_READWRITE, &Protect)) {
					memcpy(((BYTE*)DrawingContext), &CallCode, sizeof(CallCode)); // 写入shellcode
					VirtualProtect((PVOID)DrawingContext, 8, Protect, &Protect);
				}
			}
#endif	
		}
	}
#else

	//保存地址
	DWORD64 dwRender = 0;
#ifdef OUTADR
	std::cout << osversion.dwBuildNumber << std::endl;
#endif
	if (osversion.dwBuildNumber <= 19044)
	{
		auto dxgi = (UINT64)GetModuleHandleA("dxgi.dll");
		//48 8B C4 41 56 48 83 EC ?? 48 C7 40 ?? ?? ?? ?? ?? 48 89 58 ?? 48 89 68 ?? 48 89 78
		dwRender = FindPatternImage(dxgi,
			"\x48\x8B\xC4\x41\x56\x48\x83\xEC\x00\x48\xC7\x40\x00\x00\x00\x00\x00\x48\x89\x58\x00\x48\x89\x68\x00\x48\x89\x78",
			"xxxxxxxx?xxx?????xxx?xxx?xxx");
		if (dwRender)
		{
			// 定位到第四个虚表地址
			dwRender += 0x40;
			dwRender = ResolveRelative(dwRender);
			PDWORD_PTR Vtbl = PDWORD_PTR(dwRender);
#ifdef OUTADR
			std::cout << std::hex << dwRender << std::endl;
			std::cout << std::hex << PVOID(Vtbl[16]) << std::endl;
			std::cout << std::hex << PVOID(Vtbl[23]) << std::endl;
#endif

#ifdef VEHHOOK
			g_VEHHook.Init(LONGLONG(Vtbl[16]), (LONGLONG)Vtbl[23], (LONGLONG)&hkPresentDWM_dxgi, (LONGLONG)&hkPresentMPO_dxgi);
			oPresentDWM_dxgi_ = reinterpret_cast<PresentDWM_dxgi_>(Vtbl[16]);
			oPresentMPO_dxgi_ = reinterpret_cast<PresentMPO_dxgi_>(Vtbl[23]);
			g_VEHHook.Run_HOOK();
#else
			MH_CreateHook(PVOID(Vtbl[16]), PVOID(&hkPresentDWM_dxgi), reinterpret_cast<PVOID*>(&oPresentDWM_dxgi_));
			MH_CreateHook(PVOID(Vtbl[23]), PVOID(&hkPresentMPO_dxgi), reinterpret_cast<PVOID*>(&oPresentMPO_dxgi_));
			MH_EnableHook(MH_ALL_HOOKS);
#endif
		}
	}
	if (osversion.dwBuildNumber == 19045)
	{
		auto d2d1 = (UINT64)GetModuleHandleA("d2d1.dll");
		//48 8D 05 ?? ?? ?? ?? 33 ED 48 8D 71 08
		dwRender = FindPatternImage(d2d1, "\x48\x8D\x05\x00\x00\x00\x00\x33\xED\x48\x8D\x71\x08", "xxx????xxxxxx");
		if (dwRender)
		{
			dwRender = ResolveRelative(dwRender);

			PDWORD_PTR Vtbl = PDWORD_PTR(dwRender);
#ifdef OUTADR
			std::cout << std::hex << dwRender << std::endl;
			std::cout << std::hex << PVOID(Vtbl[6]) << std::endl;
			std::cout << std::hex << PVOID(Vtbl[7]) << std::endl;
#endif

#ifdef VEHHOOK
			g_VEHHook.Init(LONGLONG(Vtbl[6]), (LONGLONG)Vtbl[7], (LONGLONG)&hkPresentDWM, (LONGLONG)&hkPresentMPO);
			oPresentDWM = reinterpret_cast<PresentDWM_>(Vtbl[6]);
			oPresentMPO = reinterpret_cast<PresentMPO_>(Vtbl[7]);
			g_VEHHook.Run_HOOK();

#else
			MH_CreateHook(PVOID(Vtbl[6]), PVOID(&hkPresentDWM), reinterpret_cast<PVOID*>(&oPresentDWM));
			MH_CreateHook(PVOID(Vtbl[7]), PVOID(&hkPresentMPO), reinterpret_cast<PVOID*>(&oPresentMPO));
			MH_EnableHook(MH_ALL_HOOKS);
#endif
		}
	}
	if (osversion.dwBuildNumber >= 22000)
	{
		auto dxgi = (UINT64)GetModuleHandleA("dxgi.dll");
		//40 53 48 83 EC 20 48 8B D9 48 8D 05 ?? ?? ?? ??
		dwRender = FindPatternImage(dxgi,
			"\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8D\x05\x00\x00\x00\x00",
			"xxxxxxxxxxxx????");
		if (dwRender)
		{
			// 定位到第四个虚表地址
			dwRender += 0x29;
			dwRender = ResolveRelative(dwRender);
			PDWORD_PTR Vtbl = PDWORD_PTR(dwRender);
#ifdef OUTADR
			std::cout << std::hex << dwRender << std::endl;
			std::cout << std::hex << PVOID(Vtbl[16]) << std::endl;
			std::cout << std::hex << PVOID(Vtbl[23]) << std::endl;

#endif

#ifdef VEHHOOK
			g_VEHHook.Init(LONGLONG(Vtbl[16]), (LONGLONG)Vtbl[23], (LONGLONG)&hkPresentDWM_dxgi, (LONGLONG)&hkPresentMPO_dxgi);
			oPresentDWM_dxgi_ = reinterpret_cast<PresentDWM_dxgi_>(Vtbl[16]);
			oPresentMPO_dxgi_ = reinterpret_cast<PresentMPO_dxgi_>(Vtbl[23]);
			g_VEHHook.Run_HOOK();
#else
			MH_CreateHook(PVOID(Vtbl[16]), PVOID(&hkPresentDWM_dxgi), reinterpret_cast<PVOID*>(&oPresentDWM_dxgi_));
			MH_CreateHook(PVOID(Vtbl[23]), PVOID(&hkPresentMPO_dxgi), reinterpret_cast<PVOID*>(&oPresentMPO_dxgi_));
			MH_EnableHook(MH_ALL_HOOKS);
#endif
		}
	}
#endif
	return 0;
}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, PVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
#ifdef OUTADR
		AllocConsole();
		freopen("CONOUT$", "w+", stdout);
#endif
		_beginthreadex(nullptr, NULL, MainThread, nullptr, NULL, nullptr);
	}
	return true;
}