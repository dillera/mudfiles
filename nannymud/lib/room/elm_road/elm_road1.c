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
      add_exit("west", "/room/malkuth/malkuth_street1");
      add_exit("north", "elm_road2");
      add_exit("south", "/room/yard");
/*      add_exit("east", "/players/highlander/room/boot_shop"); */
        /* Added by Rohan 951002 */
      /* This was a copy of Ppannions old shop. and that one is already taken
	 over by Akasha. Why have the same shop in two places?
	 Banshee 951006 */
      /* Yep, but this one is the original one and it has been fixed by me instead
         of obscured by Akasha and it belongs to the righfull owner (Highlander).
         Unfortunatly it seems like someone have deleted the code under highlander
         and thereby destroying a lot of my work...
         Well, lets connect my own shop here then. *Mumble and grumble*
            /Rohan 951016
*/
      add_exit("east", "/players/rohan/watchmaker/shop");

      set_short("Elm road");
      set_long
	("Elm road starts here, continuing to the north. To the west is Malkuth\n"+
	 "street, and south is a yard. A pub can be seen there.\n");
      add_item( ({ "elm road", "road" }),
	       "Elm road starts here. It has gotten its name after the old elms\n"+
	       "growing on both sides of it.\n");
      add_item( ({ "tree", "trees", "elm", "elms" }),
	       "The elms are old, lending an air of dignity to the road.\n");
      add_item( ({ "malkuth street", "street" }),
	       "Malkuth street goes west from here. No one knows what \"Malkuth\"\n"+
	       "means, or who it was. The street is old and worn by many feet.\n");
      add_item("yard", "A small open place.\n");
      add_item("pub", "West of the small yard is a pub.\n");
    }
} /* reset */
