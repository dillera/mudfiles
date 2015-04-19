/* Rewritten using add and set functions in room.c. Banshee 950217 */

inherit "room/room";

reset(arg) {
  if (arg)
    return;
  set_light(1);
  set_short("Narrow road");
  set_long("You are standing on a dark and narrow road.\n" +
	   "To the north are the hills and to the south\n" +
	   "you can see the church.\n");
  add_exit("room/hill4", "north");
  add_exit("room/dark_place", "south");
  add_item("road","It looks very unused");
  set_outdoors();
} /* reset */

