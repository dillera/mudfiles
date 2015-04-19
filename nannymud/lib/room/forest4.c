/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/forest_def";

reset(arg) {
  if(arg) return; ::reset(arg);
  add_exit("room/forest3", "north");
  add_exit("room/forest5", "west");
  add_exit("room/forest6", "east");
  add_exit("room/forest7", "south");
  set_short("Deep forest");
  set_long("You are in the deep big forest. You can hardly see the sky through the trees.\n"+
	   "Now and then you hear birds singing and animals walking. The forest continues \n"+
	   "in all directions.\n");
}
