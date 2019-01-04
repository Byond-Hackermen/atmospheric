#pragma once
#include <Windows.h>
#include <TlHelp32.h>
namespace Pocket {
	const wchar_t * GetWC(const char * c);
	HANDLE GetProcessByName(const char* name);
}