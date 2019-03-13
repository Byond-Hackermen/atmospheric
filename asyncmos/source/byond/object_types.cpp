#include "object_types.h"
#include "variables.h"

BYOND::DatumObject::DatumObject(struct temporary_return_value_holder ret)
{
	this->type = ret.type;
	this->value = ret.value;
}

BYOND::DatumObject::DatumObject(ObjectType type, int value)
{
	this->type = static_cast<VariableType>(type);
	this->value = reinterpret_cast<void*>(value);
}

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

BYOND::Object BYOND::DatumObject::Call(std::string procName, std::vector<Object> args) const
{
	return vars.CallObjectProc(*this, procName, args);
}

BYOND::Object BYOND::DatumObject::Call(std::string procName) const
{
	return vars.CallObjectProc(*this, procName);
}

/*BYOND::Datum BYOND::DatumObject::NewDatum(std::string textpath)
{
	BYOND::Object path = vars.Text2Path(textpath);
	return BYOND::Object(vars.createNewDatum(3, 1, reinterpret_cast<int>(path.value), 0xffff)).As(BYOND::Datum);
}*/

std::vector<std::string> BYOND::DatumObject::GetVariableNames() const
{
	BYOND::List varnames = Get<BYOND::List>("vars");
	std::vector<std::string> result;
	for(int i=0; i<varnames.Length(); i++)
	{
		result.push_back(varnames.At(i)->AsString());
	}
	return result;
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