inherit "/std/room";

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("realm", "Middle-Earth");

    add_exit("south", "city_arch");
    add_exit("northeast", "city_central");

    set_short("South Square");
    set_long("You stand in the center of the southmost square of the "+
	"Grey Havens, exited on the south by an arch leading to the "+
	"harbour, and narrow alleys leading off in several other directions.  "+
	"A wide avenue heads northeast.");
  }
}
