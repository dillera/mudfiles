/* chapel.c
 *
 * Texts and most of the code by Catwoman.
 * The macros and some cleaning by Brom.
 * 
 */

inherit "std/room";
#define LB(X) sprintf("%-=78s\n", X)

object priest;

reset(arg)
{
  if (!arg)
  {
    
    add_property("indoors");
    add_property("no_magic");
    add_property("no_fight");
    add_property("holy_ground");
    set_light(1);
    
    add_exit("west", "church");
    
    set_short("Little Chapel 'O Love");
    set_long(LB("You have entered a small chapel with tall ceilings and big "+
		"stained glass windows. The room is light, and airy, and you "+
		"can see the dust particles floating around on the beams of "+
		"sunlight. On each side of a wide aisle, there are a few rows "+
		"of wooden pews. A white carpet is rolled out in the aisle. On "+
		"the end of each pew, there is a cluster of champagne colored "+
		"roses, tied together with white and silver ribbons and bows. "+
		"At the end of the aisle stands a small altar, covered with "+
		"ivory cloth, and decorated with more arrangements of roses. "+
		"As you stand here gazing at the little chapel, you realize it "+
		"is the perfect place to hold a wedding for two people who love "+
		"each other deeply."));
    
    add_item( ({ "window", "windows" }),
	     "These are large, stained glass windows, depicting the great "+
	     "Lords, Ladies, and Wizards in Nanny.");
    add_item( ({ "pew", "pews" }),
	     "The wooden pews are beautifully carved, and decorated "+
	     "with the small roses and ribbons.");
    add_item("altar",
	     "The altar is covered with a fine silk cloth, and decorated with "+
	     "white and silver flowers and candles.");
    add_item( ({ "candle", "candles" }),
	     "These are tall, thin candles, placed in silver candle holders. "+
	     "They burn with a soft, romantic light.");
    add_item( ({ "flower", "flowers" }),
	     "Beautiful bouquets of champagne, ivory, and white roses with "+
	     "baby's breath and greenery.");
    add_item( ({ "rose", "roses" }),
	     "Rosebuds that feel like the softest satin, in shades of ivory, "+
	     "champagne, and white.");
    add_item("carpet",
	     "A long, white carpet reaching from the stairs of the chapel to "+
	     "the altar at the front.");
    add_item("cloth",
	     "A long, ivory colored cloth, completely covering the altar.");
    add_item( ({ "ribbon", "ribbons" }),
	     "Thin pieces of white and silver ribbons, wrapped around the "+
	     "clusters of roses.");
    add_item( ({ "ceiling", "ceilings" }),
	     "As you look up, you notice just how high the oak ceiling is in "+
	     "the pretty little chapel.");
    add_item("aisle",
	     "This is a wide aisle, surrounded by the wooden pews, and covered "+
	     "by the long, white carpet.");
    add_item("bows",
	     "Little tiny bows that hold the ribbons together, made from the "+
	     "thinnest satin ribbon.");
  }

  if (!priest)
  {
    priest = clone_object("local/priest");
    move_object(priest, this_object());
  }

} /* reset */
