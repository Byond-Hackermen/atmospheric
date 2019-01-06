#include "byond.h"

#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else

#endif


std::string BYOND::GetByondAbout()
{
	std::ostringstream ss;

	ss << BYOND::GetByondVersion() << "." << BYOND::GetByondBuild();

	return ss.str();
}

int BYOND::GetByondVersion()
{
#ifdef _WIN32
	typedef int(*GetByondVersion)(void);
	static auto getByondVersion = reinterpret_cast<GetByondVersion>(GetProcAddress(GetModuleHandleA("byondcore.dll"), "?GetByondVersion@ByondLib@@QAEJXZ"));

	if (!getByondVersion)
		return 0;

	return getByondVersion();
#else
	return 0; // TODO
#endif
}

int BYOND::GetByondBuild()
{
#ifdef _WIN32
	typedef int (*GetByondBuild)(void);
	static auto getByondBuild = reinterpret_cast<GetByondBuild>(GetProcAddress(GetModuleHandleA("byondcore.dll"), "?GetByondBuild@ByondLib@@QAEJXZ"));

	if (!getByondBuild)
		return 0;

	return getByondBuild();
#else
	return 0; // TODO
#endif
}

