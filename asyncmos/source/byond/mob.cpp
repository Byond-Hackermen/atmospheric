#include "mob.h"

#include "variables.h"


void BYOND::Mob::SetVariable(std::string varName, VariableType varType, int new_value) const
{
	vars.SetVariable(BYOND::ObjectType::Mob, reinterpret_cast<int>(value), varName, varType, new_value);
}

void BYOND::Mob::SetVariable(std::string varName, VariableType varType, float new_value) const
{
	vars.SetVariable(BYOND::ObjectType::Mob, reinterpret_cast<int>(value), varName, varType, new_value);
}

BYOND::Object BYOND::Mob::GetVariable(std::string varName) const
{
	return vars.ReadVariable(BYOND::ObjectType::Mob, reinterpret_cast<int>(value), varName);
}

BYOND::Object BYOND::Mob::CallProc(std::string procName, std::vector<Object> args)
{
	return vars.CallObjectProc(*this, procName, args);
}

BYOND::Object BYOND::Mob::CallProc(std::string procName)
{
	return vars.CallObjectProc(*this, procName);
}