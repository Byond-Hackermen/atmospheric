#pragma once

#include <map>
#include <vector>
#include "byond.h"
#include "object.h"
#include <easyhook.h>

namespace BYOND
{
	class List;
	struct ByondList;

	class Variables
	{
	public:
		static std::map<std::string, int> stringTable;

		bool Initialize();
		bool Ready() const { return init_done; }
		


	public:
		BYOND::Object ReadVariable(BYOND::ObjectType type, int datumId, std::string varName);
		BYOND::Object ReadVariable(BYOND::Object obj, std::string varName);
		BYOND::Object ReadWorldVariable(std::string name);
		BYOND::Object ReadGlobalVariable(std::string name);

		void SetVariable(BYOND::ObjectType type, int datumId, std::string varName, BYOND::VariableType varType, int new_value) const;
		void SetVariable(BYOND::ObjectType type, int datumId, std::string varName, BYOND::VariableType varType, float new_value) const;

		BYOND::Object CallObjectProc(BYOND::Object, std::string procName);
		BYOND::Object CallObjectProc(BYOND::Object, std::string procName, std::vector<BYOND::Object> arguments);

		std::string GetStringFromId(int id) const;
		List GetListFromId(int id) const;
		char* GetCStringFromId(int id) const;
		BYOND::Object GetContainerItem(BYOND::VariableType containerType, int containerId, BYOND::Object key);

	private:
		void* mob_list = nullptr;
		bool init_done = false;
		HOOK_TRACE_INFO globalTimerHookInfo = { 0 };

	private:
		// Internal
		void GenerateStringTable() const;
		bool GetFunctionPointers() const;
		bool HookGlobalTimer();

	public:
		typedef void(__cdecl SetVariablePtr)(BYOND::ObjectType type, int datumId, int varNameId, BYOND::VariableType varType, void* newValue);
		static SetVariablePtr* setVariable;

		typedef void(__cdecl GetVariablePtr)(BYOND::ObjectType type, int datumId, int varNameId);
		static GetVariablePtr* getVariable;

		typedef char**(__cdecl GetStringPointerFromIdPtr)(int stringId);
		static GetStringPointerFromIdPtr* getStringPointerFromId;

		typedef BYOND::ByondList*(__cdecl GetListPointerPtr)(int listId);
		static GetListPointerPtr* getListPointer;

		typedef void(__cdecl AppendToContainerPtr)(BYOND::VariableType containerType, int containerId, BYOND::VariableType varType, void* varValue);
		static AppendToContainerPtr* appendToContainer;

		typedef void(__cdecl RemoveFromContainerPtr)(BYOND::VariableType containerType, int containerId, BYOND::VariableType varType, void* varValue);
		static RemoveFromContainerPtr* removeFromContainer;

		typedef void(__cdecl ReadVariablePtr)(BYOND::ObjectType type, int datumId, int varNameId);
		static ReadVariablePtr* readVariable;

		typedef void*(__cdecl CallProcPtr)(int unk1, int unk2, ProcType procType, int procName, ObjectType datumType, int datumId, Object* argList, int argListLen, int unk4, int unk5);
		static CallProcPtr* callProc;

		typedef void*(__cdecl GetContainerItemPtr)(VariableType containerType, int containerId, VariableType keyType, int keyValue);
		static GetContainerItemPtr* getContainerItem;
	};
}
extern BYOND::Variables vars;
