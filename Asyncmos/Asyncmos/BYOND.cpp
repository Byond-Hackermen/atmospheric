#include "stdafx.h"
#include "BYOND.h"
#include "Pocket/Utilities.h"

std::map<std::string, int> BYOND::Variables::stringTable;

BYOND::Variables::Variables()
{
}
BYOND::Variables::~Variables()
{
}

char* BYOND::Variables::getStringFromId(int id)
{
	return *getStringPointerFromId(id);
}

int BYOND::Variables::ReadVariable(ObjectType type, int datumId, std::string varName)
{
	return 0xDEADB33F;
}
void BYOND::Variables::SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, DWORD new_value)
{
	setVariable(type, datumId, BYONDSTR(varName), varType, (void*)new_value);
}

void BYOND::Variables::SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, float new_value)
{
	setVariable(type, datumId, BYONDSTR(varName), varType, (void*)*(uint32_t*)&new_value);
}


void BYOND::Variables::GenerateStringTable()
{
	char* current_char = getStringFromId(1);
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
			stringTable[current_name] = current_string_id;
			if (*(current_char + 1) == 0x00) {
				break;
			}
			current_name = "";
			current_string_id++;
		}
		current_char++;
	}
}

void BYOND::Variables::GetFunctionPointers()
{
	HMODULE byondCore = GetModuleHandleA("byondcore.dll");
	MODULEINFO mod_info;
	GetModuleInformation(GetCurrentProcess(), byondCore, &mod_info, sizeof(mod_info));
	setVariable = (SetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 57 8B 7D 10 83 F8");
	getVariable = (GetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 83 F8 53 0F 87 F1");
	getStringPointerFromId = (GetStringPointerFromIdPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 3B 0D D0 17 3E 73 73 10 A1 CC 17 3E 73 8B 04 88 85 C0 0F 85 87 00 00 00 83 3D A4 29 3E 73 00");
		
}
