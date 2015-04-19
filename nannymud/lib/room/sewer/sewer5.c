#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("author", "brom");
    add_exit("north", "sewer1");
    add_exit("south", "sewer8");
    set_short("In the sewers");
    set_long
      ("The brick wall here is almost rotten. Water seeps through and down it,\n"+
       "joining the stinking liquid at your feet.\n");
    add_item( ({ "wall", "brick wall", "bricks" }),
	     "The brick wall is badly constructed and time has not been kind\n"+
	     "either. Water seeps through and down the bricks, thoroughly\n"+
	     "soaking the whole arrangement.\n");
    add_item("floor", "It is below a foot of stinking sewage.\n");
    add_item("roof",
	     "Bricks, this too, and dripping water.\n");
    add_item( ({ "water", "liquid" }),
	     "Water drips from the roof, and seeps from the walls, down into\n"+
	     "the stinking liquid on the floor.\n");
  }
  if (!present("rat"))
  {
    object r;
    int i;
    for (i=0; i<3; i++) 
    {
      r = "/room/sewer/sewer1" -> make_rat();
      move_object(r, this_object());
    }
  }
} /* reset */
