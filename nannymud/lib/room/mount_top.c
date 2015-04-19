/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("mountain");

  set_short("Top of mountain");
  set_long("You are on top of a mountain. \n"+
	   "There is a small plateau to the east.\n");
  add_exit("down","ravine");
  add_exit("east","mount_top2");
}
