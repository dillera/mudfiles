#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
    {
      set_light(1);
      add_property("outdoors");
      add_property("road");
      add_property("authour", "brom");
      add_exit("north", "elm_road4");
      add_exit("south", "square");
      set_short("Elm road");
      set_long
        ("Behind the elms you see the old and worn houses of the village.\n"+
	 "Elm road continues north and south. To the south the road crosses\n"+
	 "a small square with a fountain.\n");

      add_item("square",
	       "It is a small cobbled square with a fountain on it.\n");
      add_item( ({ "house", "houses" }),
	       "The hoses are old but as well tended as the people here can\n"+
	       "afford. Paint is a rare luxuary here.\n");
      add_item( ({ "trees", "elms", "elm trees" }),
	       "On both sides of the road stands old elms, somewhat hiding the\n"+
	       "old houses behind.\n");
      add_item( ({ "tree", "elm", "elm tree" }),
	       "It's an old elm tree. Behind the tree a house hides.\n");
    }
} /* reset */
