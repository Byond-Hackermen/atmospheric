
#include "interface.h"
#include <Windows.h>
#include "BYOND.h"
#include <fstream>
#include <thread>

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
	}
	vars.GenerateStringTable();

	std::ofstream fout("strings.txt");
	for (auto const& x : vars.stringTable)
	{
		fout << x.first << std::endl;
	}

	MessageBoxA(nullptr, "done", "btw", MB_OK);
}

BYOND_EXPORT(init)
{
	std::thread t(foo);
	t.detach();

	return "test lol";
}
