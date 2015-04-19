/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;

  set_long("You are standing on a large plain. You hear birds singing and\n"+
	   "the humming of insects flying around. The plain continues to the\n"+
	   "north and to the east.\n");
  add_exit("east", "plane2");
  add_exit("north","plane7");
}
