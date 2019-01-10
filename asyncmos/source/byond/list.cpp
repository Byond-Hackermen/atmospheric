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

BYOND::Object* BYOND::List::operator[](int index)
{
	return At(index);
}

BYOND::Object* BYOND::List::operator[](BYOND::Object why)
{
	for (int i = 0; i < Length(); i += 2) //assuming only the 0th, 2nd, 4th... values in the list may be keys
	{
		Object* potential_key = At(i);
		if (potential_key->Type() == why.Type() && potential_key->value == why.value)
		{
			return At(i + 1);
		}
	}
	return nullptr;
}