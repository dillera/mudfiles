inherit "std/room";

reset(arg) {
  ::reset(arg); /* You have to have this in order to make the objects return
                   at the next reset. */
  if (arg) return;
  
  add_property("indoors");
  set_light(1);
  set_short("Example room 6");
  set_long(
	   "This is the sixth room in a series of examples of LPC code.\n" +
	   "It is a very simple room, but there are two things in it:\n" +
	   "a torch and a knife.\n" +
	   "They are not defined by the add_items command, but they are real\n" +
	   "objects that you can pick up, use and carry with you.\n" +
	   "The room will be re-stocked with these two items at every\n" +
	   "reset (which means every hour or so).\n" +
	   "You have come to a junction, to the west the you find the\n"+
           "simpler rooms, to the east are more advanced rooms, and to\n"+
           "the north begins the suite of rooms that teaches how to code\n"+ 
           "good objects like weapons and armours.\n");
  
  add_exit("west","room5");
  add_exit("north","objects1");
  add_exit("east","room7");
  
  add_item( ({ "walls", "stone walls", "wall", "stone wall" }),
	   "The walls are made of stone and are bare.");
  add_item( ({ "floor", "wooden floor" }),
	   "The floor is made of rough wooden planks.");
  add_item( "ceiling",
	   "There are some spiderwebs in the ceiling.");
  add_item( ({ "spiderweb", "spiderwebs", "web", "webs" }),
	   "The spiderwebs in the ceiling are too high up for you to reach.");


  /* These two lines makes a torch and a knife at every reset.
     It can either be defined by a filename or by calling a function. */
  add_object("/obj/torch");
  add_object("@make_knife()");
  
} /* reset */


make_knife() {
  object knife;
  knife=clone_object("/std/simple_weapon");
  knife->set_name("knife");
  knife->set_class(5);
  knife->set_value(8);
  knife->set_weight(2);
  knife->set_short("a knife");
  knife->set_long("This is a plain knife. Not very valuable,\n" +
		  "and moderately useful as a weapon.\n");
  return knife; /* The function has to return the object. */
} /* knife */

