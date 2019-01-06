
#include "interface.h"
#include <Windows.h>
#include <thread>

#include "byond/variables.h"

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

void foo()
{
	BYOND::Variables vars;
	if (!vars.GetFunctionPointers())
	{
		msg("Failed to get all function pointers!", "heck");
		return;
	}
	vars.GenerateStringTable();

	std::string var = vars.ReadGlobalVariable("game_version").AsString(vars);
	msg(var.c_str(), "game_version");

	const std::string about = BYOND::GetByondAbout();
	msg(about.c_str(), "byond about");
}

BYOND_EXPORT(init)
{
	std::thread t(foo);
	t.detach();

	return "test lol";
}
