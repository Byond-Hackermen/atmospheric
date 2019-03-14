#include "variables.h"
#include "object_types.h"

#include <windows.h>
#include <psapi.h>
#include <algorithm>

#include "../pocket/utilities.h"
#include "list.h"
#include "bstring.h"

#pragma comment( lib, "psapi.lib" )

#define exception(x) MessageBoxA(NULL, x, "Exception!", NULL);

BYOND::Variables vars;

BYOND::Variables::SetVariablePtr*				BYOND::Variables::setVariable = nullptr;
BYOND::Variables::GetVariablePtr*				BYOND::Variables::getVariable = nullptr;
BYOND::Variables::GetStringPointerFromIdPtr*	BYOND::Variables::getStringPointerFromId = nullptr;
BYOND::Variables::GetListPointerPtr*			BYOND::Variables::getListPointer = nullptr;
BYOND::Variables::AppendToContainerPtr*			BYOND::Variables::appendToContainer = nullptr;
BYOND::Variables::RemoveFromContainerPtr*		BYOND::Variables::removeFromContainer = nullptr;
BYOND::Variables::ReadVariablePtr*				BYOND::Variables::readVariable = nullptr;
BYOND::Variables::CallProcPtr*					BYOND::Variables::callProc = nullptr;
BYOND::Variables::GetContainerItemPtr*			BYOND::Variables::getContainerItem = nullptr;
BYOND::Variables::GetStringTableIndexPtr*		BYOND::Variables::getStringTableIndex = nullptr;
BYOND::Variables::Text2PathPtr*					BYOND::Variables::text2path = nullptr;
BYOND::Variables::CallGlobalProcPtr*			BYOND::Variables::callGlobalProc = nullptr;
BYOND::Variables::CreateNewDatumObjectPtr*		BYOND::Variables::createNewDatumObject = nullptr;

BYOND::DatumObject BYOND::Variables::world;
std::recursive_mutex BYOND::Variables::callglobalproc_mutex;
std::recursive_mutex BYOND::Variables::callproc_mutex;

bool BYOND::Variables::Initialize()
{
	if (!GetFunctionPointers())
		return false;

	if (!HookGlobalTimer())
		return false;

	if (!MakeProcCallThreadsafe())
		return false;

	if (!HookObjectProcCalls())
		return false;

	world = BYOND::DatumObject(BYOND::ObjectType::World, 0);
	init_done = true;

	//MessageBoxA(NULL, getStringPointerFromId(0x1C0)->stringData, "wew", NULL);

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

BYOND::temporary_return_value_holder callglobalprochook(int unk1, int unk2, int const_2, unsigned int proc_id, int const_0, int unk3, int unk4, BYOND::Object* argList, int argListLen, int const_0_2, int const_0_3)
{
	std::lock_guard<std::recursive_mutex> lock(BYOND::Variables::callglobalproc_mutex);
	return BYOND::Variables::callGlobalProc(unk1, unk2, const_2, proc_id, const_0, unk3, unk4, argList, argListLen, const_0_2, const_0_3);
}

bool BYOND::Variables::MakeProcCallThreadsafe()
{
	NTSTATUS result = LhInstallHook(
		callGlobalProc,
		callglobalprochook,
		NULL,
		&callGlobalProcHookInfo);
	ULONG ACLEntries[1] = { 1 };
	LhSetExclusiveACL(ACLEntries, 1, &callGlobalProcHookInfo);

	if (FAILED(result))
	{
		MessageBoxA(nullptr, "Failed to install call_proc_by_id hook!", "oh crap!", 0);
		return false;
	}
	return true;
}

BYOND::temporary_return_value_holder callprochook(int unk1, int unk2, BYOND::ProcType procType, int procName, BYOND::ObjectType datumType, int datumId, BYOND::Object* argList, int argListLen, int unk4, int unk5)
{
	__asm {
		pushad
		pushfd
		}
	//std::lock_guard<std::recursive_mutex> lock(BYOND::Variables::callproc_mutex);
	std::string name(BYOND::Variables::getStringPointerFromId(procName)->stringData);
	datumType = static_cast<BYOND::ObjectType>(static_cast<unsigned char>(datumType));
	if(vars.callProcHooks.find(name) != vars.callProcHooks.end())
	{
		BYOND::Variables::InternalCallProcHookInfo internal_info = vars.callProcHooks[name];
		switch (internal_info.filter.type)
		{
		case BYOND::Variables::ProcHookFilterType::None:
			goto run_hook;
			break;
		case BYOND::Variables::ProcHookFilterType::Type:
			if (datumType == internal_info.filter.objType) goto run_hook;
			else goto run_proc;
			break;
		case BYOND::Variables::ProcHookFilterType::Object:
			if (datumType == internal_info.filter.objType &&
				datumId == reinterpret_cast<int>(internal_info.filter.objValue)) goto run_hook;
			else goto run_proc;
			break;
		default:
			MessageBoxA(NULL, "Invalid proc hook filter type!", "fix yo code", NULL);
			goto run_proc;
		}
		run_hook:
		BYOND::Variables::CallProcHookInfo info;
		info.args = argList;
		info.numArgs = argListLen;
		info.src = BYOND::DatumObject(datumType, datumId);
		if(!internal_info.hookFunc(&info))
		{
			BYOND::temporary_return_value_holder trvh;
			trvh.value = info.returnValue.value;
			trvh.type = info.returnValue.type;
			__asm {
				popfd
				popad
				}
			return trvh;
		}
	}
	run_proc:
	name.~basic_string();
	__asm {
		popfd
		popad
		mov eax, BYOND::Variables::callProc
		add eax, 7
		jmp eax
		};
}

bool BYOND::Variables::HookObjectProcCalls()
{
	DWORD new_prot = PAGE_EXECUTE_READWRITE;
	DWORD old;
	VirtualProtect(callProc, 5, new_prot, &old);
	DWORD offset = (DWORD)callprochook - (DWORD)callProc - 5;
	unsigned char* addr = (unsigned char*)&offset;
	*(unsigned char*)callProc = 0xE9; //JMP
	std::memcpy((void*)((unsigned char*)callProc + 1), addr, 4);
	VirtualProtect(callProc, 5, old, &new_prot);
	return true;
}

void BYOND::Variables::HookProc(std::string procName, CallProcHookFunction* func, ProcHookFilter filter)
{
	InternalCallProcHookInfo info;
	info.filter = filter;
	info.hookFunc = func;
	std::replace(procName.begin(), procName.end(), '_', ' ');
	callProcHooks[procName] = info;
}

void BYOND::Variables::HookProc(std::string procName, CallProcHookFunction* func)
{
	ProcHookFilter filter{ ProcHookFilterType::None, ObjectType::Null, 0 };
	HookProc(procName, func, filter);
}

void BYOND::Variables::HookProc(std::string procName, CallProcHookFunction* func, ObjectType type)
{
	ProcHookFilter filter{ ProcHookFilterType::Type, type, 0 };
	HookProc(procName, func, filter);
}

void BYOND::Variables::HookProc(std::string procName, CallProcHookFunction* func, ObjectType type, int value)
{
	ProcHookFilter filter{ ProcHookFilterType::Object, type, reinterpret_cast<void*>(value) };
	HookProc(procName, func, filter);
}

void BYOND::Variables::HookProc(std::string procName, CallProcHookFunction* func, ObjectType type, float value)
{
	ProcHookFilter filter{ ProcHookFilterType::Object, type, *reinterpret_cast<void**>(&value) };
	HookProc(procName, func, filter);
}

void BYOND::Variables::UnhookProc(std::string procName)
{
	callProcHooks.erase(procName);
}

bool BYOND::Variables::GetFunctionPointers()
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

	if (!Pocket::GetFunction<GetContainerItemPtr*>(getContainerItem, rangeStart, miModInfo.SizeOfImage, "55 8B EC 51 8B 4D 08 C6 45 FF 00 80 F9 05 76 ?? 80 F9 21 74 ?? 80 F9 0D 74 ?? 80 F9 0E 75 ?? EB ?? 84 C9 74 ?? 6A 00 8D 45 FF 50 FF 75 0C 51 6A 00 6A 7B"))
	{
		MessageBoxA(nullptr, "Failed to acquire getContainerItem", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<GetStringTableIndexPtr*>(getStringTableIndex, rangeStart, miModInfo.SizeOfImage, "55 8B EC 8B 45 08 83 EC 18 53 8B 1D ?? ?? ?? ?? 56 57 85 C0 75 ?? 68 ?? ?? ?? ?? FF D3 83 C4 04 C6 45 10 00 80 7D 0C 00 89 45 E8 74 ?? 8D 45 10 50 8D 45 E8 50"))
	{
		MessageBoxA(nullptr, "Failed to acquire getStringTableIndex", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<Text2PathPtr*>(text2path, rangeStart, miModInfo.SizeOfImage, "55 8B EC 83 EC 08 56 57 8B 7D 08 57 E8 ?? ?? ?? ?? 83 C4 04 8B 30 A1 ?? ?? ?? ?? 89 45 FC A1 ?? ?? ?? ?? 89 45 F8 81 FF 9D 00 00 00 0F ?? ?? ?? ?? ?? 6A 06 57 E8 ?? ?? ?? ?? 52 50 FF 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B D0 83 C4 14 85 D2 74 ?? 8B 42 08"))
	{
		MessageBoxA(nullptr, "Failed to acquire text2path", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<CallGlobalProcPtr*>(callGlobalProc, rangeStart, miModInfo.SizeOfImage, "55 8B EC 81 EC 98 00 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 FC 8B 55 14 8B 45 30 89 85 6C FF FF FF 53 8B 5D 24 56 8B 75 2C 57 8B 7D 28 81 FA FF FF 00 00 75 ?? 0F 57 C0 66 0F 13 85 68 FF FF FF 8B BD 6C FF FF FF 8B 9D 68 FF FF FF 85 F6"))
	{
		MessageBoxA(nullptr, "Failed to acquire callGlobalProc", "oh no!", 0);
		return false;
	}

	if (!Pocket::GetFunction<CreateNewDatumObjectPtr*>(createNewDatumObject, rangeStart, miModInfo.SizeOfImage, "55 8B EC 83 EC 20 80 3D ?? ?? ?? ?? 00 A1 ?? ?? ?? ?? 56 57 8B 00 8B 48 08 8B 40 0C 89 4D FC 89 45 F8 74 38 8B 4D 08 8B 15 ?? ?? ?? ?? 8B 35 ?? ?? ?? ?? 8B 41 04 8B 39 50 57 89 11 89 71 04"))
	{
		MessageBoxA(nullptr, "Failed to acquire createNewDatumObject", "oh no!", 0);
		return false;
	}

	unsigned char* false_null_location = (unsigned char*)Pocket::Sigscan::FindPattern(rangeStart, miModInfo.SizeOfImage, "83 C4 1C A1 ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 5E 8B E5 5D C3 CC", 8);
	DWORD protec;
	DWORD dong;
	VirtualProtect(false_null_location, 6, PAGE_READWRITE, &protec);
	false_null_location[0] = 0xBA; //mov edx, constant
	false_null_location[1] = 0xEF;
	false_null_location[2] = 0xBE;
	false_null_location[3] = 0xAD;
	false_null_location[4] = 0xDE;
	false_null_location[5] = 0x90; // nop
	VirtualProtect(false_null_location, 6, protec, &dong);
	
	// One of these is right (maybe), not needed as of now.
	//mob_list = (DWORD*)**(DWORD**)(*(int*)((BYTE*)readVariable + 57 + *(int*)((BYTE*)readVariable + 40)) + (DWORD)((BYTE*)readVariable + 57 + *(int*)((BYTE*)readVariable + 40)) + 23); //OH GOD OH FUCK
	//mob_list = Pocket::Sigscan::FindPattern(rangeStart, miModInfo.SizeOfImage, "A1 ?? ?? ?? ?? 8B 04 B0 85 C0 74 34 FF B0", 1);

	return true;
}


BYOND::Object BYOND::Variables::ReadVariable(ObjectType type, int datumId, const std::string varName)
{
	return BYOND::Object(readVariable(type, datumId, BYONDSTR(varName)));
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


void BYOND::Variables::SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, unsigned int new_value) const
{
	setVariable(type, datumId, BYONDSTR(varName), varType, reinterpret_cast<void*>(new_value));
}

void BYOND::Variables::SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, float new_value) const
{
	setVariable(type, datumId, BYONDSTR(varName), varType, *reinterpret_cast<void**>(&new_value));
}

void BYOND::Variables::SetVariable(BYOND::ObjectType type, int datumId, std::string varName, std::string new_value) const
{
	SetVariable(type, datumId, varName, VariableType::String, BYONDSTR(new_value));
}

BYOND::Object BYOND::Variables::CallObjectProc(Object obj, std::string procName, std::vector<Object> arguments)
{
	std::replace(procName.begin(), procName.end(), '_', ' ');
	ULONG ACLEntries[1] = { 0 };
	LhSetInclusiveACL(ACLEntries, 1, &globalTimerHookInfo);
	return BYOND::Object(callProc(0, 0, ProcType::Proc, BYONDSTR(procName), static_cast<ObjectType>(obj.Type()), reinterpret_cast<int>(obj.value), arguments.data(), arguments.size(), 0, 0));
}

BYOND::Object BYOND::Variables::CallObjectProc(Object obj, std::string procName)
{
	return CallObjectProc(obj, procName, std::vector<Object>());
}

struct UnknownContextElement
{
	int unk1;
	int unk2;
	int someType;
	int someValue;
	char some_writable_memory[90];
};

struct Context
{
	UnknownContextElement* uce;
	char some_writable_memory[90];
};

BYOND::DatumObject BYOND::Variables::New(std::string type, BYOND::DatumObject loc)
{
	BYOND::Object* otype = new BYOND::Object(type);
	Context* ctx = new Context();
	ctx->uce = new UnknownContextElement();
	ctx->uce->someType = 3;
	ctx->uce->someValue = 1;
	*(Context**)*(int*)((char*)createNewDatumObject + 14) = ctx;
	//MessageBoxA(NULL, Pocket::IntegerToStrHex(cursed_variable).c_str(), "help me", NULL);
	createNewDatumObject(otype, &loc, 0, 0);
	BYOND::DatumObject result(static_cast<ObjectType>(otype->type), reinterpret_cast<int>(otype->value));
	delete ctx->uce;
	delete ctx;
	delete otype;
	return result;
}

BYOND::DatumObject BYOND::Variables::New(std::string type)
{
	return New(type, BYOND::DatumObject(ObjectType::Null, 0));
}

std::string BYOND::Variables::GetStringFromId(unsigned int id) const
{
	return std::string(GetCStringFromId(id));
}

BYOND::List BYOND::Variables::GetListFromId(unsigned int id) const
{
	return BYOND::List(id);
}

char* BYOND::Variables::GetCStringFromId(unsigned int id) const
{
	return getStringPointerFromId(id)->stringData;
}

void BYOND::Variables::IncreaseStringRefcount(unsigned int id)
{
	getStringPointerFromId(id)->refcount++;
}

void BYOND::Variables::DecreaseStringRefcount(unsigned int id)
{
	getStringPointerFromId(id)->refcount--;
}

BYOND::Object BYOND::Variables::GetContainerItem(BYOND::VariableType containerType, unsigned int containerId, BYOND::Object key)
{
	return BYOND::Object(getContainerItem(containerType, containerId, key.Type(), reinterpret_cast<int>(key.value)));
}

unsigned int BYOND::Variables::GetByondString(std::string str)
{
	unsigned int index = getStringTableIndex(str.c_str(), 0, 1);
	IncreaseStringRefcount(index);
	return index;
}

BYOND::Object BYOND::Variables::CallGlobalProc(std::string procName, std::vector<Object> arguments)
{
	if (procName.rfind("/proc/", 0) != 0)
	{
		procName = "/proc/" + procName;
	}
	BYOND::Object path = Text2Path(procName);
	ULONG ACLEntries[1] = { 0 };
	LhSetInclusiveACL(ACLEntries, 1, &globalTimerHookInfo);
	return BYOND::Object(callGlobalProc(3, 1, 2, reinterpret_cast<unsigned int>(path.value), 0, 0, 0, arguments.data(), arguments.size(), 0, 0));
}

BYOND::Object BYOND::Variables::CallGlobalProc(std::string procName)
{
	return CallGlobalProc(procName, std::vector<Object>());
}

BYOND::Object BYOND::Variables::Text2Path(unsigned int id)
{
	return BYOND::Object(text2path(id));
}

BYOND::Object BYOND::Variables::Text2Path(std::string text)
{
	return Text2Path(BYONDSTR(text));
}

