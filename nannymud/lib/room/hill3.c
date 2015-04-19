/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("hills");

  set_short("Rolling hills");
  set_long("You are in some rolling hills. They continue in all directions,\n"+
	   "but get steeper to the north.\n");
  add_exit("south","hill2");
  add_exit("west", "hill5");
}
