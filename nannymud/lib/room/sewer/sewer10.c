#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("author", "brom");
    add_exit("north", "sewer7");
    add_exit("west",  "sewer9");
    set_short("In the sewers");
    set_long
      ("The water moves north here at a steady pace. The water is about a yard\n"+
       "deep here, with a slippery bottom.\n");
    add_item("floor", "It has a slippery feeling to it under the water.\n");
    add_item("roof", "It looks like it would collapse any day now.\n");
    add_item( ({ "wall", "walls" }),
	     "They are solidly built, but very old. Perhaps the bricks didn't start\n"+
	     "off as top-quality, either.\n");
    add_item("water",
	     "less filth and more water, but still mainly filth.\n");
  }
} /* reset */
