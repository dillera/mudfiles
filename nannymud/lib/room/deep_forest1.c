/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("forest");

  set_short("Deep forest");
  set_long("In the deep forest. The wood lights up to the east.\n");
  add_exit("east","plane12");
}

