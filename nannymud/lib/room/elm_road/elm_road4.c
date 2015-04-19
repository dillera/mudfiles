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
      add_exit("south", "elm_road3");
      set_short("Elm road");
      set_long
        ("Here the houses are better, the walls are painted and there's even\n"+
	 "glass in some windows. People stroll the street on different errands.\n"+
	 "Elm road goes south towards a small open place, and continues north.\n");
      add_item("people",
	       "The normal, happy people of Lars Town, going hither and yon\n"+
	       "on their daily tasks and chores.\n");
      add_item("road",
	       "It continues north and south. South is an open place.\n");
      add_item( ({ " place", "small place", "open place", "small open place" }),
	       "It looks like a small cobbled square.\n");
      add_item( ({ "house", "houses" }),
	       "The people here obviously spend some times to keep their houses\n"+
	       "in shape, even sweeping the road just outside their doors.\n");
      add_item( ({ "trees", "elms", "elm trees" }),
	       "On both sides of the road stands gnarled old elms, shading the\n"+
	       "nice houses behind.\n");
      add_item( ({ "tree", "elm", "elm tree" }),
	       "A gnarled old elm, throwing a shadow over the house behind it.\n");
    }
} /* reset */
