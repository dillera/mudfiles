/* Earendil -- July 95 */
/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(1);
  add_property("track");

  set_short("North Track");
  set_long(LB("This is a dirt track leading out of the village to the "+
	"hills to the north.  The sides of the track are lined with "+
	"small shrubs, and the track itself rutted from the passage of "+
	"carts and many feet.  Off to the west, the church rises up, but "+
	"there is no entrance from here."));

  add_item("church", LB("Standing vigil on the border of the westlands, the "+
	"church is the the most well-known building in the village and the "+
	"lands around it.  It is a tall, wooden structure, with no entrance "+
	"from this side."));
  add_item("track", LB("This track is the only safe travel to the "+
	"lands of the north, which are largely wild and uninhabited."));
  add_item(({"shrubs", "shrub"}), 
	LB("Small shrubs line either side of the track "+
	"you travel along.  The north hills have few large trees, and "+
	"this track foreshadows that land."));

  add_exit("north","track3");
  add_exit("south","vill_track");
}
