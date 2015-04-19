/* Rewritten using add and set functions in room.c. Banshee 950218
   Added Traders Club, to reduce annoying shouts.
   Date: Sun Dec 10 00:26:22 1995 Taren and Brom.

   Changed to /std/room and spiced up Wed Mar 20 21:57:55 1996
   Perhaps, but WHO! and why didn't it work after!!!!
   /Taren Date: Sun Mar 24 17:43:00 1996
 */

inherit "/std/room";

void reset(int arg) {
  object jacket;

  if( !arg ) {
    set_light(1);
    add_property("forest");

    add_exit("east", "/room/wild1");
    add_exit("west", "/room/clearing");
    add_exit("north", "/room/glade");

    set_short("Edge of Forest"); /* Changed from set_long. Banshee 960321 */
    set_long("You are just inside the greenwood forest.  The leafy boughs "+
	"of tall oak and elm trees spread out above you, and only the "+
	"occasional beam of light makes its way through their thick canopy.  "+
	"Now and then you hear songs of birds and the stealthy movement of "+
	"forest animals, but few make themselves visible.  A path starts "+
	"here, leading west to a clearing, and then back into the forest.  "+
	"Beside the path is a canvas tent.");

    add_item(({ "tent", "canvas tent" }),
	"The tent belongs to some gypsy traders.  You can enter it.");
    add_item(({ "oak", "elm", "oaks", "elms", "tree", "trees" }),
	"The trees are tall and their trunks are thick and gnarled, but "+
	"none that you can see provide branches to climb.");
    add_item(({ "boughs", "canopy", "beam", }), "@day_or_night()");
    add_item(({ "birds", "animals" }), "They are too secretive to be "+
	"seen.");
    add_command("enter %s", "@enter_tent()");
  }
  if( !present("leather jacket") ) {
    tell_room(this_object(), line_break("You suddenly notice that someone has "+
	"dropped a leather jacket beside one of the trees."));
    move_object(jacket=(object)"/std/lib"->make("leather jacket"), 
	this_object());
    jacket->set_short("a leather jacket");
    jacket->add_alias("leather jacket");
  }
}

string day_or_night() {
  string desc;
  switch( (string) "/obj/daemon/timed"->_part_of_day(time(), 1, 1) ) {
    case "morning": case "day": case "noon": case "afternoon":
      desc = "The leafy boughs of the trees are too dense to see the sky.  "+
	"You can only now and then catch glimpses of the light of the sun.";
    default:
      desc = "The leafy boughs of the trees are too dense to see the sky.  "+
	"You can only now and then catch glimpses of the light of the stars "+
	"in the night sky, which is barely sufficient to illuminate your "+
	"way.";
  }
  return line_break(desc);
}

int enter_tent(string bah, string str) {
  if( str != "tent" )
    return notify_fail("Enter what?\n"), 0;
  this_player()->move_player("into the tent",
			     "/std/special/trade/trade_room");
  return 1;
}  
