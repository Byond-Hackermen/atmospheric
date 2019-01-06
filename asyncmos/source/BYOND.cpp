#include "BYOND.h"
#include "Pocket/Utilities.h"

std::map<std::string, int> BYOND::Variables::stringTable;

char* BYOND::Variables::GetCStringFromId(int id)
{
	return *getStringPointerFromId(id);
}

std::string BYOND::Variables::GetStringFromId(int id)
{
	return std::string(GetCStringFromId(id));
}

BYOND::Object BYOND::Variables::ReadVariable(ObjectType type, int datumId, std::string varName)
{
	readVariable(type, datumId, BYONDSTR(varName));
	VariableType varType;
	DWORD varValue;
	__asm {
		mov varType, eax
		mov varValue, edx
		}
	Object obj;
	obj.type = (VariableType)(BOOL)varType;
	if (obj.type == BYOND::VariableType::Number) {
		obj.value = *(void**)&varValue;
	}
	else
	{
		obj.value = (void*)varValue;
	}
	return obj;
}

BYOND::Object BYOND::Variables::ReadWorldVariable(std::string name)
{
	return ReadVariable(BYOND::ObjectType::World, 0, name);
}

BYOND::Object BYOND::Variables::ReadGlobalVariable(std::string name)
{
	return ReadVariable(BYOND::ObjectType::Global, 0xDEADC0DE, name);
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

void BYOND::List::Remove(VariableType type, DWORD value)
{
	Variables::removeFromContainer(VariableType::List, ID, type, (void*)value);
}

void BYOND::List::Remove(VariableType type, float value)
{
	Variables::removeFromContainer(VariableType::List, ID, type, *(void**)&value);
}

BYOND::List* BYOND::Variables::GetListFromId(int id)
{
	List* newlist = new List(id);
	return newlist;
}


void BYOND::Variables::GenerateStringTable()
{
	char* current_char = GetCStringFromId(1);
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
BYOND::Variables::RemoveFromContainerPtr* BYOND::Variables::removeFromContainer = nullptr;
BYOND::Variables::ReadVariablePtr* BYOND::Variables::readVariable = nullptr;

bool BYOND::Variables::GetFunctionPointers()
{
	HMODULE byondCore = GetModuleHandleA("byondcore.dll");
	MODULEINFO mod_info;
	GetModuleInformation(GetCurrentProcess(), byondCore, &mod_info, sizeof(mod_info));
	setVariable = (SetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 57 8B 7D 10 83 F8");
	getVariable = (GetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 83 F8 53 0F 87 F1");
	getStringPointerFromId = (GetStringPointerFromIdPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 3B 0D ?? ?? ?? ?? 73 10 A1 ?? ?? ?? ?? 8B 04 88 85 C0 0F 85 87 00 00 00 83 3D ?? ?? ?? ?? 00");
	getListPointer = (GetListPointerPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 3B 0D ?? ?? ?? ?? 73 11 A1 ?? ?? ?? ?? 8B 04 88 85 C0 74 05 FF 40 10");
	appendToContainer = (AppendToContainerPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 56 83 F8 53 0F 87 B1 00 00 00 0F B6 80 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? FF 75 0C");
	removeFromContainer = (RemoveFromContainerPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 83 EC 0C 0F B6 C1 48 53 83 F8 53 0F 87 2D 01 00 00 0F B6 80 ?? ?? ?? ?? 8B 55 10 FF 24 85");
	readVariable = (ReadVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 83 F8 53 0F 87 F1 00 00 00 0F B6 80 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? FF 75 10 FF 75 0C E8 ?? ?? ?? ?? 83 C4 08 5D C3");
	if (!setVariable || !getVariable || !getStringPointerFromId || !getListPointer || !appendToContainer || !removeFromContainer || !readVariable)
	{
		return false;
	}
	mob_list = (DWORD*)**(DWORD**)(*(int*)((BYTE*)readVariable + 57 + *(int*)((BYTE*)readVariable + 40)) + (DWORD)((BYTE*)readVariable + 57 + *(int*)((BYTE*)readVariable + 40)) + 23); //OH GOD OH FUCK
	return true;
}
