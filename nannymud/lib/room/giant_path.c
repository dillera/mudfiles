/* Rewritten using /std/basic_room.c, Mats 960223 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("plain");

  set_short("A path");
  set_long("You are on a path going in east/west direction. There are\n" +
	   "some VERY big footsteps here.\n");
  add_exit("east","big_tree");
  add_exit("west","giant_lair");
  add_item("footsteps","They are *VERY* big.\n");
}

