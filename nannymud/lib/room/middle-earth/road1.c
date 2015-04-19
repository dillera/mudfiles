inherit "/std/room";

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("realm", "Middle-Earth");

    add_exit("south", "dock_road");
    add_exit("north", "road2");

    set_short("Road toward Docks");
    set_long("This cobbled road leads down from the Grey Havens to the "+
	"north to the docks, now in sight just south of where you stand.  "+
	"To the west the ground drops down into the Bay of Lune, a "+
	"clear body of water which eventually empties into the great sea.");
  }
}
