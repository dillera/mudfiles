/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/water_room";

reset(int arg) {
  ::reset(arg);
  if (!arg) {
    set_light(1);
    set_short("All at sea");
    set_long("You are swimming out in the sea.\n");
    property = "waterfilled";
    set_bottom("room/sea_bottom");
    add_exit("room/jetty","west");
    add_exit("room/sea_bottom","down");
    set_outdoors(); }
}

init() {
  ::init();
  add_action("dive", "dive");
}

dive(string str) {
  if (!str || str == "down") {
    this_player()->move_player("down#room/sea_bottom");
    return 1; }
}

