#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#define INRANGE(x,a,b)	(x >= a && x <= b) 
#define getBits( x )	(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )	(getBits(x[0]) << 4 | getBits(x[1]))
namespace Pocket {
	const wchar_t * GetWC(const char * c);
	HANDLE GetProcessByName(const char* name);
	DWORD FindPattern(DWORD rangeStart, DWORD rangeEnd, const char * pattern);
}