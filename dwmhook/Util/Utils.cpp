#include "Utils.h"

BOOL PatternCheck(PCSTR data, PCSTR pattern, PCSTR mask) {
    
    auto len = strlen(mask);

    for (auto i = 0ui64; i < len; i++)  {

        if (data[i] == pattern[i] || mask[i] == '?') continue; else return FALSE;
    }
    return TRUE;
}

UINT64 FindPattern(UINT64 address, UINT64 size, PCSTR pattern, PCSTR mask) {

    size -= strlen(mask);

    for (auto i = 0ui64; i < size; i++) if (PatternCheck((PCSTR)address + i, pattern, mask)) return address + i;

    return 0;
}

UINT64 FindPatternImage(UINT64 ImageBase, PCSTR Pattern, PCSTR Mask) {

    auto DosHeader = (IMAGE_DOS_HEADER*)ImageBase;

    auto NtHeaders = (PIMAGE_NT_HEADERS64)(ImageBase + DosHeader->e_lfanew);

    auto Section = IMAGE_FIRST_SECTION(NtHeaders);

    for (auto i = 0; i < NtHeaders->FileHeader.NumberOfSections; i++) {

        if (!strcmp((PCSTR)Section[i].Name, ".text")) {

            return FindPattern(ImageBase + Section[i].VirtualAddress, Section[i].Misc.VirtualSize, Pattern, Mask);
        }
    }
    return 0;
}

DWORD64 Search(DWORD64 Module, std::string Feature, DWORD64 StartOffset, DWORD64 EndOffset) 
{

	// È¥¿Õ
	for (int i = 0; i < Feature.length(); i++)
	{
		if (Feature[i] == ' ')
			Feature.replace(i, 1, "");
	}

	if (Feature.size() < 2)
		return 0;

	if (Feature.size() % 2 != 0)
		return 0;

	std::vector<std::pair<bool, byte>> Bytes;

	// ×Ö½Ú¡¢ÑÚÂëÉú³É
	for (int i = 0; i < Feature.length(); i += 2)
	{
		if (Feature.substr(i, 2) == "??")
			Bytes.push_back(std::make_pair<bool, byte>(false, 0x0));
		else
			Bytes.push_back(std::make_pair<bool, byte>(true, static_cast<byte>(std::stol(Feature.substr(i, 2), 0, 16))));
	}

	// ËÑË÷Æ¥Åä
	for (DWORD64 Offset = StartOffset; Offset < EndOffset; Offset++)
	{
		//if (!IsBadHugeReadPtr(reinterpret_cast<PVOID>(Module + Offset), 1))
		{
			byte* pTempByte = reinterpret_cast<byte*>(Module + Offset);
			int ProofNum = 0;

			for (auto SingleByte : Bytes)
			{
				if (SingleByte.first && SingleByte.second != *pTempByte)
					break;
				pTempByte++;
				ProofNum++;
			}

			if (ProofNum == Bytes.size() && *pTempByte)
				return Module + Offset;
		}
	}
	return NULL;

}