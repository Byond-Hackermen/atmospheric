
#include "..\byond\interface.h"
#include "..\byond\variables.h"
#include "..\byond\list.h"
#include "..\byond\object_types.h"
namespace Atmospherics {
	void ProcessCell(bool OpenTurf, float FireCount, BYOND::Object turf) {
		BYOND::DatumObject SSair = vars.ReadGlobalVariable("SSair").As<BYOND::DatumObject>();
		if (!OpenTurf) {
			SSair.Call("remove_from_active", { turf });
			return;
		}
		BYOND::DatumObject pTurf = turf.As<BYOND::DatumObject>();
		pTurf.Set("current_cycle",BYOND::VariableType::Number, FireCount);
		/*	var/list/adjacent_turfs = atmos_adjacent_turfs
	var/datum/excited_group/our_excited_group = excited_group
	var/adjacent_turfs_length = LAZYLEN(adjacent_turfs)
	var/cached_atmos_cooldown = atmos_cooldown + 1*/
		BYOND::List adjacentTurfs = pTurf.Get<BYOND::List>("atmos_adjacent_turfs");
		BYOND::Datum ourExcitedGroup = pTurf.Get<BYOND::Datum>("excited_group");
		int adjacentTurfsLength = adjacentTurfs.Length();
		int cachedAtmosCooldown = pTurf.GetVariable("atmos_cooldown").AsNumber();
		/*	var/planet_atmos = planetary_atmos
			if (planet_atmos)
				adjacent_turfs_length++*/
		bool planetaryAtmos = pTurf.GetVariable("planetary_atmos").AsNumber();
		if (planetaryAtmos)
			adjacentTurfsLength++;
		/*	var/datum/gas_mixture/our_air = air

	for(var/t in adjacent_turfs)
		var/turf/open/enemy_tile = t

		if(fire_count <= enemy_tile.current_cycle)
			continue
		enemy_tile.archive()*/
		BYOND::Datum ourAir = pTurf.Get<BYOND::Datum>("air");
		for (int i = 0; i < adjacentTurfsLength; i++) {
			BYOND::DatumObject enemyTile = adjacentTurfs[i]->As(BYOND::DatumObject);
			if (FireCount < enemyTile.GetVariable("current_cycle").AsNumber()) {
				continue;
			}
			enemyTile.Call("archive");
			bool shouldShareAir = false;
			/*var/should_share_air = FALSE
  var/datum/gas_mixture/enemy_air = enemy_tile.air

  //cache for sanic speed
  var/datum/excited_group/enemy_excited_group = enemy_tile.excited_group

  if(our_excited_group && enemy_excited_group)
	  if(our_excited_group != enemy_excited_group)
		  //combine groups (this also handles updating the excited_group var of all involved turfs)
		  our_excited_group.merge_groups(enemy_excited_group)
		  our_excited_group = excited_group //update our cache
	  should_share_air = TRUE*/
			BYOND::Datum enemyAir = enemyTile.Get<BYOND::Datum>("air");
			BYOND::Datum enemyExcitedGroup = enemyTile.Get<BYOND::Datum>("excited_group");
			if (enemyExcitedGroup )
			
		}

		
	}
}