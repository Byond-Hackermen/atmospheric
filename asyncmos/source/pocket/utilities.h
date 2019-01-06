#pragma once

#include <string>
#include "sigscan.h"
#include "errorhandling.h"

#ifdef _WIN32
#include <Windows.h>
#else

#endif

using DWORD = unsigned long;

namespace Pocket
{
	const wchar_t* GetWC(const char* c);
	std::string IntegerToStrHex(int i);
	float DwordToFloat(DWORD i);

	template<typename I>
	bool GetFunction(I& out, std::string moduleName, std::string pattern)
	{
		void* temp = Sigscan::FindPattern(moduleName.c_str(), pattern.c_str());
		if (ErrorHandling::IsValidPtr(temp))
		{
			out = reinterpret_cast<I>(temp);
			return true;
		}

		return false;
	}

	template<typename I>
	bool GetFunction(I& out, std::uintptr_t address, std::uintptr_t size, std::string pattern)
	{
		void* temp = Sigscan::FindPattern(address, size, pattern.c_str());
		if (ErrorHandling::IsValidPtr(temp))
		{
			out = reinterpret_cast<I>(temp);
			return true;
		}

		return false;
	}

	template<typename I>
	bool GetExport(I& out, std::string moduleName, std::string exportName)
	{
#ifdef _WIN32
		void* temp = GetProcAddress(GetModuleHandleA(moduleName.c_str()), exportName.c_str());
		if (ErrorHandling::IsValidPtr(temp))
		{
			out = reinterpret_cast<I>(temp);
			return true;
		}

		return false;
#else
		return false;
#endif
	}
}