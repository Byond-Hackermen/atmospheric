#pragma once

#include "object.h"

namespace BYOND
{
	class Mob : public BYOND::Object
	{
	public:
		void SetVariable(std::string varName, VariableType varType, int new_value) const;
		void SetVariable(std::string varName, VariableType varType, float new_value) const;

		BYOND::Object GetVariable(std::string varName) const;
	};
}
