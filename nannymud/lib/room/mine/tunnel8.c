/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(0);
  add_property("cave");

  set_short("Shaft");
  set_long("You are hanging in a shaft leading deeper down in the mines.\n");
  add_exit("up",  "tunnel3");
  add_exit("down","tunnel9");
}

