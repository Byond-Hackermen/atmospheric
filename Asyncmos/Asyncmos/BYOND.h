#pragma once

#include <map>
#include <string>
#include <windows.h>
#include <DbgHelp.h>
#include <psapi.h>

#include "Utils.h"

#define BYONDSTR(x) string_table[x]

typedef int datum_type;
typedef int datum_id;

typedef void(set_var_f)(datum_type type, datum_id id, int var_name_id, int var_type, float new_value);
typedef void(set_var_i)(datum_type type, datum_id id, int var_name_id, int var_type, int new_value);

namespace BYOND
{
	set_var_f* set_number;
	set_var_i* set_other;

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
			String = 0x60
		};
		Variables();
		~Variables();

		void generate_string_table();
		void get_function_pointers();

	public:
		void set(datum_type d_type, datum_id id, std::string var_name, int var_type, void* new_value);
		int ReadVariable(char type, int datumId, int varNameId);
		int SetVariable(ObjectType type, int datumId, int varNameId, VariableType varType, DWORD new_value);

	};
};

