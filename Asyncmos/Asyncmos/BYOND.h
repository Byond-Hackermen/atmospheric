#pragma once

#include <map>
#include <string>
#include <windows.h>
#include <DbgHelp.h>
#include <psapi.h>

#include "Pocket/Utilities.h"

#define BYONDSTR(x) StringTable.operator[x]

typedef void(SetVariablePtr)(BYOND::Variables::ObjectType type, int datumId, int varNameId, BYOND::Variables::VariableType varType, DWORD newValue);

namespace BYOND
{
	SetVariablePtr* setVariable;

	class Variables {
	public:
		enum ObjectType {
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
		enum VariableType {
			Integer = 0x2a,
			String = 0x06
		};
		Variables();
		~Variables();

		std::map<std::string, int> StringTable;

		Variables();
		~Variables();

		void GenerateStringTable();
		void get_function_pointers();

	public:
		int ReadVariable(char type, int datumId, int varNameId);
		int SetVariable(ObjectType type, int datumId, int varNameId, VariableType varType, DWORD new_value);
	};
};

