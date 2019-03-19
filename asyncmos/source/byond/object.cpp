#include "object.h"
#include "list.h"
#include "variables.h"
#include "../pocket/utilities.h"
#include <cassert>

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

BYOND::Object::Object(float value)
{
	this->type = BYOND::VariableType::Number;
	this->value = *reinterpret_cast<void**>(&value);
}

BYOND::Object::Object(std::string value)
{
	this->type = BYOND::VariableType::String;
	this->value = reinterpret_cast<void*>(BYONDSTR(value));
}

BYOND::Object::Object(struct temporary_return_value_holder ret)
{
	this->type = ret.type;
	this->value = ret.value;
}

bool BYOND::operator==(const BYOND::Object& lhs, const BYOND::Object& rhs)
{
	return lhs.type == rhs.type && lhs.value == rhs.value;
}

bool BYOND::operator!=(const BYOND::Object& lhs, const BYOND::Object& rhs)
{
	return !(&lhs == &rhs);
}

std::string BYOND::Object::AsString() const
{
	assert(Type() == BYOND::VariableType::String);
	return vars.GetStringFromId(reinterpret_cast<int>(value));
}

float BYOND::Object::AsNumber() const
{
	assert(Type() == BYOND::VariableType::Number);
	if (this == NULL) return 0;
	return Pocket::DwordToFloat(reinterpret_cast<DWORD>(value));
}

BYOND::List BYOND::Object::AsList() const
{
	assert(Type() == BYOND::VariableType::List);
	return vars.GetListFromId(reinterpret_cast<int>(value));
}

BYOND::VariableType BYOND::Object::Type() const
{
	return static_cast<VariableType>(static_cast<unsigned char>(type));
}

BYOND::Object::operator bool() const {
	return Type() != BYOND::VariableType::Null;
}

BYOND::Object::operator std::string() const {
	return AsString();
}

BYOND::Object::operator float() const {
	return AsNumber();
}

BYOND::Object::operator BYOND::List() const {
	return AsList();
}

bool BYOND::Object::CheckValid() const
{
	return !(Type() == BYOND::VariableType::Null && reinterpret_cast<unsigned int>(value) == INVALID_VAR_VALUE);
}