#include "list.h"

#include "variables.h"


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
