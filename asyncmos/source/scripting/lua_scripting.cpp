#include "lua_scripting.h"
#include <luaconf.h>
#include <fstream>

void lua_push_list(BYOND::Object o, lua_State* L)
{
	BYOND::List list = o.AsList();
	lua_newtable(L);
	for (int i = 0; i < list.Length(); i++) {
		BYOND::Object* thing = list.At(i);
		switch(thing->Type())
		{
		case BYOND::VariableType::Number:
			luabridge::push(L, thing->AsNumber());
			break;
		case BYOND::VariableType::String:
			luabridge::push(L, thing->AsString());
			break;
		case BYOND::VariableType::Area:
		case BYOND::VariableType::Turf:
		case BYOND::VariableType::Obj:
		case BYOND::VariableType::Mob:
		case BYOND::VariableType::Datum:
			luabridge::push(L, reinterpret_cast<BYOND::DatumObject*>(thing));
			break;
		case BYOND::VariableType::Null:
		default:
			luabridge::push(L, luabridge::Nil());
			break;
		}
		lua_rawseti(L, -2, i+1);
	}
}

void lua_push_byond_object(BYOND::Object o, lua_State* L)
{
	if(!o.CheckValid())
	{
		luabridge::push(L, luabridge::Nil());
		return;
	}
	BYOND::VariableType rtype = o.Type();
	if (rtype == BYOND::VariableType::Number)
		luabridge::push(L, o.AsNumber());
	else if (rtype == BYOND::VariableType::String)
		luabridge::push(L, o.AsString());
	else if (rtype == BYOND::VariableType::Null)
		luabridge::push(L, luabridge::Nil());
	else if (rtype == BYOND::VariableType::List)
		lua_push_list(o, L);
	else if (rtype == BYOND::VariableType::Area ||
		rtype == BYOND::VariableType::Turf ||
		rtype == BYOND::VariableType::Obj ||
		rtype == BYOND::VariableType::Mob ||
		rtype == BYOND::VariableType::Datum)
		luabridge::push(L, *static_cast<BYOND::DatumObject*>(&o));
}

std::vector<BYOND::Object> lua_get_call_args(lua_State* L)
{
	int num_args = lua_gettop(L);
	std::vector<BYOND::Object> args;
	for (int i = 2; i <= num_args; i++)
	{
		int type = lua_type(L, i);
		if (type == LUA_TNUMBER || type == LUA_TBOOLEAN)
			args.push_back(BYOND::Object(luabridge::Stack<float>::get(L, i)));
		else if (type == LUA_TSTRING)
			args.push_back(BYOND::Object(luabridge::Stack<std::string>::get(L, i)));
		else if (type == LUA_TNIL)
			args.push_back(BYOND::Object(BYOND::VariableType::Null, 0));
		else
			args.push_back(static_cast<BYOND::Object>(luabridge::Stack<BYOND::DatumObject>::get(L, i)));
	}
	return args;
}

LuaObjectProcCallProxy::LuaObjectProcCallProxy(class BYOND::DatumObject* src, std::string name)
{
	this->src = src;
	this->name = name;
}

int LuaObjectProcCallProxy::call(lua_State* L)
{

	BYOND::Object result = src->Call(name, lua_get_call_args(L));;
	lua_push_byond_object(result, L);
	return 1;
}

LuaGlobalProcCallProxy::LuaGlobalProcCallProxy(std::string name)
{
	this->name = name;
}

int LuaGlobalProcCallProxy::call(lua_State* L)
{
	BYOND::Object result = vars.CallGlobalProc(name, lua_get_call_args(L));
	lua_push_byond_object(result, L);
	return 1;
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
	lua_push_byond_object(result, state);
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

int LuaGlobal::lua_access_global(lua_State* L)
{
	const char* varname = lua_tostring(L, 2);
	BYOND::DatumObject GLOB = vars.ReadGlobalVariable("GLOB");
	BYOND::Object result = GLOB.GetVariable(varname);
	if(result.CheckValid())
	{
		lua_push_byond_object(result, L);
	}
	else
	{
		result = vars.ReadGlobalVariable(varname);
		if(result.CheckValid())
		{
			lua_push_byond_object(result, L);
		}
		else
		{
			luabridge::push(L, LuaGlobalProcCallProxy(varname));
		}
	}
	return 1;
}

void sleep(int milis)
{
	Sleep(milis);
}

void lua_to_world(std::string text)
{
	vars.CallGlobalProc("to_chat", { vars.world, text });
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
		.beginClass<LuaObjectProcCallProxy>("ObjectProcCallProxy")
		.addCFunction("__call", &LuaObjectProcCallProxy::call)
		.endClass()
		.beginClass<LuaGlobalProcCallProxy>("GlobalProcCallProxy")
		.addCFunction("__call", &LuaGlobalProcCallProxy::call)
		.endClass()
		.beginClass<LuaGlobal>("Glob")
		.addCFunction("__index", &LuaGlobal::lua_access_global)
		.endClass()
		.addFunction("sleep", sleep)
		.addFunction("to_world", lua_to_world)
		.addVariable("glob", new LuaGlobal(), false)
		.addFunction("mymob", getmymob)
		.endNamespace();

	if(luaL_dofile(L, "testscript.lua") != LUA_OK)
	{
		vars.CallGlobalProc("message_admins", { "LUA ERROR: " + std::string(lua_tostring(L, -1)) });
	}

}
