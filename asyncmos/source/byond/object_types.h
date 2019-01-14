#pragma once
#include "object.h"
#include "list.h"

#include <vector>

namespace BYOND
{
	class VariableAccessProxy;

	class DatumObject : public BYOND::Object
	{
	public:
		void Set(std::string varName, VariableType varType, unsigned int new_value) const;
		void Set(std::string varName, std::string new_value) const;
		void Set(std::string varName, VariableType varType, float new_value) const;

		BYOND::Object GetVariable(std::string varName) const;

		template<typename T> T Get(std::string varName)
		{
			return GetVariable(varName).As(T);
		}

		template<> float Get(std::string varName)
		{
			return GetVariable(varName).AsNumber();
		}

		template<> std::string Get(std::string varName)
		{
			return GetVariable(varName).AsString();
		}

		template<> BYOND::List Get(std::string varName)
		{
			return GetVariable(varName).AsList();
		}

		BYOND::Object Call(std::string procName, std::vector<Object> args);
		BYOND::Object Call(std::string procName);

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