#pragma once

#include "byond.h"
#include <string>

namespace BYOND
{
	class Variables;
	class List;

	struct Object
	{
		BYOND::VariableType type;
		void* value;

		std::string AsString() const;
		float AsNumber() const;
		List* AsList() const;
		VariableType Type() const;
	};
}
