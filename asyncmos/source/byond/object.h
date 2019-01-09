#pragma once

#include "byond.h"
#include <string>

namespace BYOND
{
	class List;

	class Object
	{
	public:
		Object();
		Object(BYOND::VariableType type, int value);
		Object(BYOND::VariableType type, float value);
		BYOND::VariableType type;
		void* value;

		std::string AsString() const;
		float AsNumber() const;
		BYOND::List* AsList() const;
		BYOND::VariableType Type() const;
	};
}
