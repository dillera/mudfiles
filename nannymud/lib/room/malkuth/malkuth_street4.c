#include <std_mudlib.h>
inherit I_ROOM;

reset(arg) 
{
  if (!arg)
  {
    add_property("outdoors");
    add_property("road");
    add_property("author", "brom");
    set_light(1);
    add_exit("north", "malkuth_street5");
    add_exit("south", "malkuth_street3");
    set_short("A narrow street");
    set_long
      ("The houses crowd in on the street here, making it more of an alley than\n"+
       "a street. It goes somewhat north-south, with the south being more friendly\n"+
       "and the north more gloomy.\n");
    add_item( ({ "house", "houses" }),
	     "The houses here have very small, usually barred, windows.\n");
    add_item( ({ "street", "alley" }),
	     "It gets even narrower to the north.\n");
  }
} /* reset */
