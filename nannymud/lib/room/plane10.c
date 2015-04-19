/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;

  set_long ("You are on a large plain. You hear birds singing and the \n"+
	    "humming of insects flying around. The plain continues in\n"+
	    "all directions but west.\n");
  add_exit("north","plane12");
  add_exit("east", "plane6");
  add_exit("south","plane7");
}
