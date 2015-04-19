#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("author", "brom");
    add_exit("south", "sewer10");
    add_exit("west", "sewer6");
    set_short("In the sewers");
    set_long
      ("Water streams out from huge clay pipes in the north wall, taking a turn\n"+
       "towards the east. From the east you can hear the sound of a waterfall.\n");
    add_item("water",
	     "The water coming out of the pipes looks fresher than the sewage here.\n"+
	     "The mix is still far from clear, but the stench is less.\n");
    add_item("floor", 
	     "The floor is under about a yard of water.\n");
    add_item("roof",
	     "water drips from the rotten roof.\n");
    add_item( ({ "wall", "walls" }),
	     "The structure looks familiar: more rotten bricks.\n");
    add_item( ({ "pipe", "pipes", "clay pipe", "clay pipes" }),
	     "Nice workmanship! Made to last a life time or two, they lead water\n"+
	     "into the sewers from some unknown source. They are not big enough\n"+
	     "to enter.\n");
  }
} /* reset */
