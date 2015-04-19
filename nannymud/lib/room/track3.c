/* Earendil -- July 95 */
/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(1);
  add_property("track");

  set_short("North Track");
  set_long(LB("The dirt track continues.  In front of you, the narrow "+
	"track widens and winds over the gently rolling hills of the "+
	"north.  The bustling village lies behind you to the south. Beside "+
	"the path small shrubs grow, and the songs of crickets are loud in "+
	"the air."));

  add_item("track", LB("Here the track widens to head over the hilly "+
	"lands of the north.  There is little vegetation here.  A slight "+
	"mist hangs over the edges of this land, which has a lonely, "+
	"folorn feel to it."));
  add_item("shrubs", LB("Small shrubs, the primary vegetation of this "+
	"forlorn land, grow about the sides of the track, and are sprinkled "+
	"over the hills to the north."));
  add_item("village", LB("The business and bustle of the village to the "+
	"south is left behind here in this lonely land, where human "+
	"habitation is far and in between."));
  add_item(({"cricket", "crickets"}), LB("Unseen but not unheard in their "+
	"natural home, they give these surroundings a serene feeling."));
  add_item("hills", LB("The wide rolling hills stretch several miles "+
	"around to the north.  It is a lonely land, that of these hills, "+
	"with few human habitants.  It is generally a peaceful land, but "+
	"that is only because the evil that lurks within it has little "+
	"been disturbed.  Tread softly on this ancient land."));
	
  add_exit("north","hill1");
  add_exit("south","track2");
}
