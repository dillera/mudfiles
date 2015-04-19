/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/forest_def";

reset(arg) {
  if(arg) return; ::reset(arg);
  add_exit("room/forest8", "west");
  add_exit("room/forest4", "east");
  set_short("Deep forest");
  set_long("You are in the deep big forest. You can hardly see the sky through the trees.\n"+
	   "Now and then you hear birds singing and animals walking. The forest expands\n"+
	   "to the west and to the east.\n");
}
