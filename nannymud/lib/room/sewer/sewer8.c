#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("author", "brom");
    add_exit("north", "sewer5");
    add_exit("east", "sewer9");
    set_short("In the sewers");
    set_long
      ("Here you are wading in knee-deep dung. The place smells like dead rats.\n");
    add_item("floor", 
	     "It is under about two feet of unmentionable slough.\n");
    add_item("roof", "It is dark, and it leaks on you.\n");
    add_item( ({ "wall", "walls" }),
	     "Slime from high tide has crawled about a yard up the walls.\n"+
	     "The rest is old brick and and rotting mortar.\n");
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
