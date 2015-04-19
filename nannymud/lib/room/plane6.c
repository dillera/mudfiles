/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;
  set_long("You are walking on a large plain. You hear birds singing and\n"+
	   "the humming of insects flying around. The plain extends in all\n"+
	   "directions.\n");
  add_exit("south","plane3");
  add_exit("north","plane11");
  add_exit("east", "plane8");
  add_exit("west", "plane10");
}
