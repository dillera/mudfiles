/* Rewritten using /std/room.c, Mats 960222 */

inherit "/std/room";

reset(arg) {
  if (arg) return;

  set_light(0);
  add_property("cave");

  set_short("Tunnel");
  set_long("@long_desc()");
  add_exit("east","tunnel14");
  add_exit("west","tunnel10");

  add_item("rope","@desc_rope()");
  add_command(({"up","climb"}),"@go_up()");
}

long_desc() {
  string l;
  l = "In a tunnel leading through the mines. There is a big hole in "+
      "the ceiling.\n";
  if ("/room/mine/tunnel3"->query_rope())
    l += "There is a rope hanging down through the hole.\n";
  return l;
}

go_up() {
  if ("/room/mine/tunnel3"->query_rope()) {
    this_player()->move_player("up#room/mine/tunnel8");
    return 1;
  }
  write("You can't go straight up without some kind of support.\n");
  return 1;
}

desc_rope() {
  if ("/room/mine/tunnel3"->query_rope()) {
    return "It looks possible to climb the rope.\n";
  }
  return "There is no rope here.\n";
}
