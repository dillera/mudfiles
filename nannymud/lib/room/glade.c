/* Earendil -- July 23, 95 */
/* Brom -- Aug 31, 95 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("outside");

    set_short("Dark Glade");
    set_long(LB("The woods open up a little here into a small glade.  Dark "+
	"shadows are cast about by the tall trees on the edge of the "+
	"glade, and you shiver at the thought of what may lurk within "+
	"them.  A narrow but well-trod path leads south; another less "+
	"trodden but equally narrow leads north."));

    add_item("trees", LB("The trees loom omniously before you at the "+
	"edge of the glade.  None have low-lying branches and none "+
	"can be climbed."));
    add_item("shadows", LB("The tall trees cast dark shadows around you.  "+
	"Though you stand in an open glade, it almost seems as if the "+
	"trees are pressing into the glade, trying to blot out all "+
	"sunlight."));
    add_item("path", LB("The south path or the north path, perhaps?\n"));
    add_item("south path",
	LB("It seems many people pass this way, perhaps "+
	"in search of adventure here in this dangerous land; for though "+
	"the path is narrow and bramble-filled, it is flattened out "+
	"by the passage of many adventurers."));
    add_item("north path",
	LB("The narrow path leading north is less well used than the one going"+
	   "south; perhaps the area to the north is less interesting or maybe"+
	   "it is just less well known."));
    add_exit("south", "/room/forest1");
    add_exit("north", "/room/pine_wood/pine_wood_track");
  }
}
