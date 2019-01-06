#include "byond.h"

#include <sstream>
#include "../pocket/utilities.h"


std::string BYOND::GetByondAbout()
{
	std::ostringstream ss;

	ss << BYOND::GetByondVersion() << "." << BYOND::GetByondBuild();

	return ss.str();
}

int BYOND::GetByondVersion()
{
	typedef int(*GetByondVersion)(void);
#ifdef _WIN32
	static GetByondVersion getByondVersion;
	if(Pocket::ErrorHandling::IsValidPtr(getByondVersion) || Pocket::GetExport<GetByondVersion>(getByondVersion, "byondcore.dll", "?GetByondVersion@ByondLib@@QAEJXZ"))
		return getByondVersion();

	return 0;
#else
	return 0; // TODO
#endif
}

int BYOND::GetByondBuild()
{
	typedef int(*GetByondBuild)(void);
#ifdef _WIN32
	static GetByondBuild getByondBuild;
	if (Pocket::ErrorHandling::IsValidPtr(getByondBuild) || Pocket::GetExport<GetByondBuild>(getByondBuild, "byondcore.dll", "?GetByondBuild@ByondLib@@QAEJXZ"))
		return getByondBuild();

	return 0;
#else
	return 0; // TODO
#endif
}

