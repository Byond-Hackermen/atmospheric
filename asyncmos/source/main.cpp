
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
	BYOND::List lst = vars.ReadGlobalVariable("mob_list").AsList();
	//msg(Pocket::IntegerToStrHex((unsigned int)lst).c_str(), "address of mob list");
	for (int i = 0; i < lst.Length(); i++)
	{
		BYOND::Mob* mob = static_cast<BYOND::Mob*>(lst.At(i));
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

template <typename T>
void test_object(std::string objType, BYOND::VariableType expectedVariableType)
{
	T object = vars.ReadGlobalVariable("global_variable_" + objType).As(T);
	assert(object.Type() == expectedVariableType);
	assert(object.value == 0);

	BYOND::Object argument(static_cast<float>(5));
	assert(object.CallProc(objType + "_test_proc", { argument }).AsNumber() == argument.AsNumber());

	assert(object.GetVariable(objType + "_variable_number").AsNumber() == static_cast<float>(5));
	assert(object.GetVariable(objType + "_variable_string").AsString() == objType + " variable of type string");

	BYOND::List object_list_var = object.GetVariable(objType + "_variable_list").AsList();
	assert(object_list_var[0]->AsString() == objType + " variable list element 1");
	assert(object_list_var[1]->AsString() == objType + " variable list element 2");

}

void perform_tests()
{
	assert(vars.ReadGlobalVariable("global_variable_string").AsString() == "global variable of type string");
	assert(vars.ReadGlobalVariable("global_variable_number").AsNumber() == static_cast<float>(5));
	BYOND::List list = vars.ReadGlobalVariable("global_variable_list").AsList();
	assert(list[0]->AsString() == "global variable list element 1");
	assert(list[1]->AsString() == "global variable list element 2");
	BYOND::List assocList = vars.ReadGlobalVariable("global_associative_list").AsList();
	BYOND::Object index("associative list key 1");
	assert(assocList[index]->AsString() == "associative list value 1");
	BYOND::Object index2("associative list key 2");
	assert(assocList[index2]->AsString() == "associative list value 2");

	test_object<BYOND::Area>("area", BYOND::VariableType::Area);
	test_object<BYOND::Obj>("obj", BYOND::VariableType::Obj);
	test_object<BYOND::Mob>("mob", BYOND::VariableType::Mob);
	test_object<BYOND::Datum>("datum", BYOND::VariableType::Datum);
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
