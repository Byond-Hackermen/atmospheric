#include "object_types.h"
#include "variables.h"


void BYOND::DatumObject::SetVariable(std::string varName, VariableType varType, int new_value) const
{
	vars.SetVariable(static_cast<ObjectType>(type), reinterpret_cast<int>(value), varName, varType, new_value);
}

void BYOND::DatumObject::SetVariable(std::string varName, VariableType varType, float new_value) const
{
	vars.SetVariable(static_cast<ObjectType>(type), reinterpret_cast<int>(value), varName, varType, new_value);
}

BYOND::Object BYOND::DatumObject::GetVariable(std::string varName) const
{
	return vars.ReadVariable(static_cast<ObjectType>(type), reinterpret_cast<int>(value), varName);
}

BYOND::Object BYOND::DatumObject::CallProc(std::string procName, std::vector<Object> args)
{
	return vars.CallObjectProc(*this, procName, args);
}

BYOND::Object BYOND::DatumObject::CallProc(std::string procName)
{
	return vars.CallObjectProc(*this, procName);
}