#pragma once

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT extern "C" __attribute__((visibility("default")))	
#endif

#define BYOND_EXPORT(func) DLL_EXPORT const char* func (int argc, const char* argv[])