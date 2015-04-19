/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("hills");

  set_short("Rolling hills");
  set_long("You are in some rolling hills. They continue in all directions, but\n" +
	   "get steeper to the north. To the south you can see the old wood.\n");
  add_exit("east", "hill3");
  add_exit("west", "hill8");
  add_exit("south","old_wood");
}
