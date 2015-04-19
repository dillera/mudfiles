/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(0);
  add_property("cave");

  set_short("Tunnel");
  set_long("In a tunnel leading through the mines.\n");
  add_exit("east","tunnel22");
  add_exit("west","tunnel20");
}

