#pragma once

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "LuaBridge.h"

#include "../byond/variables.h"

void run_script();

extern std::map<int, BYOND::VariableType> lua_to_byond_types;

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