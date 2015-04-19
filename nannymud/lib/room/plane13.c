/* Earendil -- July 18, 95 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("outside");

    set_short("Open Plains");
    set_long(LB("You stand at the foot of a tall, craggy mountain range.  "+
	"The rocks rise up before you, too steep to climb.  To the east, "+
	"a swift stream cascades down from the mountains and widens into "+
	"a river flowing southwards.  The plains extend to the horizon to "+
	"the south and west."));

    add_item(({"river", "stream"}), LB("Several separate streams tumble " +
	"down the mountainside and here form a wide but slow river.  "+
	"Its waters are dark from rich soil here, but it is fine to "+
	"drink."));
    add_item(({"range", "mountain", "mountains"}), LB("The mountain range "+
	"cuts sharply across the northlands, dividing the world of Nanny.  "+
	"Few know what lies on the otherside of the mountains, for "+
	"its paths are treacherous and steep and few have climbed over "+
	"its expanse."));
    add_item(({"plain", "plains"}), LB("The plains stretch out to the south "+
	"and west.  They are dotted here and there with castles and "+
	"ancient ruins."));

    add_exit("west", "/room/plane11");
    add_exit("south", "/room/plane8");
  }
}

void init() {
  add_action("_east_verb", "east");
  add_action("_drink_verb", "drink");
  add_action("_climb_verb", "climb");
  ::init();
}

int _drink_verb(string s) {
  if( s == "water" || s == "from river" || s == "from water" ) {
    write("You drink some river water.  It is cold but very refreshing.\n");
    say(this_player()->query_name()+" drinks some water from the river.\n");
    this_player()->drink_soft(2);
    return 1;
  }
  notify_fail("Drink what?\n");
}

int _climb_verb(string s) {
  if( s == "mountains" || s == "mountain" || s == "range" || s == "up" ) {
    write("You climb up no more than 5 feet before you slide back down.\n");
    say(this_player()->query_name()+" attempts to climb the mountainside "+
	"but slides down\nafter a few feet.\n");
    return 1;
  }
  notify_fail("Climb what?\n");
}

int _east_verb() {
  write("The river becomes too wide here and is still to swiftly flowing\n"+
	"for you to cross.  However, to the south you see a ford.\n");
  say(this_player()->query_name()+" wishes to cross the river, but finds "+
	"it too deep and swift.\n");
  return 1;
}
