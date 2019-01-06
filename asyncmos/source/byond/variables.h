#pragma once

#include <map>
#include "byond.h"
#include "object.h"

namespace BYOND
{
	class List;
	struct ByondList;

	class Variables
	{
	public:
		static std::map<std::string, int> stringTable;

		void GenerateStringTable() const;
		bool GetFunctionPointers() const;


	public:
		BYOND::Object ReadVariable(BYOND::ObjectType type, int datumId, std::string varName);
		BYOND::Object ReadWorldVariable(std::string name);
		BYOND::Object ReadGlobalVariable(std::string name);

		void SetVariable(BYOND::ObjectType type, int datumId, std::string varName, BYOND::VariableType varType, int new_value) const;
		void SetVariable(BYOND::ObjectType type, int datumId, std::string varName, BYOND::VariableType varType, float new_value) const;

		std::string GetStringFromId(int id) const;
		List* GetListFromId(int id) const;
		char* GetCStringFromId(int id) const;

	private:
		void* mob_list = nullptr;

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
	};
}