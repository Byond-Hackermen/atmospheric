#include "list.h"
#include "variables.h"

#include <memory>

BYOND::List::List(int id) {
	ByondList* list = BYOND::Variables::getListPointer(id);
	this->internalList = list;
	this->ID = id;
}

BYOND::Object* BYOND::List::At(int i) const
{
	return internalList->At(i);
}

int BYOND::List::Length() const
{
	return internalList->length;
}

void BYOND::List::Append(VariableType type, int value) const
{
	BYOND::Variables::appendToContainer(VariableType::List, ID, type, reinterpret_cast<void*>(value));
}

void BYOND::List::Append(VariableType type, float value) const
{
	BYOND::Variables::appendToContainer(VariableType::List, ID, type, *reinterpret_cast<void**>(&value));
}

void BYOND::List::Remove(VariableType type, int value) const
{
	BYOND::Variables::removeFromContainer(VariableType::List, ID, type, reinterpret_cast<void*>(value));
}

void BYOND::List::Remove(VariableType type, float value) const
{
	BYOND::Variables::removeFromContainer(VariableType::List, ID, type, *reinterpret_cast<void**>(&value));
}

BYOND::Object* BYOND::List::operator[](int index) const
{
	return At(index);
}

std::unique_ptr<BYOND::Object> BYOND::List::operator[](BYOND::Object why) const
{
	return std::make_unique<BYOND::Object>(vars.GetContainerItem(BYOND::VariableType::List, ID, why));
}

bool BYOND::List::AssocHasKey(std::string key)
{
	return vars.CallObjectProc(BYOND::Object(BYOND::VariableType::List, ID), "Find", { BYOND::Object(key) } ).AsNumber() > 0;
}