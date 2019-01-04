#pragma once
namespace BYOND
{
	class Variables {
	public:
		enum ObjectType {
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
		enum VariableType {
			Integer = 0x2a,
			String = 0x60
		};
		Variables();
		~Variables();
		int ReadVariable(char type, int datumId, int varNameId);
		int SetVariable(ObjectType type, int datumId, int varNameId, VariableType varType, DWORD new_value);

	};
};

