#include "errorhandling.h"

#ifdef _WIN32
	#include <Windows.h>
	#define IsValid( pointer )	( pointer && pointer != 0 && HIWORD( pointer ) )
#else
	#define IsValid( pointer )	( pointer && pointer != 0 )
#endif

bool Pocket::ErrorHandling::IsValidPtr(void* ptr)
{
	return IsValid(ptr);
}
