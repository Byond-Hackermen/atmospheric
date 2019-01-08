#include "variables.h"

#include <windows.h>
#include <psapi.h>
#include <algorithm>

#include "../pocket/utilities.h"
#include "list.h"

#pragma comment( lib, "psapi.lib" )

BYOND::Variables vars;

std::map<std::string, int> BYOND::Variables::stringTable;

BYOND::Variables::SetVariablePtr*				BYOND::Variables::setVariable = nullptr;
BYOND::Variables::GetVariablePtr*				BYOND::Variables::getVariable = nullptr;
BYOND::Variables::GetStringPointerFromIdPtr*	BYOND::Variables::getStringPointerFromId = nullptr;
BYOND::Variables::GetListPointerPtr*			BYOND::Variables::getListPointer = nullptr;
BYOND::Variables::AppendToContainerPtr*			BYOND::Variables::appendToContainer = nullptr;
BYOND::Variables::RemoveFromContainerPtr*		BYOND::Variables::removeFromContainer = nullptr;
BYOND::Variables::ReadVariablePtr*				BYOND::Variables::readVariable = nullptr;
BYOND::Variables::CallProcPtr*					BYOND::Variables::callProc = nullptr;



bool BYOND::Variables::Initialize()
{
	if (!GetFunctionPointers())
		return false;

	if (!HookGlobalTimer())
		return false;

	GenerateStringTable();
	init_done = true;

	return true;
}

void __stdcall GlobalTimerStartHook(int _this, int a1, int a2, int a3, int a4)
{
	return;
}

bool BYOND::Variables::HookGlobalTimer() //gonna need a function to unhook and clean up
{
	NTSTATUS result = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("byondcore")), "?Start@GlobalTimer@@QAEXUtimeval@@EP6AXPAU1@@ZPAX@Z"),
		GlobalTimerStartHook,
		NULL,
		&globalTimerHookInfo);

	if (FAILED(result))
	{
		MessageBoxA(nullptr, "Failed to install GlobalTimer::Start hook!", "oh crap!", 0);
		return false;
	}
	return true;
}


void BYOND::Variables::GenerateStringTable() const
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


bool BYOND::Variables::GetFunctionPointers() const
{
	const auto rangeStart = reinterpret_cast<DWORD>(GetModuleHandleA("byondcore.dll"));
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(rangeStart), &miModInfo, sizeof(MODULEINFO));

	if (!Pocket::GetFunction<SetVariablePtr*>(setVariable, rangeStart, miModInfo.SizeOfImage, "55 8B EC 8B 4D 08 0F B6 C1 48 57 8B 7D 10 83 F8 53 0F ?? ?? ?? ?? ?? 0F B6 80 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? FF 75 18 FF 75 14 57 FF 75 0C E8 ?? ?? ?? ?? 83 C4 10 5F 5D C3"))
	{
		MessageBoxA(nullptr, "Failed to acquire setVariable", "oh no!", 0);
		return false;
	}

	  
	if (!Pocket::GetFunction<GetVariablePtr*>(getVariable, rangeStart, miModInfo.SizeOfImage, "55 8B EC 8B 4D 08 0F B6 C1 48 83 F8 53 0F 87 ?? ?? ?? ?? 0F B6 80 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? FF 75 10"))
	{
		MessageBoxA(nullptr, "Failed to acquire getVariable", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<GetStringPointerFromIdPtr*>(getStringPointerFromId, rangeStart, miModInfo.SizeOfImage, "55 8B EC 8B 4D 08 3B 0D ?? ?? ?? ?? 73 10 A1"))
	{
		MessageBoxA(nullptr, "Failed to acquire getStringPointer", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<GetListPointerPtr*>(getListPointer, rangeStart, miModInfo.SizeOfImage, "55 8B EC 8B 4D 08 3B 0D ?? ?? ?? ?? 73 11 A1 ?? ?? ?? ?? 8B 04 88 85 C0 74 05 FF 40 10 5D C3 6A 0F 51 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 52 50 E8 ?? ?? ?? ?? 83 C4 14 5D C3"))
	{
		MessageBoxA(nullptr, "Failed to acquire getListPointer", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<AppendToContainerPtr*>(appendToContainer, rangeStart, miModInfo.SizeOfImage, "55 8B EC 8B 4D 08 0F B6 C1 48 56"))
	{
		MessageBoxA(nullptr, "Failed to acquire appendToContainer", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<RemoveFromContainerPtr*>(removeFromContainer, rangeStart, miModInfo.SizeOfImage, "55 8B EC 8B 4D 08 83 EC 0C 0F B6 C1 48 53"))
	{
		MessageBoxA(nullptr, "Failed to acquire removeFromContainer", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<ReadVariablePtr*>(readVariable, rangeStart, miModInfo.SizeOfImage, "55 8B EC 8B 4D 08 0F B6 C1 48 83 F8 53 0F 87 F1 00 00 00 0F B6 80 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? FF 75 10 FF 75 0C E8 ?? ?? ?? ?? 83 C4 08 5D C3"))
	{
		MessageBoxA(nullptr, "Failed to acquire readVariable", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<CallProcPtr*>(callProc, rangeStart, miModInfo.SizeOfImage, "55 8B EC 83 EC 0C 53 8B 5D 10 8D 45 FF 56 8B 75 14 57 6A 01 50 FF 75 1C C6 45 FF 00 FF 75 18 6A 00 56 53"))
	{
		MessageBoxA(nullptr, "Failed to acquire callProc", "oh no!", 0);
		return false;
	}
	// One of these is right (maybe), not needed as of now.
	//mob_list = (DWORD*)**(DWORD**)(*(int*)((BYTE*)readVariable + 57 + *(int*)((BYTE*)readVariable + 40)) + (DWORD)((BYTE*)readVariable + 57 + *(int*)((BYTE*)readVariable + 40)) + 23); //OH GOD OH FUCK
	//mob_list = Pocket::Sigscan::FindPattern(rangeStart, miModInfo.SizeOfImage, "A1 ?? ?? ?? ?? 8B 04 B0 85 C0 74 34 FF B0", 1);

	return true;
}


BYOND::Object BYOND::Variables::ReadVariable(ObjectType type, int datumId, const std::string varName)
{
	VariableType varType;
	unsigned int varValue;
	int varNameId = BYONDSTR(varName);
	ObjectType differentlyNamedType = type;
	//readVariable(type, datumId, BYONDSTR(varName));
	__asm {
		push varNameId
		push datumId
		push differentlyNamedType
		call readVariable
		add esp, 12
		mov varType, eax
		mov varValue, edx
	}
	Object obj{};
	obj.type = static_cast<VariableType>(varType);
	if (obj.type == BYOND::VariableType::Number) {
		obj.value = *reinterpret_cast<void**>(&varValue);
	}
	else
	{
		obj.value = reinterpret_cast<void*>(varValue);
	}

	return obj;
}

BYOND::Object BYOND::Variables::ReadVariable(BYOND::Object obj, const std::string varName)
{
	return ReadVariable(static_cast<ObjectType>(obj.Type()), reinterpret_cast<int>(obj.value), varName);
}

BYOND::Object BYOND::Variables::ReadWorldVariable(std::string name)
{
	return ReadVariable(BYOND::ObjectType::World, 0, name);
}

BYOND::Object BYOND::Variables::ReadGlobalVariable(std::string name)
{
	return ReadVariable(BYOND::ObjectType::Global, 0xDEADC0DE, name);
}


void BYOND::Variables::SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, int new_value) const
{
	setVariable(type, datumId, BYONDSTR(varName), varType, reinterpret_cast<void*>(new_value));
}

void BYOND::Variables::SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, float new_value) const
{
	setVariable(type, datumId, BYONDSTR(varName), varType, *reinterpret_cast<void**>(&new_value));
}

void BYOND::Variables::CallObjectProc(Object obj, std::string procName, std::vector<Object> arguments)
{
	std::replace(procName.begin(), procName.end(), '_', ' ');
	ULONG ACLEntries[1] = { 0 };
	LhSetInclusiveACL(ACLEntries, 1, &globalTimerHookInfo);
	callProc(0, 0, ProcType::Proc, BYONDSTR(procName), static_cast<ObjectType>(obj.Type()), reinterpret_cast<int>(obj.value), arguments.data(), arguments.size(), 0, 0);
}

void BYOND::Variables::CallObjectProc(Object obj, std::string procName)
{
	CallObjectProc(obj, procName, std::vector<Object>());
}

std::string BYOND::Variables::GetStringFromId(int id) const
{
	return std::string(GetCStringFromId(id));
}

BYOND::List* BYOND::Variables::GetListFromId(int id) const
{
	BYOND::List* newlist = new BYOND::List(id);
	return newlist;
}

char* BYOND::Variables::GetCStringFromId(int id) const
{
	return *getStringPointerFromId(id);
}
