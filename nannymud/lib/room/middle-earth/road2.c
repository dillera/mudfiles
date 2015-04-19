inherit "/std/room";

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("realm", "Middle-Earth");

    add_exit("south", "road1");
    add_exit("north", "city_arch");

    set_short("Road near Grey Havens");
    set_long("You are on a short road leading from the Havens to the "+
	"docks south of here.  The Bay of Lune is west a short distance; "+
	"the smell of brine and the calls of gulls hang in the air, mixed "+
	"with the sights and scents of the township north of here.");
  }
}
