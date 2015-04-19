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
      add_exit("south", "elm_road1");
      add_exit("north", "square");
      set_short("Elm road");
      set_long
        ("This is Elm Road. You are walking in a north-south slope with old\n"+
	 "elms flanking you on both sides. Behind the trees old houses hide.\n"+
	 "South is the village and to the north Elm road crosses a small square.\n");
      add_item("square",
	       "It is a small cobbled square with a fountain in the middle.\n");
      add_item("smoke",
	       "It is black and sooty, rising towards the sky.\n");
      add_item( ({ "smoke stack", "smoke stacks", "stack", "stacks" }),
	       "Made from white-washed bricks, the smoke stacks stand silent\n"+
	       "on the huncbacked roofs. Some smoke some, others not.\n");
      add_item( ({ "roof", "roofs", "hunchbacked roof", "hunchbacked roofs" }),
	       "From the grass on the roof, a silent smoke stack juts up.\n");
      add_item( ({ "wall", "walls" }),
	       "Even though the walls lack paint, they are solidly made from\n"+
	       "sturdy logs. Time has added a fine patina, and the wind has\n"+
	       "chiseled out a soft-spoken beauty.\n");
      add_item("doorstep",
	       "It is worn by the passage of many feet.\n");
      add_item("slope",
	       "The road slopes down towards the south and the central village.\n");
      add_item( ({ "house", "houses" }),
		"Most of the houses are old, with a hunch-backed roof and a worn doorstep,\n"+
		"but at least one seems to have been built recently.\n");
      add_item( ({ "trees", "elms", "elm trees" }),
	       "On both sides of the road stands old elms, a blessing both for\n"+
	       "eye and mind. Behind then old houses hide.\n");
      add_item( ({ "tree", "elm", "elm tree" }),
	       "It's an old elm tree. Behind the tree a house hides.\n");
    }
} /* reset */
