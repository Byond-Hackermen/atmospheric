
#include "..\byond\interface.h"
#include "..\byond\variables.h"
#include "..\byond\list.h"
#include "..\byond\object_types.h"
#include "..\byond\object.h"
#include "..\Atmospherics\Atmospherics.h"
/*Copyright 2019 k-k-karma & McDonald072 - MIT

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
namespace Atmospherics {

	void archive_mixture(BYOND::Datum mixture)
	{
		mixture.Set("temperature_archived", BYOND::VariableType::Number, mixture.Get<float>("temperature"));
		auto gases = mixture.Get<BYOND::List>("gases");
		for (int i = 0; i < gases.Length(); i++)
		{
			//gases[i]->AsList()[MOLES]->AsNumber();
			BYOND::Object* asd = gases[i]->AsList()[ARCHIVE];
			if(asd != NULL)
			{
				asd->value = 0;
			}
		}
	}

	void archive_turf(BYOND::Turf turf)
	{
		archive_mixture(turf.Get<BYOND::Datum>("air"));
		turf.Set("archived_cycle", BYOND::VariableType::Number, vars.ReadGlobalVariable("SSair").As(BYOND::DatumObject).Get<float>("times_fired"));
		turf.Set("temperature_archived", BYOND::VariableType::Number, turf.Get<float>("temperature"));
	}

	void excited_group_reset_cooldowns(BYOND::Datum excited_group)
	{
		excited_group.Set("breakdown_cooldown", BYOND::VariableType::Number, static_cast<float>(0));
		excited_group.Set("dismantle_cooldown", BYOND::VariableType::Number, static_cast<float>(0));
	}

	void excited_group_add_turf(BYOND::Datum excited_group, BYOND::Turf turf)
	{
		excited_group.Get<BYOND::List>("turf_list").Append(turf.Type(), reinterpret_cast<int>(turf.value));
		turf.Set("excited_group", excited_group.Type(), reinterpret_cast<unsigned int>(excited_group.value));
		excited_group_reset_cooldowns(excited_group);
	}

	void excited_group_merge_groups(BYOND::Datum excited_group, BYOND::Datum other_group)
	{
		BYOND::Datum SSair = vars.ReadGlobalVariable("SSair").As(BYOND::Datum);
		if(excited_group.Get<BYOND::List>("turf_list").Length() > other_group.Get<BYOND::List>("turf_list").Length())
		{
			SSair.Get<BYOND::List>("excited_groups").Remove(other_group.Type(), (int)other_group.value);
			
		}
	}

	void ssair_add_to_active_simple(BYOND::Turf turf)
	{
		BYOND::Datum SSair = vars.ReadGlobalVariable("SSair").As(BYOND::Datum);
		turf.Set("excited", BYOND::VariableType::Number, (float)1);
		SSair.Get<BYOND::List>("active_turfs").Append(turf.Type(), (int)turf.value);
		//TODO: unfinished
	}

	void inline LastShareCheck(float LastShare, BYOND::DatumObject pTurf, float* CachedAtmosCooldown) {
		/*#define LAST_SHARE_CHECK \
	var/last_share = our_air.last_share;\
	if(last_share > MINIMUM_AIR_TO_SUSPEND){\
		our_excited_group.reset_cooldowns();\
		cached_atmos_cooldown = 0;\
	} else if(last_share > MINIMUM_MOLES_DELTA_TO_MOVE) {\
		our_excited_group.dismantle_cooldown = 0;\
		cached_atmos_cooldown = 0;\
	}*/
		BYOND::Datum ourExcitedGroup = pTurf.Get<BYOND::DatumObject>("our_excited_group").As(BYOND::Datum);
		if (LastShare > MINIMUM_AIR_TO_SUSPEND) {
			excited_group_reset_cooldowns(ourExcitedGroup);
			*CachedAtmosCooldown = 0;
		}
		else if (LastShare > MINIMUM_MOLES_DELTA_TO_MOVE) {
			ourExcitedGroup.Set("dismantle_cooldown", BYOND::VariableType::Number, (float)0);
			*CachedAtmosCooldown = 0;
		}
	}
	void ProcessCell(bool OpenTurf, float FireCount, BYOND::Turf turf) {
		BYOND::DatumObject SSair = vars.ReadGlobalVariable("SSair").As(BYOND::DatumObject);
		if (!OpenTurf) {
			SSair.Get<BYOND::List>("active_turfs").Remove(turf.type, reinterpret_cast<int>(turf.value));
			SSair.Get<BYOND::List>("currentrun").Remove(turf.type, reinterpret_cast<int>(turf.value));
			turf.Set("excited", BYOND::VariableType::Number, static_cast<unsigned int>(0));
			//TODO: add garbage_collect!!
			//nah
			return;
		}
		auto pTurf = turf.As(BYOND::Turf);
		pTurf.Set("current_cycle", BYOND::VariableType::Number, FireCount);

		BYOND::List adjacentTurfs = pTurf.Get<BYOND::List>("atmos_adjacent_turfs");
		BYOND::Datum ourExcitedGroup = pTurf.Get<BYOND::Datum>("excited_group");
		float adjacentTurfsLength = adjacentTurfs.Length();
		float cachedAtmosCooldown = pTurf.Get<float>("atmos_cooldown") + 1;

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
			if (ourExcitedGroup && enemyExcitedGroup) {
				if (enemyExcitedGroup != ourExcitedGroup)
				{
					ourExcitedGroup.Call("merge_groups", { enemyExcitedGroup });
					ourExcitedGroup = pTurf.Get<BYOND::Datum>("excited_group");
					shouldShareAir = true;
				}
			}
			/*
					else if(our_air.compare(enemy_air))
						if(!enemy_tile.excited)
							SSair.add_to_active(enemy_tile)
						var/datum/excited_group/EG = our_excited_group || enemy_excited_group || new
						if(!our_excited_group)
							EG.add_turf(src)
						if(!enemy_excited_group)
							EG.add_turf(enemy_tile)
						our_excited_group = excited_group
						should_share_air = TRUE*/
			else if (ourAir.Call("compare", { enemyAir} ).AsNumber()) {
				if (!enemyTile.GetVariable("excited").AsNumber()) {
					ssair_add_to_active_simple(enemyTile);
				}
				BYOND::Datum EG;
				if (ourExcitedGroup) {
					EG = ourExcitedGroup;
					excited_group_add_turf(EG, pTurf);
				}
				else if (enemyExcitedGroup) {
					EG = enemyExcitedGroup;
					excited_group_add_turf(EG, pTurf);
				}
				else {
					//FUCK NEW OBJECT
<<<<<<< HEAD
					excited_group_add_turf(EG, pTurf);
					excited_group_add_turf(EG, enemyTile);
=======
					EG = &vars.New("/datum/excited_group/excited_group").As(BYOND::DatumObject);
					EG->Call("add_turf", { pTurf });
					EG->Call("add_turf", { enemyTile });
>>>>>>> 0401b4884c70c5099cd4bd4b1fcef090a189b6b6
				}
				ourExcitedGroup = pTurf.Get<BYOND::Datum>("excited_group");
				shouldShareAir = true;
			}
			shouldShareAir = false;
			if (shouldShareAir) {
				BYOND::Object differenceObj = ourAir.Call("share", { enemyAir, adjacentTurfsLength });
				float difference = differenceObj.AsNumber();
				if (differenceObj) {
					if (difference > 0) {
						pTurf.Call("consider_pressure_difference", { enemyTile, difference });
					}
					else {
						enemyTile.Call("consider_pressure_difference", { pTurf, -difference });
					}
				}
				LastShareCheck(ourAir.GetVariable("last_share").AsNumber(), pTurf, &cachedAtmosCooldown);
			}


			/*
	if (planet_atmos) //share our air with the "atmosphere" "above" the turf
		var/datum/gas_mixture/G = new
		G.copy_from_turf(src)
		G.archive()
		if(our_air.compare(G))
			if(!our_excited_group)
				var/datum/excited_group/EG = new
				EG.add_turf(src)
				our_excited_group = excited_group
			our_air.share(G, adjacent_turfs_length)
			LAST_SHARE_CHECK

	our_air.react(src)*/
			if (planetaryAtmos) {
				BYOND::Datum newGasMixture = vars.New("/datum/gas_mixture").As(BYOND::Datum);
				newGasMixture.Call("copy_from_turf", { pTurf });
				archive_mixture(newGasMixture);
				if (ourAir.Call("compare", { newGasMixture }).AsNumber()) {
					if (!ourExcitedGroup) {
						BYOND::Datum newExcitedGroup = vars.New("/datum/excited_group").As(BYOND::Datum);
						newExcitedGroup.Call("add_turf", { pTurf });
						ourExcitedGroup = newExcitedGroup;
					}
					ourAir.Call("share", { newGasMixture, BYOND::Object(adjacentTurfsLength) });
					LastShareCheck(pTurf.GetVariable("last_share").AsNumber(), pTurf, &cachedAtmosCooldown);
				}

			}
			ourAir.Call("react", {pTurf});
			pTurf.Call("update_visuals");
			/*
	if((!our_excited_group && !(our_air.temperature > MINIMUM_TEMPERATURE_START_SUPERCONDUCTION && consider_superconductivity(starting = TRUE))) \
	|| (cached_atmos_cooldown > (EXCITED_GROUP_DISMANTLE_CYCLES * 2)))
		SSair.remove_from_active(src)*/
			if ((!ourExcitedGroup && !(ourAir.Get<float>("temperature") > MINIMUM_TEMPERATURE_START_SUPERCONDUCTION) && pTurf.Call("consider_superconductivity", { BYOND::Object(true) }).AsNumber()) || (cachedAtmosCooldown > (EXCITED_GROUP_DISMANTLE_CYCLES * 2)))
				SSair.Call("remove_from_active", { pTurf });
			pTurf.Set("atmos_cooldown", BYOND::VariableType::Number, cachedAtmosCooldown);
		}


	}
}