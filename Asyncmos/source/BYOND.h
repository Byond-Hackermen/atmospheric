#pragma once

#include <map>
#include <string>
#include <windows.h>
#include <DbgHelp.h>
#include <psapi.h>

#include "Pocket/Utilities.h"

#define BYONDSTR(x) (DWORD)BYOND::Variables::stringTable[x]

#define STRING_TABLE_PTR_OFFSET 

namespace BYOND
{
	enum class ObjectType {
		Turf = 0x1,
		Obj = 0x2,
		Mob = 0x3,
		Area = 0x4,
		Client = 0x5,
		Image = 0xD,
		World = 0xE, //set datum id to null
		Global = 0xE,
		List = 0x54,
		Datum = 0x21,
		Savefile = 0x23
	};

	enum class VariableType {
		Number = 0x2A,
		String = 0x06,
		List = 0x0F,
		Mob = 0x3,
	};

	struct Object;
	class List;
	struct ByondList;

	class Variables {
	public:
		Variables();
		~Variables();

		static std::map<std::string, int> stringTable;

		void GenerateStringTable();
		bool GetFunctionPointers();

		typedef void(__cdecl SetVariablePtr)(ObjectType type, int datumId, int varNameId, VariableType varType, void* newValue);
		typedef void(__cdecl GetVariablePtr)(ObjectType type, int datumId, int varNameId);
		typedef char**(__cdecl GetStringPointerFromIdPtr)(int stringId);
		typedef ByondList*(__cdecl GetListPointerPtr)(int listId);
		typedef void(__cdecl AppendToContainerPtr)(VariableType containerType, int containerId, VariableType varType, void* varValue);
		typedef void(__cdecl RemoveFromContainerPtr)(VariableType containerType, int containerId, VariableType varType, void* varValue);
		typedef void(__cdecl ReadVariablePtr)(ObjectType type, int datumId, int varNameId);

		static SetVariablePtr* setVariable;
		static GetVariablePtr* getVariable;
		static GetStringPointerFromIdPtr* getStringPointerFromId;
		static GetListPointerPtr* getListPointer;
		static AppendToContainerPtr* appendToContainer;
		static RemoveFromContainerPtr* removeFromContainer;
		static ReadVariablePtr* readVariable;

		DWORD* mob_list;

		char* GetCStringFromId(int id);

	public:
		BYOND::Object ReadVariable(ObjectType type, int datumId, std::string varName);
		void SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, DWORD new_value);
		void SetVariable(ObjectType type, int datumId, std::string varName, VariableType varType, float new_value);
		std::string GetStringFromId(int id);
		List* GetListFromId(int id);
	};

	struct Object {
		VariableType type;
		void* value;

		std::string AsString(Variables& v)
		{
			return v.GetStringFromId((DWORD)value);
		}

		float AsNumber()
		{
			return Pocket::DwordToFloat((DWORD)value);
		}

		VariableType Type()
		{
			return (VariableType)(BYTE)type;
		}
	};

	struct ByondList {
		Object* elements;
		DWORD unk1;
		DWORD unk2;
		DWORD length;
		DWORD refcount;
		DWORD unk3;
		DWORD unk4;
		DWORD unk5;

		Object* At(DWORD i)
		{
			if (i < 0 || i > length)
			{
				return nullptr;
			}
			return elements + i;
		}
	};

	class List
	{
		ByondList* internalList;
		DWORD ID;
	public:
		List(int id);

		Object* At(DWORD i);
		DWORD Length();
		void Append(VariableType type, DWORD value);
		void Append(VariableType type, float value);
		void Remove(VariableType type, DWORD value);
		void Remove(VariableType type, float value);
	};
};

