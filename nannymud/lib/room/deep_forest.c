/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "room/room";

reset(arg) {
  if (!arg) {
    set_light(1);
    set_short("Clearing");
    set_long("A small clearing. There are trees all around you.\n" +
	     "However, the trees are sparse to the north.\n");
    add_exit("room/forest1", "east");
    add_exit("room/forest2", "west");
    add_exit("room/plane1", "north");
    set_outdoors();
  }
}
