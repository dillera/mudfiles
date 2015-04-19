inherit "/std/room";

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("realm", "Middle-Earth");

    add_exit("north", "south_square");
    add_exit("south", "road2");

    set_short("Southern Archway");
    set_long("You stand under the southern archway leading from the "+
	"town of the Grey Havens.  There are no fortifications here, for "+
	"no enemies come from the West.  The cobblestone roadway leads "+
	"south from here to the ancient harbour and docks, built many ages "+
	"ago.");
  }
}
