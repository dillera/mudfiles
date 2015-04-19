/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/room";

reset(arg) {
  if (!arg) {
    set_light(1);
    set_short("Plateau");
    set_long("You are on a large, open plateau on top of the mountain.\n" +
	     "The view is fantastic in all directions and the clouds\n" +
	     "that rush past above feels so close you could almost\n" +
	     "touch them. The air here is fresh and clean.\n");
    add_exit("room/mount_top", "west");
    set_outdoors();
  }
}
