#pragma once

#include <string>

#define BYONDSTR(x) BYOND::Variables::stringTable[x]

namespace BYOND
{
	enum class ObjectType {
		Null		= 0x00,
		Turf		= 0x01,
		Obj			= 0x02,
		Mob			= 0x03,
		Area		= 0x04,
		Client		= 0x05,
		Image		= 0x0D,
		World		= 0x0E, //set datum id to null
		Global		= 0x0E,
		Datum		= 0x21,
		Savefile	= 0x23,
		List		= 0x54
	};

	enum class VariableType {
		Null		= 0x00,
		Mob			= 0x03,
		String		= 0x06,
		List		= 0x0F,
		Number		= 0x2A
	};

	enum class ProcType
	{
		Proc		= 0x02,
		Verb		= 0x15
	};

	std::string GetByondAbout();
	int GetByondVersion();
	int GetByondBuild();
}

