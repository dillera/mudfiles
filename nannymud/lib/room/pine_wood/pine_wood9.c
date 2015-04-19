#include <std_mudlib.h>
inherit I_ROOM;

object goblin;

reset(arg) {
  if (!arg) {
    set_light(1);
    add_property("outdoors");
    add_property("authour", "brom");
    add_exit("south", "pine_wood6");
    add_exit("west",  "pine_wood8");
    add_exit("north", "ruin");
    set_short("In the pine wood");
    set_long
      ("Here the pine wood is darker and colder. There are claw marks on the trees,\n"+
       "and the ground is littered with small bones.\n");
    add_item( ({ "tree", "trees", "pine", "pines" }),
	     "The pine trees reach for the heaven. From here it looks like they\n"+
	     "succeded in their grasping; you don't see the sky.\n");
    add_item( ({ "branch", "branches" }),
	     "Thick, and very high up.\n");
    add_item( ({ "crown", "crowns", "tree crown", "tree crowns" }),
	     "The tree crowns are very dense, putting this place into\n"+
	     "a chilly shadow, even at noon.\n");
    add_item( ({ "track", "faint track" }),
	     "It looks like an animal track here in the woods. It goes\n"+
	     "north and west.\n");
    add_item( ({ "mark", "marks", "claw mark", "claw marks" }),
	     "It looks like some creature or other used the trees for target\n"+
	     "practise with its claws.\n");
    add_item( ({ "bone", "small bone", "bones", "small bones" }),
	     "This is what remains of small animals after they've been eaten.\n");
    add_item("ground",
	     "It is covered with mixed vegetations, among which numerous small\n"+
	     "bones can be seen.\n");
  }
  if (!present("goblin")) {
    object item;

    goblin = "std/lib" -> make("goblin");
    item = "std/lib" -> make("shortsword");
    move_object(item, goblin);
    item = "std/lib" -> make("bag");
    move_object(item, goblin);
    move_object(goblin, this_object());
  }
} /* reset */
