inherit "/std/room";

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("realm", "Middle-Earth");

    add_exit("southwest", "south_square");
    add_exit("east", "east_square");

    set_short("Main Avenue");
    set_long("This wide avenue leads through the heart of the town of the "+
	"Grey Havens.  The stones are regular and the street is sparkling "+
	"clean, the first thing every traveller notices about this old "+
	"city.  The houses around the street are both modest and strikingly "+
	"beautiful, with tall shining walls of white and flowers springing "+
	"from the windowsills.  There is little poverty and unhappiness here, "+
	"it seems, even in this dark age.");
  }
}
