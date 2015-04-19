/* Earendil -- July 23, 95 */
/* Rewritten using /std/room.c, Mats 960223 */

inherit "/std/room";

void reset(int arg) {
  ::reset(arg);
  if (arg) return;

  set_light(1);
  add_property("plain");

  set_short("Base of Giant Ash");
  set_long("You are walking beneath a gigantic ash tree, which sits by "+
    	"itself on the open plains.  Its boughs stretch high above you, "+
	"reaching hundreds of feet toward the sky.  Its leaves are rich "+
	"and green, and its bark sturdy looking.  There are a few "+
	"low-lying branches within reach.");

  add_item("branches","There are enough low-hanging branches that "+
	"it is possible to climb up the tree in relative safety.");
  add_item(({"tree", "ash"}), "The ash tree is magnificent.  Surely "+
	"some magic is at work, for no natural tree could reach the "+
	"heights that the one before you does.");
  add_item("plains", "The plains stretch back miles to the east.\n");

  add_exit("east","plane7");
  add_exit("west","giant_path");

  add_item_cmd("climb","tree","@climb_verb()");
  add_command("up","@climb_verb()");

  add_object("/obj/rope");
}

int climb_verb(string arg, string str) {
  return this_player()->move_player("up the tree", "/room/tree/tree1"), 1;
}
