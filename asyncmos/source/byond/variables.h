#pragma once

#include <map>
#include <vector>
#include "byond.h"
#include "object.h"
#include "bstring.h"
#include <easyhook.h>
#include <mutex>

namespace BYOND
{
	class List;
	struct ByondList;

	class Variables
	{
	public:

		bool Initialize();
		bool Ready() const { return init_done; }

		static BYOND::Object world;

	public:
		BYOND::Object ReadVariable(BYOND::ObjectType type, int datumId, std::string varName);
		BYOND::Object ReadVariable(BYOND::Object obj, std::string varName);
		BYOND::Object ReadWorldVariable(std::string name);
		BYOND::Object ReadGlobalVariable(std::string name);

		void SetVariable(BYOND::ObjectType type, int datumId, std::string varName, BYOND::VariableType varType, unsigned int new_value) const;
		void SetVariable(BYOND::ObjectType type, int datumId, std::string varName, std::string new_value) const;
		void SetVariable(BYOND::ObjectType type, int datumId, std::string varName, BYOND::VariableType varType, float new_value) const;

		BYOND::Object CallObjectProc(BYOND::Object, std::string procName);
		BYOND::Object CallObjectProc(BYOND::Object, std::string procName, std::vector<BYOND::Object> arguments);
		BYOND::Object CallGlobalProc(std::string procName);
		BYOND::Object CallGlobalProc(std::string procName, std::vector<Object> arguments);

		std::string GetStringFromId(unsigned int id) const;
		List GetListFromId(unsigned int id) const;
		char* GetCStringFromId(unsigned int id) const;
		BYOND::Object GetContainerItem(BYOND::VariableType containerType, unsigned int containerId, BYOND::Object key);

		static unsigned int GetByondString(std::string str);

		static void BYOND::Variables::IncreaseStringRefcount(unsigned int id);
		static void BYOND::Variables::DecreaseStringRefcount(unsigned int id);

		BYOND::Object Text2Path(unsigned int id);
		BYOND::Object Text2Path(std::string text);

	public:
		bool init_done = false;
		HOOK_TRACE_INFO globalTimerHookInfo = { 0 };
		HOOK_TRACE_INFO callGlobalProcHookInfo = { 0 };
		HOOK_TRACE_INFO callProcHookInfo = { 0 };

	public:
		// Internal
		bool GetFunctionPointers();
		bool HookGlobalTimer();
		bool MakeProcCallThreadsafe();
		bool HookObjectProcCalls();

	public:
		static ::std::recursive_mutex callproc_mutex;
		static ::std::recursive_mutex callglobalproc_mutex;

	public:

		typedef void(__cdecl SetVariablePtr)(BYOND::ObjectType type, int datumId, int varNameId, BYOND::VariableType varType, void* newValue);
		static SetVariablePtr* setVariable;

		typedef void(__cdecl GetVariablePtr)(BYOND::ObjectType type, int datumId, int varNameId);
		static GetVariablePtr* getVariable;

		typedef String*(__cdecl GetStringPointerFromIdPtr)(int stringId);
		static GetStringPointerFromIdPtr* getStringPointerFromId;

		typedef BYOND::ByondList*(__cdecl GetListPointerPtr)(int listId);
		static GetListPointerPtr* getListPointer;

		typedef void(__cdecl AppendToContainerPtr)(BYOND::VariableType containerType, int containerId, BYOND::VariableType varType, void* varValue);
		static AppendToContainerPtr* appendToContainer;

		typedef void(__cdecl RemoveFromContainerPtr)(BYOND::VariableType containerType, int containerId, BYOND::VariableType varType, void* varValue);
		static RemoveFromContainerPtr* removeFromContainer;

		typedef temporary_return_value_holder(__cdecl ReadVariablePtr)(BYOND::ObjectType type, int datumId, int varNameId);
		static ReadVariablePtr* readVariable;

		typedef temporary_return_value_holder(__cdecl CallProcPtr)(int unk1, int unk2, ProcType procType, int procName, ObjectType datumType, int datumId, Object* argList, int argListLen, int unk4, int unk5);
		static CallProcPtr* callProc;

		typedef temporary_return_value_holder(__cdecl GetContainerItemPtr)(VariableType containerType, int containerId, VariableType keyType, int keyValue);
		static GetContainerItemPtr* getContainerItem;

		typedef unsigned int(__cdecl GetStringTableIndexPtr)(const char* string, int unk1, int unk2);
		static GetStringTableIndexPtr* getStringTableIndex;

		typedef temporary_return_value_holder(__cdecl Text2PathPtr)(unsigned int text);
		static Text2PathPtr* text2path;

		typedef temporary_return_value_holder(__cdecl CallGlobalProcPtr)(int unk1, int unk2, int const_2, unsigned int proc_id, int const_0, int unk3, int unk4, Object* argList, int argListLen, int const_0_2, int const_0_3);
		static CallGlobalProcPtr* callGlobalProc;
	};
}
extern BYOND::Variables vars;
