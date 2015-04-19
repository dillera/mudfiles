/* Earendil -- July 95 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

object beggar;

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("outdoors");

    set_short("Open Yard");
    set_long(LB("You stand in an open, grassy yard just north of the "+
	"main village road.  The small and cozy village pub lies to "+
	"the east.  The village continues to the north."));

    add_item("grass", "The lawn is made up of healthy, green grass.\n");
    add_item("pub",
	     LB("The village pub, The Rose and Dragon, is a place where you can "+
		"relax among friends and have a pint of the best beer Nanny "+
		"can offer.  Of course, there are stronger spirits as well.  "+
		"And drinking is good for you, too!"));
    add_item("village", LB("The main shops and businesses of the village "+
	"are to the south, but an older and run-down section lies north."));
    add_item("road",
	     "The primary road running through the village is south.\n");

    add_exit("east", "/room/pub2");
    add_exit("south", "/room/vill_road1");
    add_exit("north", "/room/elm_road/elm_road1");
  }
  if( !present("knife") ) 
    move_object("/std/lib"->make("knife"), this_object());
  if( !beggar )
    move_object(beggar=clone_object("/obj/beggar_outside_pub"), this_object());
}
