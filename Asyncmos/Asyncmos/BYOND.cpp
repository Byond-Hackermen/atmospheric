#include "stdafx.h"
#include "BYOND.h"

BYOND::Variables::Variables()
{
}
BYOND::Variables::~Variables()
{
}

void BYOND::Variables::generate_string_table()
{
	char* current_char = (char*)findPattern(0x00700000, 0x00A00000, "45 78 69 74 00 45 6E 74 65 72 00 4E 65 77 00"); //CHANGE 0x007000000 - WONT CATCH ALL POSSIBLE ADDRESSES
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
			string_table[current_name] = current_string_id;
			if (*(current_char + 1) == 0x00) {
				break;
			}
			current_name = "";
			current_string_id++;
		}
		current_char++;
	}
}

void BYOND::Variables::get_function_pointers()
{
	HMODULE byondcore = GetModuleHandleA("byondcore.dll");
	MODULEINFO mod_info;
	GetModuleInformation(GetCurrentProcess(), byondcore, &mod_info, sizeof(mod_info));
	set_number = (set_var_f*)findPattern((DWORD)byondcore, (DWORD)byondcore + (mod_info.SizeOfImage), "55 8B EC 8B 4D 08 0F B6 C1 48 57 8B 7D 10 83 F8");
	set_other = (set_var_i*)set_number;
}

void BYOND::Variables::set(datum_type d_type, datum_id id, std::string var_name, int var_type, void* new_value)
{
	switch (var_type) {
		case 0x2a: //enumize this
			set_number(d_type, id, BYONDSTR(var_name), var_type, (float)(int)new_value);
			break;
		case 0x06:
			set_other(d_type, id, BYONDSTR(var_name), var_type, BYONDSTR(*(std::string*)new_value));
			break;
	}
}
