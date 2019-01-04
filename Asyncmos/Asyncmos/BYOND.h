#pragma once

#include <map>
#include <string>
#include <windows.h>
#include <DbgHelp.h>
#include <psapi.h>

#include "Pocket/Utilities.h"

#define BYONDSTR(x) (DWORD)BYOND::Variables::stringTable[x]

#define STRING_TABLE_PTR_OFFSET 

namespace BYOND
{
	class Variables {
	public:
		enum class ObjectType {
			Turf = 0x1,
			Obj = 0x2,
			Mob = 0x3,
			Area = 0x4,
			Client = 0x5,
			Image = 0xD,
			World = 0xE, //set datum id to null
			Global = 0xE,
			List = 0x54,
			Datum = 0x21,
			Savefile = 0x23
		};
		enum class VariableType {
			Number = 0x2A,
			String = 0x06,
			List = 0x0F,
		};

		Variables();
		~Variables();

		static std::map<std::string, int> stringTable;

		void GenerateStringTable();
		void GetFunctionPointers();

		typedef void(__cdecl SetVariablePtr)(BYOND::Variables::ObjectType type, int datumId, int varNameId, BYOND::Variables::VariableType varType, void* newValue);
		typedef void(__cdecl GetVariablePtr)(BYOND::Variables::ObjectType type, int datumId, int varNameId);
		typedef char**(__cdecl GetStringPointerFromIdPtr)(int stringId);

		SetVariablePtr* setVariable;
		GetVariablePtr* getVariable;
		GetStringPointerFromIdPtr* getStringPointerFromId;

		char* getStringFromId(int id);

	public:
		int ReadVariable(ObjectType type, int datumId, std::string varName);
		void SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, DWORD new_value);
		void SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, float new_value);
	};

};

