/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

string rope;

reset(arg) {
  if (arg) return;

  set_light(0);
  add_property("cave");

  set_short("Hole");
  set_long("There is a big hole here, and some kind of iron rings in the wall.\n" +
	   "It should be possible to pass the hole without falling down.\n");
  add_exit("north","tunnel4");
  add_exit("south","tunnel2");

  add_item(({"ring","rings"}),"A sturdy iron ring, fastened to the wall.");
  add_command(({"down","climb"}),"@down()");
}

down() {
  if (!rope) {
    write("You would fall down the hole and possibly hurt yourself.\n");
    return 1;
  }
  this_player()->move_player("down#room/mine/tunnel8");
  return 1;
}

tie(str) {
  if (str != "ring" && str != "rings")
    return 0;
  rope = 1;
  return 1;
}

untie(str) {
  rope = 0;
  return 1;
}

query_rope() {
  return rope;
}

