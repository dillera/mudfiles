/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(0);
  add_property("cave");

  set_short("Stone table");
  set_long("In a tunnel leading into the mines.\n" +
	   "There is a big stone table here.\n");
  add_exit("north","tunnel_room");
  add_exit("south","tunnel4");
  add_item(({"table","stone table"}),"It's a big, very heavy table made of black stone");
  add_item_cmd("move","table","It's too heavy.\n");
}
