
#include <Windows.h>
#include <thread>
#include <cassert>

#include "pocket/utilities.h"
#include "pocket/errorhandling.h"

#include "byond/interface.h"
#include "byond/variables.h"
#include "byond/list.h"
#include "byond/object_types.h"
#include "Atmospherics/atmospherics.h"
#include "scripting/lua_scripting.h"

#include <eh.h>

#define msg(x, y) MessageBoxA(nullptr, x, y, MB_OK)

std::map<int, BYOND::VariableType> lua_to_byond_types = {
	{ LUA_TSTRING, BYOND::VariableType::String },
	{ LUA_TNUMBER, BYOND::VariableType::Number },
	{ LUA_TNIL, BYOND::VariableType::Null },
	{ LUA_TTABLE, BYOND::VariableType::List }
};


LONG WINAPI pls(LPEXCEPTION_POINTERS exc)
{
	//MessageBoxA(NULL, "Caught uncaught unfiltered exception", "yes", NULL);
	return EXCEPTION_CONTINUE_EXECUTION;
}

__declspec(naked) void return_immediately()
{
	__asm {
		mov esp, ebp
		pop ebp
		ret
		}
}

LONG WINAPI VectoredHandler(_EXCEPTION_POINTERS *ExceptionInfo)
{
	//MessageBoxA(NULL, "Caught uncaught vectored exception", "yes", NULL);
	ExceptionInfo->ContextRecord->Eip = reinterpret_cast<DWORD>(return_immediately);
	return EXCEPTION_CONTINUE_EXECUTION;
}

void init_thread()
{
	if (!vars.Initialize())
	{
		msg("Failed to initialize!", "heck");
	}

	SetUnhandledExceptionFilter(pls);
	AddVectoredExceptionHandler(0, VectoredHandler);

	vars.CallGlobalProc("to_chat", { BYOND::Variables::world, BYOND::Object("Asyncmos initialized!") });
}

BYOND_EXPORT(init)
{
	if (vars.Ready())
		return "already initialized";

	std::thread t(init_thread);
	t.detach();

	return nullptr;
}

BYOND_EXPORT(run_lua_script)
{
	if (!vars.Ready())
		return "Vars not initialized!";

	std::thread t(run_script);
	t.detach();

	return nullptr;
}

BYOND::Mob ayylmao;

bool sprechen(BYOND::Variables::CallProcHookInfo* info)
{
	info->src.Call("say", { BYOND::Object("Hello, world!") });
	return true;
}

bool process_cell(BYOND::Variables::CallProcHookInfo* info)
{
	Atmospherics::ProcessCell(true, vars.ReadGlobalVariable("SSair").As(BYOND::Datum).Get<float>("times_fired"), info->src.As(BYOND::Turf));
	return true;
}

void process_thread()
{
	BYOND::List mobs = vars.ReadGlobalVariable("mob_list");
	BYOND::Mob mob = *mobs.At(1);
	/*while (true)
	{
		auto machines = vars.ReadGlobalVariable("machines").AsList();
		for (int i = 0; i < machines.Length(); i++)
		{
			BYOND::Obj machine = machines.At(i)->As(BYOND::Obj);
			MessageBoxA(NULL, std::to_string(static_cast<int>(machine.Type())).c_str(), "machine type", NULL);
			MessageBoxA(NULL, std::to_string(reinterpret_cast<int>(machine.value)).c_str(), "machine value", NULL);
			machine.Call("process", { (float)2 });
			Sleep(2000 / machines.Length());
		}
	}*/
	/*auto mobs = vars.ReadGlobalVariable("mob_list").AsList();
	BYOND::Mob me;
	for(int i=0; i<mobs.Length(); i++)
	{
		auto m = mobs[i]->As(BYOND::Mob);
		if(m.Get<std::string>("name") == "Francesca Owens")
		{
			ayylmao = m;
			break;
		}
	}
	BYOND::Object appearance = ayylmao.GetVariable("appearance");

	MessageBoxA(NULL, std::to_string(static_cast<int>(appearance.Type())).c_str(), "appearance type", NULL);
	MessageBoxA(NULL, std::to_string(reinterpret_cast<int>(appearance.value)).c_str(), "appearance value", NULL);*/
	/*auto start = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < 100000; x++)
	{
		if (ayylmao.Get<float>("health") != 100)
		{
			MessageBoxA(NULL, "incorrect health value!", "error", NULL);
			break;
		}
	}
	auto finish = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::nanoseconds>((finish - start)/100000);
	MessageBoxA(NULL, std::to_string(microseconds.count()).c_str(), "Nanoseconds to read one var", NULL);*/
	//vars.HookProc("Life", sprechen, BYOND::ObjectType::Mob);
	//vars.HookProc("process_cell", process_cell, BYOND::ObjectType::Turf);
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

	assert(object.HasVariable("ayylmao") == false);
}

bool datum_test_hook(BYOND::Variables::CallProcHookInfo* info)
{
	MessageBoxA(NULL, "rewriting argument to \"Hello, world!\"", "no way", NULL);
	*(info->args) = BYOND::Object("Hello, world!");
	return true;
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
	assert(assocList.AssocHasKey("associative list key 1"));

	BYOND::Object argument(static_cast<float>(5));
	assert(vars.CallGlobalProc("/proc/global_proc_1", { argument }).AsNumber() == static_cast<float>(5));

	BYOND::Datum newdatum = vars.New("/datum/datum_type_2");
	assert(newdatum.Get<std::string>("name") == "datum of type 2");
	
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
	std::thread t(process_thread);
	t.detach();

	return nullptr;
}
