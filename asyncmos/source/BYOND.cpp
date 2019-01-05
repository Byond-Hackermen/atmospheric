#include "BYOND.h"
#include "Pocket/Utilities.h"

std::map<std::string, int> BYOND::Variables::stringTable;

BYOND::Variables::Variables()
{
}
BYOND::Variables::~Variables()
{
}

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
	setVariable = (SetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 57");
	getVariable = (GetVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 83 F8 53 0F 87 ? ? ? ? 0F B6 80 ? ? ? ? FF 24 85 ? ? ? ? FF 75 10");
	getStringPointerFromId = (GetStringPointerFromIdPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 3B 0D ? ? ? ? 73 10");
	getListPointer = (GetListPointerPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 3B 0D ? ? ? ? 73 11 A1 ? ? ? ? 8B 04 88 85 C0 74 05 FF 40 10 5D C3 6A 0F");
	appendToContainer = (AppendToContainerPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 56");
	removeFromContainer = (RemoveFromContainerPtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 83 EC 0C 0F B6 C1 48 53");
	readVariable = (ReadVariablePtr*)Pocket::FindPattern((DWORD)byondCore, (DWORD)byondCore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 83 F8 53 0F 87 ? ? ? ? 0F B6 80 ? ? ? ? FF 24 85 ? ? ? ? FF 75 10");
	if (!setVariable || !getVariable || !getStringPointerFromId || !getListPointer || !appendToContainer || !removeFromContainer || !readVariable)
	{
		return false;
	}
	mob_list = (DWORD*)**(DWORD**)(*(int*)((BYTE*)readVariable + 57 + *(int*)((BYTE*)readVariable + 40)) + (DWORD)((BYTE*)readVariable + 57 + *(int*)((BYTE*)readVariable + 40)) + 23); //OH GOD OH FUCK
	return true;
}
