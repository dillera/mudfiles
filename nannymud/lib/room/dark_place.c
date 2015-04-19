/* Rewritten using add and set functions in room.c. Banshee 950217 */

inherit "room/room";

reset(arg) {
  if (arg)
    return;
  set_light(1);
  set_short("Behind the village church");
  set_no_castle_flag(1);
  set_long("You are standing on a dark and ominous place\n" +
	   "in the shadows directly behind the village church.\n" +
	   "You can enter a small dark temple to the east,\n" +
	   "and west is the ancient place where duels are fought.\n" +
	   "  A narrow road continues north.\n");
  add_exit("room/dark_temple", "east");
  add_exit("room/dark_duel_entrance", "west");
  add_exit("room/dark_road", "north");
  add_exit("room/church", "south");
  add_item("temple","It's the dark temple");
  add_item("church","It's the old village church");
  set_outdoors();
} /* reset */
