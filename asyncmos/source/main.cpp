
#include <Windows.h>
#include <thread>

#include "byond/interface.h"
#include "byond/variables.h"
#include "pocket/utilities.h"
#include "byond/list.h"

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
	//BYOND::Variables vars;
	if (!vars.Initialize())
	{
		msg("Failed to initialize!", "heck");
		return;
	}

	std::string var = vars.ReadGlobalVariable("game_version").AsString();
	msg(var.c_str(), "game_version");

	const std::string about = BYOND::GetByondAbout();
	msg(about.c_str(), "byond about");
}

BYOND_EXPORT(init)
{
	std::thread t(init_thread);
	t.detach();

	return nullptr;
}


void process_thread()
{
	BYOND::List* lst = vars.ReadGlobalVariable("mob_list").AsList();
	msg(Pocket::IntegerToStrHex((unsigned int)lst).c_str(), "address of mob list");
	for (int i = 0; i < lst->Length(); i++)
	{
		BYOND::Object* mob = lst->At(i);
		//msg(Pocket::IntegerToStrHex((int)mob->value).c_str(), "mob value");
		msg(vars.ReadVariable(BYOND::ObjectType::Mob, (int)mob->value, "color").AsString().c_str(), "mob color");
		vars.SetVariable(BYOND::ObjectType::Mob, (int)mob->value, "color", BYOND::VariableType::String, BYONDSTR("#00FF00"));
		msg(vars.ReadVariable(BYOND::ObjectType::Mob, (int)mob->value, "color").AsString().c_str(), "mob color");
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
