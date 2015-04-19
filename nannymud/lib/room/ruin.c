/* Rewritten using /std/room.c, Mats 960225 */
 
inherit "/std/room";
 
reset(arg) {
  ::reset(arg);
  if (arg) return;

  set_light(1);
  add_property("plain");

  set_short("At an old ruin");
  set_long("You are standing in an old ruin. There is no roof, and the walls\n"+
	   "are partly gone. There are either door nor windows. In all directions\n"+
	   "a large plain is extending.\n");

  add_item("plain","It's a normal plain with grass and flowers growing on it");
  add_item(({"wall","walls"}),"The walls are made by wood and are partly gone");
  add_item(({"window","windows"}),"There are no windows to be seen");
  add_item("door","It's not here. Maybe it's stolen");
  add_item("roof","It's gone");

  add_exit("south","plane4");
  add_exit("north","plane8");
  add_exit("east", "plane9");
  add_exit("west", "plane3");

  add_object("obj/plane_wolf");
}
