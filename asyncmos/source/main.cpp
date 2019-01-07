
#include <Windows.h>
#include <thread>
#include <cassert>

#include "pocket/utilities.h"
#include "pocket/errorhandling.h"

#include "byond/interface.h"
#include "byond/variables.h"
#include "byond/list.h"
#include "byond/mob.h"

/*
void test_run()
{
	msg("help me", "honk");
	BYOND::Variables vars;
	if(!vars.GetFunctionPointers())
	{
		msg("Failed to get all function pointers!", "heck");
		return;
	}
	vars.GenerateStringTable();
	msg("Finished building table", "yay");
	std::ofstream fout("strings.txt");
	for (auto const& x : vars.stringTable)
	{
		fout << x.first << std::endl;
	}
	msg("done", "woo");
	//BYOND::List* lst = vars.GetListFromId(3);
	//msg(Pocket::IntegerToStrHex((DWORD)vars.mob_list).c_str());
	//for (DWORD i = 0; i < lst->Length(); i++)
	//{
	//	BYOND::Object* elem = lst->At(i);
	//	msg(Pocket::IntegerToStrHex((DWORD)elem).c_str(), "vartype");
	//}
	//BYOND::Object var = vars.ReadVariable(BYOND::ObjectType::Obj, 0, "flippity_flop");
	//msg(var.AsString(vars).c_str());
}
*/

#define msg(x, y) MessageBoxA(nullptr, x, y, MB_OK)

void init_thread()
{
	if (!vars.Initialize())
	{
		msg("Failed to initialize!", "heck");
	}
}

BYOND_EXPORT(init)
{
	if (vars.Ready())
		return "already initialized";

	std::thread t(init_thread);
	t.detach();

	return nullptr;
}


void process_thread()
{
	BYOND::List* lst = vars.ReadGlobalVariable("mob_list").AsList();
	//msg(Pocket::IntegerToStrHex((unsigned int)lst).c_str(), "address of mob list");
	for (int i = 0; i < lst->Length(); i++)
	{
		BYOND::Mob* mob = static_cast<BYOND::Mob*>(lst->At(i));
		if (!Pocket::ErrorHandling::IsValidPtr(mob))
			continue;

		//msg(Pocket::IntegerToStrHex((int)mob->value).c_str(), "mob value");
		msg(mob->GetVariable("color").AsString().c_str(), "mob color");
		mob->SetVariable("color", BYOND::VariableType::String, BYONDSTR("#00FF00"));
		msg(mob->GetVariable("color").AsString().c_str(), "mob color after update");
	}
}

BYOND_EXPORT(process)
{
	if (!vars.Ready())
		return "vars not ready";

	std::thread t(process_thread);
	t.detach();

	return nullptr;
}

void perform_tests()
{
	assert(vars.ReadGlobalVariable("global_variable_string").AsString() == "global variable of type string");
	assert(vars.ReadGlobalVariable("global_variable_number").AsNumber() == static_cast<float>(5));
	BYOND::List* list = vars.ReadGlobalVariable("global_variable_list").AsList();
	assert(list->At(0)->AsString() == "global variable list element 1");
	assert(list->At(1)->AsString() == "global variable list element 2");

	BYOND::Mob* mob = static_cast<BYOND::Mob*>(&vars.ReadGlobalVariable("global_variable_mob"));

	vars.CallProc(*mob, "mob_test_proc");

	assert(mob->GetVariable("mob_variable_number").AsNumber() == static_cast<float>(5));
	assert(mob->GetVariable("mob_variable_string").AsString() == "mob variable of type string");
	BYOND::List* mob_list_var = mob->GetVariable("mob_variable_list").AsList();
	assert(mob_list_var->At(0)->AsString() == "mob variable list element 1");
	assert(mob_list_var->At(1)->AsString() == "mob variable list element 2");
}

BYOND_EXPORT(test)
{
	if (!vars.Ready())
	{
		msg("test() called before initializing library!", "Error!");
		return nullptr;
	}
	std::thread t(perform_tests);
	t.detach();

	return nullptr;
}
