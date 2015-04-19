/* Earendil -- July 95 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(1);
  add_property("hills");

  set_short("Rolling Hills");
  set_long(LB("You stand on the edge of a great land of rolling hills.  "+
	"A wide track winds its way through here, heading south back to "+
	"the lands of men.  The hum of insects is heard in the air, lending "+
	"a natural feel to this otherwise quiet land.  The hills seem "+
	"empty, and a light mist shrouds the distance."));
  add_item("insects", LB("The sound of crickets and cicadas give the "+
	"only signs of activity here."));
  add_item("hills", LB("The hills that surround you are quiet and "+
	"apparently largely uninhabited, except for insects and small "+
	"animals.  The skies are strangely empty of avians, and you "+
	"see no man-made structures around you.  You intuit an unexplained "+
	"spookiness in this forlorn place, however."));
  add_item("track", LB("The dirt track leads back to the village to the "+
	"south.  It is the only safe road into this land, and thus the "+
	"most travelled, but that is only relative."));
  add_item("mist", LB("The mist, which only obscures the distances, "+
	"lends to the loneliness of the land."));
  add_exit("north","hill2");
  add_exit("west", "hill4");
  add_exit("south","track3");
}
