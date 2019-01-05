#include "Utilities.h"

#include <sstream>

#define INRANGE(x,a,b)	(x >= a && x <= b) 
#define getBits( x )	(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )	(getBits(x[0]) << 4 | getBits(x[1]))


const wchar_t* Pocket::GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	//std::mbstowcs_s(wc, c, cSize); //lol

	return wc;
}

std::string Pocket::IntegerToStrHex(int i) {
	std::stringstream ss;
	ss << std::hex << i;
	return ss.str();
}

float Pocket::DwordToFloat(DWORD i)
{
	union {
		DWORD w;
		float f;
	} wordfloat;
	wordfloat.w = i;
	return wordfloat.f;
}
