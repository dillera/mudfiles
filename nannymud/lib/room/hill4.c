/* Earendil -- July 95 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(1);
  add_property("hills");

  set_short("Rolling Hills");
  set_long(LB("You are on the southern edge of the great hilly land "+
	"of the north, which stretches miles around you.  The hills are "+
	"mostly quiet and uninhabited.  A dark and narrow road leads "+
	"off to the south, where you can see the church's steeple in the "+
	"distance.  To the north lies the last remnant of the Old Wood."));

  add_item(({"wood", "old wood", "trees" }),
	LB("The Old Wood is part of a vast forest which covered all of "+
	"Nanny long, long ago, before men came and conquered this land.  "+
	"Now only scattered remnants exist, like the grove in the centre "+
	"of the northern hills.  It is said that a great power sleeps "+
	"within its borders."));
  add_item("hills", LB("The hills that surround you are quiet and "+
	"apparently largely uninhabited, except for insects and small "+
	"animals.  The skies are strangely empty of avians, and you "+
	"see no man-made structures around you.  You intuit an unexplained "+
	"spookiness in this forlorn place, however."));
  add_item(({"road","dark road"}), LB("The dark road that leads to the "+
	"south has an oppressive feel to it, even at this distance.  "+
	"Many rumours of dark and evil happennings have come from its "+
	"travellers."));
  add_item("mist", LB("The mist, which only obscures the distances, "+
	"lends to the loneliness of the land."));

  add_exit("north","old_wood");
  add_exit("east", "hill1");
  add_exit("south","dark_road");
  add_exit("west", "hill6");
}
