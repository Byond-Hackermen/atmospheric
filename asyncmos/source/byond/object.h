#pragma once

#include "byond.h"
//#include "../scripting/lua_scripting.h"
#include <string>

#define As(x) As< ## x ## &>()

namespace BYOND
{
	class List;

	class Object
	{
	public:
		Object();
		Object(BYOND::VariableType type, int value);
		Object(float value);
		Object(std::string value);
		Object(struct temporary_return_value_holder ret);
		//Object(luabridge::LuaRef luaobj);
		friend bool operator==(const Object& lhs, const Object& rhs);
		friend bool operator!=(const Object& lhs, const Object& rhs);
		operator bool() const;
		BYOND::VariableType type;
		void* value;

		std::string AsString() const;
		float AsNumber() const;
		BYOND::List AsList() const;

		bool CheckValid() const;

		#pragma push_macro("As")
		#undef As
		template<typename T>
		T As()
		{
			return reinterpret_cast<T>(*this);
		}
		#pragma pop_macro("As")

		BYOND::VariableType Type() const;
	};
}
