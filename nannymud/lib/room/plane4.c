/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;

  set_long("You are standing on a large open plain. You hear birds singing and\n"+
	   "the humming of insects flying around. To the west, the plain \n"+
	   "continues and to the east you see some kind of a building.\n");
  add_item(({"building","ruin"}),"It looks like a ruin");

  add_exit("north","ruin");
  add_exit("west", "plane2");
}
