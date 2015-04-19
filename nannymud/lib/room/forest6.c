/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/forest_def";

reset(arg) {
  if(arg) return; ::reset(arg);
  add_exit("room/forest4", "west");
  set_short("Deep forest");
  set_long("You are in the deep big forest. You can hardly see the sky through the trees.\n"+
	   "Now and then you hear birds singing and animals walking. To the west, the\n"+
	   "forest continues.\n");
}
