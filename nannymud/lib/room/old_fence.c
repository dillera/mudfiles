#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    set_light(1);
    add_property("outdoors");
    add_property("bridge");
    add_property("author", "brom");
    add_exit("west", "bridge2");
    set_short("Small open place");
    set_long
      ("You stand in a small, open place. East of here is a rotting old fence,\n"+
       "and west is an old bridge. To the north a path leads into a forest.\n");
    add_item("fence",
	     "It is old, old, old and ready to come down any minute. Behind it\n"+
	     "is a unkept meadow.\n");
    add_item("meadow",
	     "No one has bothered to keep the meadow in shape, so the forest is\n"+
	     "re-taking the ground.\n");
    add_item( ({ "bridge", "old bridge" }),
	     "It leads west over the small river.\n");
    add_item("path",
	     "It is a narrow path, obviously not that well used.\n");
    add_item("forest",
	     "It is green and friendly.\n");
    /* Better safe than sorry... */
    if (!catch(load_object("players/zander/wpath3")) )
      add_exit("north", "/players/zander/wpath3");
  }
} /* reset */
