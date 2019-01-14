
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

	//vars.CallGlobalProc("/proc/to_chat", { BYOND::Variables::world, BYOND::Object("Asyncmos initialized!") });
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
	float dirs[] = { 1, 2, 4, 8 };
	while (true) {
		BYOND::List lst = vars.ReadGlobalVariable("mob_list").AsList();
		for (int i = 0; i < lst.Length(); i++)
		{
			BYOND::Mob mob = lst.At(i)->As(BYOND::Mob);
			mob.Set("dir", BYOND::VariableType::Number, dirs[rand()%4]);
			Sleep(1);
		}
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
	assert(object.Call(objType + "_test_proc", { argument }).AsNumber() == argument.AsNumber());

	assert(object.Get<float>(objType + "_variable_number") == static_cast<float>(5));
	assert(object.Get<std::string>(objType + "_variable_string") == objType + " variable of type string");

	BYOND::List object_list_var = object.Get<BYOND::List>(objType + "_variable_list");
	assert(object_list_var[0]->AsString() == objType + " variable list element 1");
	assert(object_list_var[1]->AsString() == objType + " variable list element 2");

}

void perform_tests()
{
	assert(vars.ReadGlobalVariable("global_variable_string").AsString() == "global variable of type string");
	assert(vars.ReadGlobalVariable("global_variable_number").AsNumber() == static_cast<float>(5));

	test_object<BYOND::Area>("area", BYOND::VariableType::Area);
	test_object<BYOND::Obj>("obj", BYOND::VariableType::Obj);
	test_object<BYOND::Mob>("mob", BYOND::VariableType::Mob);
	test_object<BYOND::Datum>("datum", BYOND::VariableType::Datum);

	BYOND::List list = vars.ReadGlobalVariable("global_variable_list").AsList();
	assert(list[0]->AsString() == "global variable list element 1");
	assert(list[1]->AsString() == "global variable list element 2");
	BYOND::List assocList = vars.ReadGlobalVariable("global_associative_list").AsList();
	BYOND::Object index("associative list key 1");
	assert(assocList[index]->AsString() == "associative list value 1");
	BYOND::Object index2("associative list key 2");
	assert(assocList[index2]->AsString() == "associative list value 2");

	BYOND::Object argument(static_cast<float>(5));
	assert(vars.CallGlobalProc("/proc/global_proc_1", { argument }).AsNumber() == static_cast<float>(5));
	//*list[0] = BYOND::Object(BYOND::VariableType::String, BYONDSTR("Adding new string to the string table!"));
	//assert(list[0]->AsString() == "Adding new string to the string table!");
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
