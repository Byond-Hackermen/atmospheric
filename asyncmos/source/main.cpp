
#include <Windows.h>
#include <thread>
#include <cassert>

#include "pocket/utilities.h"
#include "pocket/errorhandling.h"

#include "byond/interface.h"
#include "byond/variables.h"
#include "byond/list.h"
#include "byond/object_types.h"

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

	BYOND::Mob mob = vars.ReadGlobalVariable("global_variable_mob").As(BYOND::Mob);
	assert(mob.Type() == BYOND::VariableType::Mob);
	assert(mob.value == 0);

	BYOND::Object argument(BYOND::VariableType::Number, static_cast<float>(5));

	assert(mob.CallProc("mob_test_proc", { argument }).AsNumber() == argument.AsNumber());

	assert(mob.GetVariable("mob_variable_number").AsNumber() == static_cast<float>(5));
	assert(mob.GetVariable("mob_variable_string").AsString() == "mob variable of type string");
	BYOND::List* mob_list_var = mob.GetVariable("mob_variable_list").AsList();
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
