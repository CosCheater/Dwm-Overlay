#pragma once
#include <windows.h>
#include <Psapi.h>
#include <vector>
#include <string>

BOOL PatternCheck(PCSTR data, PCSTR pattern, PCSTR mask);

UINT64 FindPattern(UINT64 address, UINT64 size, PCSTR pattern, PCSTR mask);

UINT64 FindPatternImage(UINT64 ImageBase, PCSTR Pattern, PCSTR Mask);

DWORD64 Search(DWORD64 Module, std::string Feature, DWORD64 StartOffset = 0, DWORD64 EndOffset = 0xFFFFFFFF);