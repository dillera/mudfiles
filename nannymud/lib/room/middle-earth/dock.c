inherit "/std/room";

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("realm", "Middle-Earth");
   
    add_exit("east", "dock_road");

    set_short("Dock");
    set_long("The dock reaches about 40 yards into the gentle waters "+
	"of the gulf of Lune, made of stout oak planks and posts.  There "+
	"is room for several boats to be moored here.  To the east is "+
	"the dock road which leads into the small township of the Grey "+
	"Havens.");
  }
}
