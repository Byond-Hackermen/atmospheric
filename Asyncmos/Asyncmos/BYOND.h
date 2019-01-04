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
		std::map<std::string, int> string_table;

		Variables();
		~Variables();

		void generate_string_table();
		void get_function_pointers();

	public:
		void set(datum_type d_type, datum_id id, std::string var_name, int var_type, void* new_value);
	};
};

