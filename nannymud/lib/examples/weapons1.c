inherit "std/room";

reset(arg) {
  ::reset(arg); /* You have to have this in order to make the objects return
                   at the next reset. */
  if (arg) return;
  
  add_property("indoors");
  set_light(1);
  set_short("Example room");
  set_long(
	   "This is the weaponry, here you can see some different weapons:\n"+
	   "the very simple knife, the little more advanced club, and the\n"+ 
	   "quite complicated unique weapon, Leo's archsword.\n"+
           "Everything that could be done with the things can also be done\n"+
           "with the weapons. Here we only show what is special to weapons.\n");
  
  add_exit("south","objects1");
  add_exit("north","armours1");
  
  add_item( ({ "walls", "stone walls", "wall", "stone wall" }),
	   "The walls are made of stone and are bare.");
  add_item( ({ "floor", "wooden floor" }),
	   "The floor is made of rough wooden planks.");
  add_item( "ceiling",
	   "There are some spiderwebs in the ceiling.");
  add_item( ({ "spiderweb", "spiderwebs", "web", "webs" }),
	   "The spiderwebs in the ceiling are too high up for you to reach.");


  add_object("@make_knife()");
  add_object("@make_club()");
  add_object("leo_sword");
  
} /* reset */


make_knife() {
  /* This is the simplest weapon you can make. */
  object knife;
  knife=clone_object("/std/simple_weapon");
  knife->set_name("knife");
  knife->set_value(8);
  knife->set_weight(1);
  knife->set_class(3);
  knife->set_short("A knife");
  knife->set_long("This is a plain knife.\n");
  return knife; /* The function has to return the object. */
} /* make_knife */

make_club() {
  /* This is a little more complicated weapon. */
  object club;
  club=clone_object("/std/simple_weapon");
  club->set_name("club");
  club->set_value(80);
  club->set_weight(2);
  club->set_class(10);
  club->set_short("A club");
  club->set_long("It is a medium sized wooden club.\n");

  /* A club is a crush-weapon, this command gives it a little 
   different pre-defined hitlines. */  
  club->set_type("crush"); 

  club->add_property("wooden"); /* The club is made of wood. */

  return club; /* The function has to return the object. */
} /* make_club */






