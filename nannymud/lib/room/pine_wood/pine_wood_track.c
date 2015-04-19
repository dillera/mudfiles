#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if(!arg) 
  {
    set_light(1);
    add_property("outdoors");
    add_property("track");
    add_property("authour", "brom");
    add_exit("south", "/room/glade");
    add_exit("north", "pine_wood2");
    set_short("Pine wood track");
    set_long
      ("The track goes north into the pine wood, and south into Lars Town.\n"+
       "A cold wind blows from the woods, carrying the scent of the pines\n"+
       "to you. It is rather chilly here.\n");
    add_item("track",
	     "Nothing much, mainly a trampled path.\n");
    add_item( ({ "town", "village" }),
	     "Lars Town is south from here. You can see the end of Elm road,\n"+
	     "and a small cobbled square.\n");
    add_item( ({ " pine wood", "wood" }),
	     "Pine wood is north of here, with its majestic old trees and peaceful\n"+
	     "air.\n");
    add_item( ({ "pine", "pine wood", "pines" }),
	     "Tall and majestic the pines raise their heads toward the sky. The\n"+
	     "wood is more than its part; here the mass of trees gives an air of\n"+
	     "tranquility and peace.\n");
  }
  if (!present("torch"))
    move_object(clone_object("obj/torch"), this_object());
} /* reset */
