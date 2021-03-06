#pragma once

#include <string>
#include <typeindex>
#include <unordered_map>

#define BYONDSTR(x) BYOND::Variables::GetByondString(x)
#define INVALID_VAR_VALUE 0xDEADBEEF

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
		Path		= 0x26,
		List		= 0x54
	};

	enum class VariableType {
		Null		= 0x00,
		Turf		= 0x01,
		Obj			= 0x02,
		Mob			= 0x03,
		Area		= 0x04,
		String		= 0x06,
		World		= 0x0E,
		List		= 0x0F,
		Datum		= 0x21,
		Path		= 0x26,
		Number		= 0x2A,
	};

	enum class ProcType
	{
		Proc		= 0x02,
		Verb		= 0x15
	};

	struct temporary_return_value_holder
	{
		VariableType type;
		void* value;
	};

	std::string GetByondAbout();
	int GetByondVersion();
	int GetByondBuild();
}

