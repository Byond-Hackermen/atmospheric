#pragma once

#include "object.h"
#include <vector>

namespace BYOND
{
	class Mob : public BYOND::Object
	{
	public:
		void SetVariable(std::string varName, VariableType varType, int new_value) const;
		void SetVariable(std::string varName, VariableType varType, float new_value) const;

		BYOND::Object GetVariable(std::string varName) const;

		BYOND::Object CallProc(std::string procName, std::vector<Object> args);
		BYOND::Object CallProc(std::string procName);
	};
}
