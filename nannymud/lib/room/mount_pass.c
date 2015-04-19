/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("mountain");

  set_short("Mountain pass");
  set_long("You are in a pass going into the mountain with a steep slope\n"+
	   "upwards to the north. It might be possible to climb up, though.\n"+
	   "There is a tunnel entrance to the north.\n");
  add_exit("north","mine/tunnel");
  add_exit("south","plane11");

  add_command(({"up","climb"}),"@up()");
}

up() {
  call_other(this_player(),"move_player", "up#room/ravine");
  return 1;
}
