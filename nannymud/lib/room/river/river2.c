inherit "std/room";
reset(arg) {
  if (arg) return;
  add_property("outdoors");
  add_property("water");
  set_light(1);
  add_exit("north","/players/akasha/akilles/stream2");
  add_exit("south","river1");
  set_short("Along the west side of a river");
  set_long(
    "You are walking the west bank of the river. On the other side are some\n"+
    "buildings. There is a ford here, so you can easily cross the river.\n");
  add_item(({"river","water"}), 
	   "This is the west bank of a river that flows north-south.");
  add_item("buildings",
    "You can't make out any details from where you are standing but\n"+
    "it looks a bit like a monestary.");
  add_item("ford",
    "It looks possible to cross.");
} /* reset */

init() {
  add_action("wade", "wade");
  add_action("wade", "east");
  add_action("wade", "e");
  add_action("wade", "cross");
  :: init();
}
 
wade(arg) {
  if (!arg || arg == "east" || arg == "river") {
    this_player()->move_player("east#players/ingis/path_2");
    return 1;
  }
  return 0;
}
 
