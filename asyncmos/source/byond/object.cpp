#include "object.h"

#include "variables.h"
#include "../Pocket/Utilities.h"


std::string BYOND::Object::AsString(Variables& v) const
{
	return v.GetStringFromId(reinterpret_cast<int>(value));
}

float BYOND::Object::AsNumber() const
{
	return Pocket::DwordToFloat(reinterpret_cast<DWORD>(value));
}

BYOND::List* BYOND::Object::AsList(Variables& v) const
{
	return v.GetListFromId(reinterpret_cast<int>(value));
}

BYOND::VariableType BYOND::Object::Type() const
{
	return type;
}
