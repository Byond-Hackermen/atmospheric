#include "Utilities.h"

const wchar_t* Pocket::GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}
HANDLE Pocket::GetProcessByName(const char * name)
{
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			const wchar_t* wName = GetWC(name);

			if (process.szExeFile == wName)
			{
				pid = process.th32ProcessID;
				break;
			}
			delete wName;
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	if (pid != 0)
	{
		return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	}

	// Not found


	return NULL;
}

DWORD Pocket::FindPattern(DWORD rangeStart, DWORD rangeEnd, const char* pattern)
{
	const char* pat = pattern;
	DWORD firstMatch = 0;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		__try {
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;
			}
			else {
				pat = pattern;
				firstMatch = 0;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { }
	}
	return NULL;
}