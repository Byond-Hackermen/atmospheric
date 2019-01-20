
#include "..\byond\interface.h"
#include "..\byond\variables.h"
#include "..\byond\list.h"
#include "..\byond\object_types.h"
namespace Atmospherics {
	enum Defines
	{
		MOLES =			1,
		ARCHIVE =		2,
		GAS_META =		3,

		META_GAS_SPECIFIC_HEAT =	1,
		META_GAS_NAME =				2,
		META_GAS_MOLES_VISIBLE =	3,
		META_GAS_OVERLAY =			4,
		META_GAS_DANGER	=			5,
		META_GAS_ID	=				6,
		META_GAS_FUSION_POWER =		7,
	};

	void archive_mixture(BYOND::Datum mixture)
	{
		mixture.Set("archived_temperature", BYOND::VariableType::Number, mixture.Get<float>("temperature"));
		auto gases = mixture.Get<BYOND::List>("gases");
		for(int i=0; i<gases.Length(); i++)
		{
			*gases[i]->AsList()[ARCHIVE] = gases[i]->AsList()[MOLES]->AsNumber();
		}
	}

	void archive_turf(BYOND::Turf turf)
	{
		archive_mixture(turf.Get<BYOND::Datum>("air"));
		turf.Set("archived_cycle", BYOND::VariableType::Number, vars.ReadGlobalVariable("SSair").As(BYOND::DatumObject).Get<float>("times_fired"));
		turf.Set("archived_temperature", BYOND::VariableType::Number, turf.Get<float>("temperature"));
	}

	void ProcessCell(bool OpenTurf, float FireCount, BYOND::Turf turf) {
		BYOND::DatumObject SSair = vars.ReadGlobalVariable("SSair").As(BYOND::DatumObject);
		if (!OpenTurf) {
			SSair.Get<BYOND::List>("active_turfs").Remove(turf.type, reinterpret_cast<int>(turf.value));
			SSair.Get<BYOND::List>("currentrun").Remove(turf.type, reinterpret_cast<int>(turf.value));
			turf.Set("excited", BYOND::VariableType::Number, static_cast<unsigned int>(0));
			//TODO: add garbage_collect!!
			return;
		}
		auto pTurf = turf.As(BYOND::Turf);
		pTurf.Set("current_cycle", BYOND::VariableType::Number, FireCount);

		BYOND::List adjacentTurfs = pTurf.Get<BYOND::List>("atmos_adjacent_turfs");
		BYOND::Datum ourExcitedGroup = pTurf.Get<BYOND::Datum>("excited_group");
		int adjacentTurfsLength = adjacentTurfs.Length();
		int cachedAtmosCooldown = pTurf.Get<float>("atmos_cooldown") + 1;

		bool planetaryAtmos = pTurf.Get<float>("planetary_atmos");
		if (planetaryAtmos)
			adjacentTurfsLength++;

		BYOND::Datum ourAir = pTurf.Get<BYOND::Datum>("air");
		for (int i = 0; i < adjacentTurfsLength; i++) {
			BYOND::Turf enemyTile = adjacentTurfs[i]->As(BYOND::Turf);
			if (FireCount < enemyTile.Get<float>("current_cycle")) {
				continue;
			}
			archive_turf(enemyTile);
			bool shouldShareAir = false;

  /*if(our_excited_group && enemy_excited_group)
	  if(our_excited_group != enemy_excited_group)
		  //combine groups (this also handles updating the excited_group var of all involved turfs)
		  our_excited_group.merge_groups(enemy_excited_group)
		  our_excited_group = excited_group //update our cache
	  should_share_air = TRUE*/
			BYOND::Datum enemyAir = enemyTile.Get<BYOND::Datum>("air");
			BYOND::Datum enemyExcitedGroup = enemyTile.Get<BYOND::Datum>("excited_group");
			if (enemyExcitedGroup != ourExcitedGroup)
			{
				
			}
			
		}

		
	}
}