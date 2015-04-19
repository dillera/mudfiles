/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(0);
  add_property("cave");

  set_short("Tunnel");
  set_long("The tunnel gets narrower here and\n"+
	   "you can see a steep path leading up.\n");
  add_exit("north","tunnel26");
  add_exit("up",   "tunnel24");
}
