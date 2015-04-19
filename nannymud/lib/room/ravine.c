/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("mountain");

  set_short("Ravine");
  set_long("You are in a ravine between mountains. It seems to be \n" +
	     "possible to go up from here.\n");
  add_exit("down","mount_pass");
  add_exit("up",  "mount_top");
}
