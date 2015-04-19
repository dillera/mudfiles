/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;

  set_long("You are standing on a large plain. You hear birds singing and\n"+
	   "the humming of insects flying around. The plain continues to all\n"+
	   "directions. To the west you see a large tree.\n");
  add_item("tree","It's a large tree");

  add_exit("south","plane5");
  add_exit("north","plane10");
  add_exit("east", "plane3");
  add_exit("west", "big_tree");
}
