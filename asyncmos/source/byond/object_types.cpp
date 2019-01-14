#include "object_types.h"
#include "variables.h"


void BYOND::DatumObject::Set(std::string varName, VariableType varType, unsigned int new_value) const
{
	vars.SetVariable(static_cast<ObjectType>(type), reinterpret_cast<int>(value), varName, varType, new_value);
}

void BYOND::DatumObject::Set(std::string varName, VariableType varType, float new_value) const
{
	vars.SetVariable(static_cast<ObjectType>(type), reinterpret_cast<int>(value), varName, varType, new_value);
}

void BYOND::DatumObject::Set(std::string varName, std::string new_value) const
{
	Set(varName, VariableType::String, BYONDSTR(new_value));
}

BYOND::Object BYOND::DatumObject::GetVariable(std::string varName) const
{
	return vars.ReadVariable(static_cast<ObjectType>(type), reinterpret_cast<int>(value), varName);
}

BYOND::Object BYOND::DatumObject::Call(std::string procName, std::vector<Object> args)
{
	return vars.CallObjectProc(*this, procName, args);
}

BYOND::Object BYOND::DatumObject::Call(std::string procName)
{
	return vars.CallObjectProc(*this, procName);
}

BYOND::VariableAccessProxy& BYOND::DatumObject::operator[](std::string varName)
{
	VariableAccessProxy vap;
	vap.parent = this;
	vap.varName = varName;
	return vap;
}

BYOND::VariableAccessProxy::operator BYOND::Object()
{
	return parent->GetVariable(varName);
}

void BYOND::VariableAccessProxy::operator=(Object newValue)
{
	parent->Set(varName, newValue.Type(), reinterpret_cast<unsigned int>(newValue.value));
}