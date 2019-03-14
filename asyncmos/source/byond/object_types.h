#pragma once
#include "object.h"
#include "list.h"

#include <vector>
#include <lua.h>

namespace BYOND
{
	class VariableAccessProxy;
	class Datum;

	class DatumObject : public BYOND::Object
	{
	public:
		DatumObject() = default;
		DatumObject(struct temporary_return_value_holder ret);
		DatumObject(ObjectType type, int value);

		void Set(std::string varName, VariableType varType, unsigned int new_value) const;
		void Set(std::string varName, std::string new_value) const;
		void Set(std::string varName, VariableType varType, float new_value) const;

		BYOND::Object GetVariable(std::string varName) const;

		bool HasVariable(std::string name) const;

		int lua_getvar(lua_State* state);
		int lua_setvar(lua_State* state);

		template<typename T> T Get(std::string varName) const
		{
			return GetVariable(varName).As(T);
		}

		template<> float Get(std::string varName) const
		{
			return GetVariable(varName).AsNumber();
		}

		template<> std::string Get(std::string varName) const
		{
			return GetVariable(varName).AsString();
		}

		template<> BYOND::List Get(std::string varName) const
		{
			return GetVariable(varName).AsList();
		}

		BYOND::Object Call(std::string procName, std::vector<Object> args) const;
		BYOND::Object Call(std::string procName) const;

		//static BYOND::Datum NewDatum(std::string textpath, BYOND::DatumObject loc);
		//static BYOND::Datum NewDatum(std::string textpath);

		VariableAccessProxy& operator[](std::string varName);
	};

	class Area : public DatumObject {};
	class Turf : public DatumObject {};
	class Obj : public DatumObject {};
	class Mob : public DatumObject {};
	class Datum : public DatumObject {};

	class VariableAccessProxy
	{
	public:
		DatumObject* parent;
		std::string varName;
		operator BYOND::Object();
		void operator=(Object newValue);
	};
}
