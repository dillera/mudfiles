#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("author", "brom");
    add_exit("west", "sewer2");
    add_exit("down", "sewer4");
    set_short("In the sewers");
    set_long
      ("You are swimming in a sea of refuse and faeces. You feel no bottom beneath\n"+
       "beneath you.\n");
    add_item( ({ "sea", "refuce", "faeces" }),
	     "You examine the liquid and the nearest turd. You feel ill.\n");
    add_item("water",
	     "There is no water here. This is a stagnant pool of the most revolting\n"+
	     "stuff imaginable.\n");
  }
} /* reset */
