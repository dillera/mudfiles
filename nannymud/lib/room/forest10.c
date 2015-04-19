/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/forest_def";

reset(arg) {
  if(arg) return; ::reset(arg);
  add_exit("room/forest9", "west");
  add_exit("room/forest7", "east");
  set_short("Deep forest");
  set_long("You are in the deep forest. You can hardly see the sky through the trees.\n"+
	   "Now and then you hear birds singing and animals walking. To the west and\n"+
	   "to the east the forest continues.\n");
}
