#include "lua_scripting.h"
#include <luaconf.h>
#include <fstream>

LuaObjectProcCallProxy::LuaObjectProcCallProxy(class BYOND::DatumObject* src, std::string name)
{
	this->src = src;
	this->name = name;
}

int LuaObjectProcCallProxy::call(lua_State* L)
{
	int num_args = lua_gettop(L);
	std::vector<BYOND::Object> args;
	for(int i=2; i<=num_args; i++)
	{
		int type = lua_type(L, i);
		;
		if (type == LUA_TNUMBER || type == LUA_TBOOLEAN)
			args.push_back(BYOND::Object(luabridge::Stack<float>::get(L, i)));
		else if (type == LUA_TSTRING)
			args.push_back(BYOND::Object(luabridge::Stack<std::string>::get(L, i)));
		else if (type == LUA_TNIL)
			args.push_back(BYOND::Object(BYOND::VariableType::Null, 0));
		else
			args.push_back(static_cast<BYOND::Object>(luabridge::Stack<BYOND::DatumObject>::get(L, i)));
	}
	BYOND::Object result = src->Call(name, args);
	BYOND::VariableType rtype = result.Type();
	if (rtype == BYOND::VariableType::Number)
		luabridge::push(L, result.AsNumber());
	else if (rtype == BYOND::VariableType::String)
		luabridge::push(L, result.AsString());
	else if (rtype == BYOND::VariableType::Null)
		luabridge::push(L, luabridge::Nil());
	else if (rtype == BYOND::VariableType::Area ||
		rtype == BYOND::VariableType::Turf ||
		rtype == BYOND::VariableType::Obj ||
		rtype == BYOND::VariableType::Mob)
		luabridge::push(L, *static_cast<BYOND::DatumObject*>(&result));
	return 1;
}


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

bool try_get_var(BYOND::DatumObject* src, std::string varname, BYOND::Object* result)
{
	if(!src->HasVariable(varname))
	{
		return false;
	}
	*result = src->GetVariable(varname);
	return true;
}

int BYOND::DatumObject::lua_getvar(lua_State* state)
{
	const char* varname = lua_tostring(state, 2);
	BYOND::Object result;
	if(!try_get_var(this, varname, &result))
	{
		luabridge::push(state, LuaObjectProcCallProxy(this, varname));
		return 1;
	}
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
		BYOND::DatumObject bdo = luabridge::Stack<BYOND::DatumObject>::get(state, 3);
		ths->Set(varname, bdo.Type(), (unsigned int)bdo.value);
	}
}

int BYOND::DatumObject::lua_setvar(lua_State* state)
{
	__try
	{
		set(this, state);
	}
	__except (1)
	{
		//MessageBoxA(NULL, "Caught server crashing error!!", "holy carp", NULL);
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
	luaopen_string(L);
	luaopen_math(L);
	getGlobalNamespace(L)
		.beginNamespace("vars")
		.beginClass<BYOND::DatumObject>("Atom")
		.addCFunction("__index", &BYOND::DatumObject::lua_getvar)
		.addCFunction("__newindex", &BYOND::DatumObject::lua_setvar)
		.endClass()
		.beginClass<LuaObjectProcCallProxy>("ProcCallProxy")
		.addCFunction("__call", &LuaObjectProcCallProxy::call)
		.endClass()
		.addFunction("mymob", getmymob)
		.addFunction("sleep", sleep)
		.endNamespace();

	if(luaL_dofile(L, "testscript.lua") != LUA_OK)
	{
		vars.CallGlobalProc("message_admins", { BYOND::Object("LUA ERROR: " + std::string(lua_tostring(L, -1))) });
	}

}