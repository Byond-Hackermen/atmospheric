#include "object.h"

#include "variables.h"
#include "../pocket/utilities.h"

BYOND::Object::Object()
{
	this->type = BYOND::VariableType::Null;
	this->value = 0;
}

BYOND::Object::Object(BYOND::VariableType type, int value)
{
	this->type = type;
	this->value = reinterpret_cast<void*>(value);
}

BYOND::Object::Object(BYOND::VariableType type, float value)
{
	this->type = type;
	this->value = *reinterpret_cast<void**>(&value);
}

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
