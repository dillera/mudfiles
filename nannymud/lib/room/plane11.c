/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;

  set_long("You are on a large plain. You hear birds singing and the \n"+
	   "humming of insects flying around. The plain continues to \n"+
	   "the south, east and to the west. To the north there is a\n"+
	   "large mountain.\n");
  add_item("mountain","Maybe you should check it out");

  add_exit("south","plane6");
  add_exit("north","mount_pass");
  add_exit("east", "plane13");
  add_exit("west", "plane12");
}
