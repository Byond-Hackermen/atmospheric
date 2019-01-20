#pragma once
#define MOLES			1
#define ARCHIVE			2
#define GAS_META		3
#define META_GAS_SPECIFIC_HEAT	1
#define META_GAS_NAME			2
#define META_GAS_MOLES_VISIBLE	3
#define META_GAS_OVERLAY		4
#define META_GAS_DANGER			5
#define META_GAS_ID				6
#define META_GAS_FUSION_POWER   7
//ATMOS
//stuff you should probably leave well alone!
#define R_IDEAL_GAS_EQUATION	8.31	//kPa*L/(K*mol)
#define ONE_ATMOSPHERE			101.325	//kPa
#define TCMB					2.7		// -270.3degC
#define TCRYO					225		// -48.15degC
#define T0C						273.15	// 0degC
#define T20C					293.15	// 20degC

#define MOLES_CELLSTANDARD		(ONE_ATMOSPHERE*CELL_VOLUME/(T20C*R_IDEAL_GAS_EQUATION))	//moles in a 2.5 m^3 cell at 101.325 Pa and 20 degC
#define M_CELL_WITH_RATIO		(MOLES_CELLSTANDARD * 0.005) //compared against for superconductivity
#define O2STANDARD				0.21	//percentage of oxygen in a normal mixture of air
#define N2STANDARD				0.79	//same but for nitrogen
#define MOLES_O2STANDARD		(MOLES_CELLSTANDARD*O2STANDARD)	// O2 standard value (21%)
#define MOLES_N2STANDARD		(MOLES_CELLSTANDARD*N2STANDARD)	// N2 standard value (79%)
#define CELL_VOLUME				2500	//liters in a cell
#define BREATH_VOLUME			0.5		//liters in a normal breath
#define BREATH_PERCENTAGE		(BREATH_VOLUME/CELL_VOLUME)					//Amount of air to take a from a tile

//EXCITED GROUPS
#define EXCITED_GROUP_BREAKDOWN_CYCLES				4		//number of FULL air controller ticks before an excited group breaks down (averages gas contents across turfs)
#define EXCITED_GROUP_DISMANTLE_CYCLES				16		//number of FULL air controller ticks before an excited group dismantles and removes its turfs from active
#define MINIMUM_AIR_RATIO_TO_SUSPEND				0.1		//Ratio of air that must move to/from a tile to reset group processing
#define MINIMUM_AIR_RATIO_TO_MOVE					0.001	//Minimum ratio of air that must move to/from a tile
#define MINIMUM_AIR_TO_SUSPEND						(MOLES_CELLSTANDARD*MINIMUM_AIR_RATIO_TO_SUSPEND)	//Minimum amount of air that has to move before a group processing can be suspended
#define MINIMUM_MOLES_DELTA_TO_MOVE					(MOLES_CELLSTANDARD*MINIMUM_AIR_RATIO_TO_MOVE) //Either this must be active
#define MINIMUM_TEMPERATURE_TO_MOVE					(T20C+100)			//or this (or both, obviously)
#define MINIMUM_TEMPERATURE_DELTA_TO_SUSPEND		4		//Minimum temperature difference before group processing is suspended
#define MINIMUM_TEMPERATURE_DELTA_TO_CONSIDER		0.5		//Minimum temperature difference before the gas temperatures are just set to be equal
#define MINIMUM_TEMPERATURE_FOR_SUPERCONDUCTION		(T20C+10)
#define MINIMUM_TEMPERATURE_START_SUPERCONDUCTION	(T20C+200)

//HEAT TRANSFER COEFFICIENTS
//Must be between 0 and 1. Values closer to 1 equalize temperature faster
//Should not exceed 0.4 else strange heat flow occur
#define WALL_HEAT_TRANSFER_COEFFICIENT		0.0
#define OPEN_HEAT_TRANSFER_COEFFICIENT		0.4
#define WINDOW_HEAT_TRANSFER_COEFFICIENT	0.1		//a hack for now
#define HEAT_CAPACITY_VACUUM				7000	//a hack to help make vacuums "cold", sacrificing realism for gameplay

//FIRE
#define FIRE_MINIMUM_TEMPERATURE_TO_SPREAD	(150+T0C)
#define FIRE_MINIMUM_TEMPERATURE_TO_EXIST	(100+T0C)
#define FIRE_SPREAD_RADIOSITY_SCALE			0.85
#define FIRE_GROWTH_RATE					40000	//For small fires
#define PLASMA_MINIMUM_BURN_TEMPERATURE		(100+T0C)
#define PLASMA_UPPER_TEMPERATURE			(1370+T0C)
#define PLASMA_OXYGEN_FULLBURN				10

//GASES
#define MIN_TOXIC_GAS_DAMAGE				1
#define MAX_TOXIC_GAS_DAMAGE				10
#define MOLES_GAS_VISIBLE					0.25	//Moles in a standard cell after which gases are visible

#define FACTOR_GAS_VISIBLE_MAX				20 //moles_visible * FACTOR_GAS_VISIBLE_MAX = Moles after which gas is at maximum visibility
#define MOLES_GAS_VISIBLE_STEP				0.25 //Mole step for alpha updates. This means alpha can update at 0.25, 0.5, 0.75 and so on
