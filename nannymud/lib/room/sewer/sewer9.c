/* +---------------------------------------------------------------------------------------+ */
#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("author", "brom");
    add_exit("east", "sewer10");
    add_exit("west", "sewer8");
    set_short("In the sewers");
    set_long
      ("The guck reaching to your knees slowly floats east here. From the east\n"+
       "also comes the sound of streaming water.\n");
    add_item("water",
	     "The filth slowly flows east here.\n");
    add_item("roof",
	     "Dirty water drips down from the cracks between the bricks.\n");
    add_item( ({ "wall", "walls" }),
	     "Some old and empty pipes jut out of the brick walls.\n");
    add_item( ({ "piep", "pipes" }),
	     "The old pipes are rat nests.\n");
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
