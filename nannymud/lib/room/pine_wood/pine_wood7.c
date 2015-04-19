#include <std_mudlib.h>
inherit I_ROOM;

reset(arg) {
  if (!arg) {
    set_light(1);
    add_property("outdoors");
    add_property("authour", "brom");
    add_exit("east", "pine_wood8");
    add_exit("south", "pine_wood4");
    set_short("A clearing");
    set_long
      ("You stand in a clearing surrounded by tall pine trees. In the middle of the\n"+
       "clearing is a small pond of still water. It is cool and silent here.\n");
    add_item( ({ "pine", "pines", "pine tree", "pine trees", "tree", "trees" }),
	     "Tall and strong they grow towards the sky. You feel small, looking at\n"+
	     "those timeless giants.\n");
    add_item("clearing",
	     "It is a small open place in the woods. Here it is cool and silent.\n");
    add_item( ({ "pond", "small pond", "water", "still water" }),
	     "In the middle of the clearing is a small pond of still water. It is\n"+
	     "surrounded by dark green moss. The water is not very clear.\n");
    add_item("moss",
	     "Green, moist velvet surrounding the small pond.\n");
  }
} /* reset */
