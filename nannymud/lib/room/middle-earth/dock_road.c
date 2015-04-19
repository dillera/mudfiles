inherit "/std/room";

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("realm", "Middle-Earth");

    add_exit("west", "pier2");
    add_exit("north", "road1");

    set_short("Dock Road");
    set_long("The road from the Grey Havens to the north ends here at "+
	"the docks of the harbour.  Gentle waves lap the shore to the "+
	"west; the water of the bay is soft and clear, and the calls of "+
	"distant gulls can be heard on the wind.");
  }
}
