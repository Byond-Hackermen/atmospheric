// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "BYOND.h"
#include <fstream>
#define msg(x, y) MessageBoxA(NULL, x, y, NULL)

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

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		DWORD dwThreadId;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&test_run, NULL, NULL, &dwThreadId);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

