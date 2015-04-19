/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("hills");

  set_short("Rolling hills");
  set_long("You are in some rolling hills. To the west is the old wood,\n" +
	   "while the hills continue in all other directions.\n");
  add_exit("north","hill3");
  add_exit("south","hill1");
  add_exit("west", "old_wood");
}

