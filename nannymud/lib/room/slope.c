/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/room";

reset(arg) {
  if(arg) return;
  set_light(1);
  set_short("A slope");
  set_long("The forest gets light here, and slopes down to the west.\n");
  add_exit("room/orc_vall", "west");
  add_exit("room/forest2", "east");
  add_exit("room/forest3", "south");
  add_item("forest","Though the forest gives an impression of ancient calmness\n"+
	   "you can't help but feeling a bit worried about what lies ahead");
  add_item("slope","The slope leads down into a valley");
  add_item("valley","You can't see much of it, but it doesn't look nice from here");
  set_outdoors();
}

