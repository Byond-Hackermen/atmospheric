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

char* BYOND::Variables::getCStringFromId(int id)
{
	return *getStringPointerFromId(id);
}

std::string BYOND::Variables::GetStringFromId(int id)
{
	return std::string(getCStringFromId(id));
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
	setVariable(type, datumId, BYONDSTR(varName), varType, *(void**)&new_value);
}
BYOND::List::List(int id) {
	ByondList* list = BYOND::Variables::getListPointer(id);
	this->internalList = list;
	this->ID = id;
}

BYOND::Object* BYOND::List::At(DWORD i)
{
	return internalList->At(i);
}

DWORD BYOND::List::Length()
{
	return internalList->length;
}

void BYOND::List::Append(VariableType type, DWORD value)
{
	Variables::appendToContainer(VariableType::List, ID, type, (void*)value);
}

void BYOND::List::Append(VariableType type, float value)
{
	Variables::appendToContainer(VariableType::List, ID, type, *(void**)&value);
}
BYOND::List* BYOND::Variables::GetListFromId(int id)
{
	List* newlist = new List(id);
	return newlist;
}


void BYOND::Variables::GenerateStringTable()
{
	char* current_char = getCStringFromId(1);
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

BYOND::Variables::SetVariablePtr* BYOND::Variables::setVariable = nullptr;
BYOND::Variables::GetVariablePtr* BYOND::Variables::getVariable = nullptr;
BYOND::Variables::GetStringPointerFromIdPtr* BYOND::Variables::getStringPointerFromId = nullptr;
BYOND::Variables::GetListPointerPtr* BYOND::Variables::getListPointer = nullptr;
BYOND::Variables::AppendToContainerPtr* BYOND::Variables::appendToContainer = nullptr;

bool BYOND::Variables::GetFunctionPointers()
{
	HMODULE byondCore = GetModuleHandleA("byondcore.dll");
	MODULEINFO mod_info;
	GetModuleInformation(GetCurrentProcess(), byondCore, &mod_info, sizeof(mod_info));
	setVariable = (SetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 57 8B 7D 10 83 F8");
	getVariable = (GetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 83 F8 53 0F 87 F1");
	getStringPointerFromId = (GetStringPointerFromIdPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 3B 0D D0 17 3E 73 73 10 A1 CC 17 3E 73 8B 04 88 85 C0 0F 85 87 00 00 00 83 3D A4 29 3E 73 00");
	getListPointer = (GetListPointerPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 3B 0D 84 18 3E 73 73 11 A1 80 18 3E 73 8B 04 88 85 C0 74 05 FF 40 10");
	appendToContainer = (AppendToContainerPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 56 83 F8 53 0F 87 B1 00 00 00 0F B6 80 DC 11 1E 73 FF 24 85 90 11 1E 73 FF 75 0C");
	if (!setVariable || !getVariable || !getStringPointerFromId || !getListPointer || !appendToContainer)
	{
		return false;
	}
	return true;
}
