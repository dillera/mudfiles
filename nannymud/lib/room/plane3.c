/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return; 

  set_long("You are standing on a large plain. You hear birds singing and\n"+
	   "the humming of insects flying around. The plain extends in all\n"+
	   "directions but to the east where you see some kind of a building.\n");
  add_item(({"building","ruin"}),"It looks like a ruin or something like that..");
  add_exit("south","plane2");
  add_exit("north","plane6");
  add_exit("east", "ruin");
  add_exit("west", "plane7");
}
