#include "stdafx.h"
#include "BYOND.h"
#include "Pocket/Utilities.h"

BYOND::Variables::Variables()
{
}
BYOND::Variables::~Variables()
{
}

int BYOND::Variables::ReadVariable(char type, int datumId, int varNameId)
{
	return 0xDEADB33F;
}
void BYOND::Variables::SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, DWORD new_value)
{
	setVariable(type, datumId, BYONDSTR(varName), varType, (float)(int)new_value);
}


void BYOND::Variables::GenerateStringTable()
{
	char* current_char = (char*)Pocket::FindPattern(0x00700000, 0x00A00000, "45 78 69 74 00 45 6E 74 65 72 00 4E 65 77 00"); //CHANGE 0x007000000 - WONT CATCH ALL POSSIBLE ADDRESSES
	int current_string_id = 1;
	std::string current_name;
	while (true)
	{
		if (*current_char != 0x00)
		{
			current_name += *current_char;
		}
		else
		{
			StringTable[current_name] = current_string_id;
			if (*(current_char + 1) == 0x00) {
				break;
			}
			current_name = "";
			current_string_id++;
		}
		current_char++;
	}
}

void BYOND::Variables::get_function_pointers()
{
	HMODULE byondCore = GetModuleHandleA("byondcore.dll");
	MODULEINFO mod_info;
	GetModuleInformation(GetCurrentProcess(), byondCore, &mod_info, sizeof(mod_info));
	setVariable = (SetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 57 8B 7D 10 83 F8");
}
