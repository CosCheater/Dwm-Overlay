#include "includes.hpp"
//定义这个宏则使用VEHHOOK 不定义则使用MinHook
#define VEHHOOK
//定义这个宏则输出地址
#define OUTADR
// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
RECT ProgramRect;
ImFont* Font;

bool debug = false;

BOOL bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
	{
		if (*szMask == 'x' && *pData != *bMask)
			return FALSE;
	}
	return (*szMask) == NULL;
}

DWORD64 FindPattern(const char* szModule, BYTE* bMask, const char* szMask)
{
	MODULEINFO mi{ };
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &mi, sizeof(mi));

	DWORD64 dwBaseAddress = DWORD64(mi.lpBaseOfDll);
	const auto dwModuleSize = mi.SizeOfImage;

	for (auto i = 0ul; i < dwModuleSize; i++)
	{
		if (bDataCompare(PBYTE(dwBaseAddress + i), bMask, szMask))
			return DWORD64(dwBaseAddress + i);
	}
	return NULL;
}

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

void DrawEverything(IDXGISwapChain* pDxgiSwapChain)
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
#ifdef VEHHOOK

#else
	MH_Initialize();
#endif
	while (!GetModuleHandleA("dwmcore.dll"))
		Sleep(150);

	// [ E8 ? ? ? ? ] the relative addr will be converted to absolute addr
	auto ResolveCall = [](DWORD_PTR sig)
		{
			return sig = sig + *reinterpret_cast<PULONG>(sig + 1) + 5;
		};

	//
	// [ 48 8D 05 ? ? ? ? ] the relative addr will be converted to absolute addr
	auto ResolveRelative = [](DWORD_PTR sig)
		{
			return sig = sig + *reinterpret_cast<PULONG>(sig + 0x3) + 0x7;
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
	//保存地址
	DWORD64 dwRender = 0;
#ifdef OUTADR
	std::cout << osversion.dwBuildNumber << std::endl;
#endif
	if (osversion.dwBuildNumber <= 19044)
	{
		//48 8B C4 41 56 48 83 EC ?? 48 C7 40 ?? ?? ?? ?? ?? 48 89 58 ?? 48 89 68 ?? 48 89 78
		dwRender = FindPattern("dxgi.dll",
			PBYTE("\x48\x8B\xC4\x41\x56\x48\x83\xEC\x00\x48\xC7\x40\x00\x00\x00\x00\x00\x48\x89\x58\x00\x48\x89\x68\x00\x48\x89\x78"),
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
		//48 8D 05 ?? ?? ?? ?? 33 ED 48 8D 71 08
		dwRender = FindPattern("d2d1.dll", PBYTE("\x48\x8D\x05\x00\x00\x00\x00\x33\xED\x48\x8D\x71\x08"), "xxx????xxxxxx");
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
		//40 53 48 83 EC 20 48 8B D9 48 8D 05 ?? ?? ?? ??
		dwRender = FindPattern("dxgi.dll",
			PBYTE("\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8D\x05\x00\x00\x00\x00"),
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