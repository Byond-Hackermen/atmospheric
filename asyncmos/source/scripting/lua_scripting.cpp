#include "lua_scripting.h"
#include <luaconf.h>

std::string make_string(std::string s) { return s; }

BYOND::DatumObject getmymob()
{
	auto mobs = vars.ReadGlobalVariable("mob_list").AsList();
	for (int i = 0; i<mobs.Length(); i++)
	{
		auto m = mobs[i]->As(BYOND::Mob);
		if (m.Get<std::string>("name") == "Francesca Owens")
		{
			return m;
		}
	}
}

int BYOND::DatumObject::lua_getvar(lua_State* state)
{
	const char* varname = lua_tostring(state, 2);
	BYOND::Object result = GetVariable(varname);
	switch(result.Type())
	{
	case BYOND::VariableType::Number:
		lua_pushnumber(state, result.AsNumber());
		break;
	case BYOND::VariableType::String:
		lua_pushstring(state, result.AsString().c_str());
		break;
	case BYOND::VariableType::Area:
	case BYOND::VariableType::Turf:
	case BYOND::VariableType::Obj:
	case BYOND::VariableType::Mob:
		luabridge::push(state, BYOND::DatumObject(static_cast<BYOND::ObjectType>(result.Type()), (int)result.value));
		break;
	case BYOND::VariableType::Null:
	default:
		lua_pushnil(state);
		break;
	}
	return 1;
}

void set(BYOND::DatumObject* ths, lua_State* state)
{
	const char* varname = lua_tostring(state, 2);
	if (lua_type(state, 3) == LUA_TNUMBER)
	{
		ths->Set(varname, BYOND::VariableType::Number, (float)lua_tonumber(state, 3));
	}
	else if (lua_type(state, 3) == LUA_TSTRING)
	{
		ths->Set(varname, lua_tostring(state, 3));
	}
	else
	{
		luabridge::LuaRef ref(state, 3);
		BYOND::DatumObject bdo = ref.cast<BYOND::DatumObject>();
		ths->Set(varname, bdo.Type(), (unsigned int)bdo.value);
	}
}

void fuck()
{

}

int BYOND::DatumObject::lua_setvar(lua_State* state)
{
	__try
	{
		set(this, state);
	}
	__except (1)
	{
		MessageBoxA(NULL, "Caught server crashing error!!", "holy carp", NULL);
	}
	return 0;
}

void sleep(int milis)
{
	Sleep(milis);
}

using namespace luabridge;
void run_script()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	getGlobalNamespace(L)
		.beginNamespace("vars")
		.beginClass<BYOND::DatumObject>("Atom")
		.addCFunction("__index", &BYOND::DatumObject::lua_getvar)
		.addCFunction("__newindex", &BYOND::DatumObject::lua_setvar)
		.endClass()
		.addFunction("mymob", getmymob)
		.addFunction("sleep", sleep)
		.endNamespace();

	luaL_dofile(L, "testscript.lua");
	LuaRef var = getGlobal(L, "testvar");
}