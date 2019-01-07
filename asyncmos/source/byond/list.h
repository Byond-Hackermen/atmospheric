#pragma once

#include "byond.h"

namespace BYOND
{
	struct Object;


	struct ByondList {
		Object* elements;
		int unk1;
		int unk2;
		int length;
		int refcount;
		int unk3;
		int unk4;
		int unk5;

		Object* At(int i)
		{
			if (i < 0 || i > length)
			{
				return nullptr;
			}

			return reinterpret_cast<Object*>(reinterpret_cast<unsigned long*>(elements) + i);
		}
	};

	class List
	{
		ByondList* internalList;
		int ID;
	public:
		List(int id);

		Object* At(int i) const;
		int Length() const;
		void Append(VariableType type, int value) const;
		void Append(VariableType type, float value) const;
		void Remove(VariableType type, int value) const;
		void Remove(VariableType type, float value) const;
	};
}