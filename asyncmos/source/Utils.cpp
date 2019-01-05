#include "Utils.h"

#define INRANGE(x,a,b)	(x >= a && x <= b) 
#define getBits( x )	(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )	(getBits(x[0]) << 4 | getBits(x[1]))

DWORD findPattern(DWORD rangeStart, DWORD rangeEnd, const char* pattern)
{
	const char* pat = pattern;
	DWORD firstMatch = 0;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		__try {
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;
			}
			else {
				pat = pattern;
				firstMatch = 0;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { }
	}
	return NULL;
}