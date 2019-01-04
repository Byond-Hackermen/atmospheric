#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdlib>
#include <string>
#include <sstream>

namespace Pocket {
	const wchar_t * GetWC(const char * c);
	HANDLE GetProcessByName(const char* name);
	DWORD FindPattern(DWORD rangeStart, DWORD rangeEnd, const char * pattern);
	std::string IntegerToStrHex(int i);
	float DwordToFloat(DWORD i);
}