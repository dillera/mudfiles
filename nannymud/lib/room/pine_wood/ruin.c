#include <std_mudlib.h>
inherit I_ROOM;

reset(arg) {
  if (!arg) {
    set_light(1);
    add_property("outdoors");
    add_property("authour", "brom");
    add_exit("south", "pine_wood9");
    set_short("At the ruin");
    set_long
      ("The trees here grow from the remains of an old villa. Some parts still stand,\n"+
       "like part of the chimney and the west wall.\n");
    add_item("chimney",
	     "About 2 yards remains of the chimney.\n");
    add_item( ({ "wall", "west wall" }),
	     "It is a sturdy brick wall. It must have taken the trees many years to\n"+
	     "reduce it to its current shape, which is mostly rubble.\n");
    add_item( ({ "brick", "bricks" }),
	     "Old and brittle, they crumble under your feet.\n");
    add_item( ({ "tree", "trees", "pine", "pines", "pine tree", "pine trees" }),
	     "The pines here are at least a hundred years old. This place has been\n"+
	     "deserted for a very long time.\n");
  }
} /* reset */
