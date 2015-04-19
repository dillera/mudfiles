#include <std_mudlib.h>
inherit I_ROOM;

object musician, instrument,note;

reset(arg) {
  if (!arg)
  {
    set_light(1);
    add_property("outdoors");
    add_property("author", "brom");
    add_property("road");
    add_exit("east", "malkuth_street1");
    add_exit("west", "malkuth_street3");
    set_short("Narrow street");
    set_long
      ("On both sides of the narrow street are small, un-interesting shops. The\n"+
       "windows are small and barred. The street continues west and east.\n");
    add_item( ({ "house", "houses" }),
	     "The houses holds un-interesting shops peddling things to the locals.\n"+
	     "A closed shop to the south has an old sign above it.\n");
    add_item( ({ "shop", "shops" }),
	     "The shops peddles things like household utensils to the locals. No\n"+
	     "adventurer with self respect would enter those. A shop to the south\n"+
	     "has an old sign above it, and looks closed.\n");
    add_item( ({ "old sign", "sign" }),
	     "It is old, with peeling paint. It says:\n"+
	     "  _________________________________________     ___________\n"+
	     " /                                  |_     ~~~~~           I#\n"+
	     " | (/)        xxxxXXXXXxx      ;                           \\_-_*\n"+
	     " |                       .    :                      x           |#\n"+
	     " |      Xx  X XXXx   xXXx   xXXx  Xx  X           x xX xXXx      |#\n"+
	     " |      Xx  X Xx  x Xx   X Xx   X XxX X           x X  Xxx       |#\n"+
	     " |       XxX  XXXx  Xx   XXXXXXXX Xx XX           x       Xx     |#\n"+
	     " |        X   Xx  x  XXXX   XXXX  Xx  X x     Xx Xx    xXXX      |#\n"+
	     " |                                                               |#\n"+
	     " | (/)                          .-,                          (/) |#\n"+     
	     " \\_______________________/L------------__________________________/#\n"+
	     "    ###############################################################\n");
	     
  }
  if (!present("__seth_the_musician"))
    make_musician();
} /* reset */

make_musician() {
  object m, o;

  m = clone_object("obj/smartmonster");
  m -> set_name("seth");
  m -> add_alias("musician");
  m -> add_alias("__seth_the_musician");
  m -> set_short("A street musician");
  m -> set_long
    ("This is Seth, the street musician. He is playing his shawm here in the street.\n"+
     "You would bet a gold coin that he wants some money, so why not give some gold\n"+
     "to him, rather than waste it on gambling?\n");
  m -> set_level(10);
  m -> set_male();
  m -> set_al(37);
  m -> set_give_money_handler(this_object(), "handle_give_money"); 
  m -> set_chat_chance(5);
  m -> add_chat("Seth blows his horn.\n");
  m -> add_chat("Seth plays a merry tune.\n"+
		"Suddenly, you begin to tap your right foot in rhythm.\n");
  m -> add_chat("Seth plays a nice tune.\n");
  m -> add_chat("Seth pipes a sentimental tune.\n");
  m -> add_chat("Seth shows off his skill at playing shawm.\n");
  m -> add_chat("Seth improvises wildly.\n");
  m -> add_chat("Seth plays a simple folk song.\n");
  m -> add_chat("Seth lets the music comfort your ear.\n");
  m -> add_attack("Seth blows his horn wildly at $OTHER!\n",
		  "Seth blow the shawm in your ear!\n", 10, 3);
  m -> add_attack("Seth groans: Why do you do this to me, $OTHER?\n", 0, 0, 5);
  m -> add_attack("Seth screams: NO! DON'T!\n", 0, 5, 0);
  m -> add_attack("Seth screams: NOT AGAIN!\n", 0, 5, 0);
  m -> add_attack("Seth shouts: You don't like free musicians?\n", 0, 0, 5);
  m -> add_attack("Seth stumbles around as $OTHER hits him!\n",
		  "Seth stumbles around as you hit him!\n", 0, 5);
  m -> add_attack("Seth begs for mercy, but $OTHER cruelly denies it.\n",
		  "Seth begs for mercy, but you cruelly deny him!\n", 0, 5);
  o = clone_object(I_TREASURE);
  o -> set_name("shawm");
  o -> set_value(12 + random(14));
  o -> set_short("a shawm");
  o -> set_long
    ("It is a music instrument, akin to a flute but with a reed pipe at the mouth\n"+
     "piece. It is made from horn, is about a foot in length and has 7 holes for\n"+
     "the players fingers.\n");
  move_object(o, m);
  move_object(m, this_object());
} /* make_musician */

handle_give_money(who_obj, who_name, nr_coins) {
  say("Seth says: Thank you, "+who_name+".\n"+
      "           I'm happy you like my music.\n");
} /* handle_give_money */
