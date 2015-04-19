/* Rewritten using /std/room.c, Mats 960223 */

inherit "/std/room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("mountain");

  set_short("Lair of the Giant");
  set_long("You are standing in a giant's lair. There are mountains "+
	   "all around you.");
  add_exit("east","giant_path");
  add_exit("west","giant_conf");

  add_object("@/room/giant_conf->make_giant(small)");
}
