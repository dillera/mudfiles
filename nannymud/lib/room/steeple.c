/* steeple.c -- Earendil, July 26, 1995 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("outside");
    add_property("high_altitude");

    set_short("Church Roof");
    set_long(LB("Church Roof\n" +
	"You stand atop the roof of the village church, beside a steeple "+
	"which rises high into the sky.  The view here is magnificent: "+
	"all four corners of Nanny are visible from here.  " +
	"It is a long drop to the ground from here.  The prefered exit "+
	"through a window, currently $WINDOW, which leads into "+
	"the church attic."));

    add_item("window", "@window_long()");
    add_item(({"ground", "drop"}),
	"It is a long way to the ground.  You could jump, but it is not "+ 
	"recommended.\n");
    add_item(({"nanny", "corners", "four corners", "corner", "view" }),
	LB("The four corners of the world can be seen from here.  You "+
	"might try looking at the north lands, the east lands, etc."));

    add_item(({"northlands", "northland", "north lands", "north"}),
 	LB("A small path winds north from the church through a "+
	"dark wooded area.  On the way it passes Earendil's castle "+
	"to the west, and leads on to a land of rolling hills, shrouded "+
	"in mist."));
    add_item("mist", "A soft mist hangs over the ancient hills to the "+
	"north.\n");
    add_item(({"rolling hills", "hills", }), LB("The ancient hills "+
	"to the north hold many secrets and much lore, but also "+
	"great unspoken dangers from another epoch."));
    add_item(({"castle", "earendil's castle", "earendils castle" }),
	LB("Earendil's castle rises up out of the surrounding "+
	"woods only a short distance to the northwest.  It is a "+
	"peaceful and cheery looking castle, built more as a "+
	"pleasant home than a fortress for defense.  Out of the "+
	"center rises a small tower."));
    add_item("tower", LB("The tower rising up out of Earendil's castle "+
	"must afford as nice a view of the landscape as the church "+
	"roof."));

    add_item(({"eastlands", "eastland", "east lands", "east"}),
	LB("The east holds the greatest mystery of all the "+
	"lands of Nanny, for there lies the Great Sea.  Across its "+
	"vast expanse few have explored.  Near the village "+
	"shore you can see Brom's island, a place of danger "+
	"and adventure."));
    add_item(({"island", "brom's island", "broms island"}),
	LB("Brom's island is a small island just off the shores "+
	"of the village.  In fact, it is but a short swim from "+
	"the jetty.  His castle takes up a large part of the "+
	"island; from there Brom lords over his feudal domains."));
    add_item("jetty", "The jetty is where ships arrive and leave.\n");

    add_item(({"southlands", "southland", "south lands", "south"}),
	LB("The lands to the south pass through the demesne of "+
	"Ingis and Chrisp, finally ending in the southern seas."));
	/* Boring south--do more later */


    add_item(({"westlands", "westland", "west lands", "west"}),
	LB("The great wilderness west of the village is a land "+
	"of greatly varied territory.  To the northwest the "+
	"Dragonspine mountains cut sharply across the lands;  who "+
	"know what lies beyond them?  Below the mountains almost "+
	"directly west (beyond the nearby Serpent river), "+
	"lie the open plains, where many castles and castle ruins "+
	"are found.  Leading west out of the village is a long "+
	"road."));
    add_item("road", LB("The village road leads from the jetty to "+
	"the east into town, across the humpbacked bridge, through "+
	"the wilderness south of the plains and eventually to the "+
	"orc valley."));
    add_item(({"valley", "orc valley"}),
	"A dark and evil place far in the westlands.\n");
    add_item("bridge", "An old, stone bridge crossing the Serpent "+
	"River.\n");
    add_item(({ "mountains", "mountain"}), LB("The Dragonspine "+
	"mountains are a mighty range of extremely tall "+
	"mountains that divide the far north from the south.  Few "+
	"journey into those frozen lands north of the Dragon's spine."));
    add_item("ruins", LB("Many are the castle ruins west of town, where "+
	"once law and order prevailed but now wild and evil creatures "+
	"have overcome and destroyed the glorious past."));
  
  }
}

void init() {
  ::init();
  add_action("_open_or_close_verb", "open");
  add_action("_open_or_close_verb", "close");

  add_action("_jump_verb", "jump");
  add_action("_jump_verb", "leap");

  add_action("_climb_verb", "climb");
}

int query_window();

string query_long(string str) {
  return replace(::query_long(str), "$WINDOW", 
	query_window() ? "open" : "closed");
}

int _climb_verb(string str) {
  if( str == "steeple" ) {
    write(LB("You attempt to climb the steeple, but only get about "+
	"a foot up, before you slide back down to your position on "+
	"the roof."));
    return 1;
  }
  if( str == "down" ) {
    write("There's no way to climb down safely.  You may jump, of course.\n");
    return 1;
  }
  notify_fail("Climb what?\n");
}

int query_window() { return (int)"/room/attic"->query_window(); }

string window_long() {
  return LB("The window is " + (query_window() ? "open" : "closed") + ".  "+
	"Through it you see the church attic.");
}

int _open_or_close_verb(string str) {
  string verb;

  verb = query_verb();
  if( str != "window" )
    return notify_fail(capitalize(verb) + " what?\n"), 0;
  if( query_window() == (verb == "open") ) {
    write("But the window is already " +
	(verb == "open" ? "open" : "closed") + ".\n");
    return 1;
  }
  write("You "+verb+" the window.\n");
  say(this_player()->query_name() + " " + verb + "s the window.\n");
  tell_room("/room/attic", 
	"Somebody on the church roof approaches the window.\n");
  "/room/attic"->set_window(verb == "open" ? "open" : "closed");
  return 1;
}

void redo_actions() {
  map_array(all_inventory(), "move_all_livings", this_object());
}

void move_all_livings(object o) {
  living(o) && move_object(o, this_object());
}

int _jump_verb(string str) {
  if( str && str != "off roof" && str != "off of roof" && str != "down" &&
	str != "off" )
    return notify_fail("Jump where?\n"), 0;
  write("Without bothering for your safety, you jump off the roof.\n");
  say(this_player()->query_name()+" suddenly and without warning "+
	"jumps off the roof.\n");
  write("Geronniimmoooooooooooooooo!!!\n");
  tell_room("/room/dark_place", 
	LB("From out of the sky, " + this_player()->query_name() + 
	" comes tumbling down, landing at your feet!\n"));
  this_player()->move_player("off the roof", "/room/dark_place");
  return 1;
}
