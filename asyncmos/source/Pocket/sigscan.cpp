#include "sigscan.h"



#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#else
// lol
#endif

bool Pocket::Sigscan::DataCompare(const char* base, const char* pattern)
{
	for (; *pattern; ++base, ++pattern)
		if (*pattern != '?' && *base != *pattern)
			return false;

	return *pattern == 0;
}

void* Pocket::Sigscan::FindPattern(std::uintptr_t address, size_t size, const char* pattern, const short offset)
{
	for (size_t i = 0; i < size; ++i, ++address)
		if (DataCompare(reinterpret_cast<const char*>(address), pattern))
			return reinterpret_cast<void*>(address + offset);

	return nullptr;
}

void* Pocket::Sigscan::FindPattern(const char* moduleName, const char* pattern, const short offset)
{
	const auto rangeStart = reinterpret_cast<DWORD>(GetModuleHandleA(moduleName));
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(rangeStart), &miModInfo, sizeof(MODULEINFO));

	return FindPattern(rangeStart, miModInfo.SizeOfImage, pattern, offset);
}
