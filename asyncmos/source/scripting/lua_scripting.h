#pragma once

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "LuaBridge.h"

#include "../byond/variables.h"
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

void run_script();

class LuaObjectProcCallProxy
{
	BYOND::DatumObject* src;
	std::string name;

public:
	LuaObjectProcCallProxy(BYOND::DatumObject* src, std::string name);
	int call(lua_State* L);
};

class LuaGlobalProcCallProxy
{
	std::string name;

public:
	LuaGlobalProcCallProxy(std::string name);
	int call(lua_State* L);
};

class LuaGlobal
{
public:
	int lua_access_global(lua_State* L);
};