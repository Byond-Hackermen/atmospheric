#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <DbgHelp.h>
#include <tlhelp32.h>

#pragma comment(lib, "dbghelp.lib")

#define SET_VAR_OFFSET 0x102e00 //it's wrong though

uintptr_t GetModuleBaseAddress(const char* modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_stricmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

typedef void(set_var_f)(int type, int datum_id, int var_name_id, int* unknown, float new_value);
typedef void(set_var_i)(int type, int datum_id, int var_name_id, int* unknown, int new_value);

void begin() {
	set_var_f* set_variable = (set_var_f*)(GetModuleBaseAddress("byondcore.dll") + SET_VAR_OFFSET);
	set_variable(2, 0, 0x166, this is probably a pointer to dung server, (float)5);
}

BOOL APIENTRY DllMain(HANDLE hInstance, DWORD fdwReason, LPVOID lpReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		DWORD dwThreadId;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&begin, NULL, NULL, &dwThreadId);
	}
	return TRUE;
}