/area/area_type_1
	name = "area of type 1"
	var/area_variable_number = 5
	var/area_variable_string = "area variable of type string"
	var/list/area_variable_list = list("area variable list element 1", "area variable list element 2")

/area/area_type_2
	name = "area of type 2"

/area/area_type_1/proc/area_test_proc(arg)
	world << "Area test successful! Argument: [arg]"
	return arg

/turf/turf_type_1
	name = "turf of type 1"
	var/turf_variable_number = 5
	var/turf_variable_string = "turf variable of type string"
	var/list/turf_variable_list = list("turf variable list element 1", "turf variable list element 2")

/turf/turf_type_1/proc/turf_test_proc(arg)
	world << "Turf test successful! Argument: [arg]"
	return arg

/turf/turf_type_2
	name = "turf of type 2"

/obj/obj_type_1
	name = "obj of type 1"
	var/obj_variable_number = 5
	var/obj_variable_string = "obj variable of type string"
	var/list/obj_variable_list = list("obj variable list element 1", "obj variable list element 2")

/obj/obj_type_1/proc/obj_test_proc(arg)
	world << "Obj test successful! Argument: [arg]"
	return arg

/obj/obj_type_2
	name = "obj of type 2"

/mob/mob_type_1
	name = "mob of type 1"
	var/mob_variable_number = 5
	var/mob_variable_string = "mob variable of type string"
	var/list/mob_variable_list = list("mob variable list element 1", "mob variable list element 2")

/mob/mob_type_1/proc/mob_test_proc(arg)
	world << "Mob test successful! Argument: [arg]"
	return arg

/mob/mob_type_2
	name = "mob of type 2"

/datum/datum_type_1
	var/name = "datum of type 1"
	var/datum_variable_number = 5
	var/datum_variable_string = "datum variable of type string"
	var/list/datum_variable_list = list("datum variable list element 1", "datum variable list element 2")

/datum/datum_type_2
	var/name = "datum of type 2"

/datum/datum_type_1/proc/datum_test_proc(arg)
	world << "Datum test successful! Argument: [arg]"
	return arg

var/area/area_type_1/global_variable_area
var/turf/turf_type_1/global_variable_turf
var/obj/obj_type_1/global_variable_obj
var/mob/mob_type_1/global_variable_mob
var/datum/datum_type_1/global_variable_datum

var/global_variable_number = 5
var/global_variable_string = "global variable of type string"
var/list/global_variable_list = list("global variable list element 1", "global variable list element 2")
var/list/global_associative_list = list("associative list key 1" = "associative list value 1", "associative list key 2" = "associative list value 2")
var/unimportant_var = "nothing"

/world/New()
	global_variable_area = new
	global_variable_obj = new
	global_variable_mob = new
	global_variable_datum = new
	//no turf because of "bad loc" runtime
	global.vars["unimportant_var"] = 5 //ensure global variable names are not optimized away

/client/verb/init_dll()
	call("../asyncmos/asyncmos/debug/asyncmos.dll", "init")()

/client/verb/test_dll()
	call("../asyncmos/asyncmos/debug/asyncmos.dll", "test")()

/client/verb/call_test_procs()
	global_variable_area.area_test_proc(15)
	global_variable_obj.obj_test_proc(15)
	global_variable_mob.mob_test_proc(15)
	global_variable_datum.datum_test_proc(15)

/client/verb/access_ssoc_list()
	world.log << global_associative_list["associative list key 1"]