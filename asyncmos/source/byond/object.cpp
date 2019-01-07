#include "object.h"

#include "variables.h"
#include "../pocket/utilities.h"


std::string BYOND::Object::AsString() const
{
	return vars.GetStringFromId(reinterpret_cast<int>(value));
}

float BYOND::Object::AsNumber() const
{
	return Pocket::DwordToFloat(reinterpret_cast<DWORD>(value));
}

BYOND::List* BYOND::Object::AsList() const
{
	return vars.GetListFromId(reinterpret_cast<int>(value));
}

BYOND::VariableType BYOND::Object::Type() const
{
	return static_cast<VariableType>(static_cast<unsigned char>(type));
}
