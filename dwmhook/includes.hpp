#pragma once

#include <Windows.h>
#include <cstdio>
#include <Psapi.h>
#include <process.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3d11_1.h>
#include <intrin.h>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <d2d1_3.h>
#include <d3d11_2.h>
#include <d2d1_3helper.h>
#include <dwrite_3.h>
#include <dcomp.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/fonts.h"
#include "VEHhook/VEHhook.h"
#include "Util/Utils.h"
#include "CALLJMP/AsmHook.h"
using namespace DirectX;

#pragma comment( lib, "dxgi" )
#pragma comment( lib, "d2d1" )
#pragma comment( lib, "d3d11" )
#pragma comment( lib, "dcomp" )
#pragma comment( lib, "dwrite" )


// 定义这个宏则使用VEHHOOK 
// #define VEHHOOK
// 定义这个宏使用MiniHook
// #define MINHOOK
// 定义这个宏 使用 call point替换
#define CALLJMP
// 这个CALLJMP 两种模式 第一个是虚拟机模式 第二个是主机模式 切记选择CALLJMP模式一定要定义下面的宏
// #define MODEL_ONE
#define MODEL_TWO
// 定义这个宏则输出地址
// #define OUTADR

#ifdef MINHOOK
#ifdef _DEBUG
#include "MinHook.h"
#pragma comment( lib, "libMinHook.x64.lib" )
#else
#pragma comment( lib, "libMinHook.x64.lib" )
#endif
#endif // MINHOOK

#pragma comment( lib, "d3dcompiler.lib" )
#define SAFE_RELEASE( p )	if( p ) { p->Release(); p = nullptr; }
