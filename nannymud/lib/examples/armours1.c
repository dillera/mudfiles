inherit "std/room";

reset(arg) {
  ::reset(arg); /* You have to have this in order to make the objects return
                   at the next reset. */
  if (arg) return;
  
  add_property("indoors");
  set_light(1);
  set_short("Example room");
  set_long(
	   "This is the armoury, here you can see some different armours:\n"+
	   "a ring, a wristband, a shield and a hat.\n"+
	   "Everything that could be done with the things can also be done\n"+
	   "with the armours. Here we only show what is special to armours.\n"+
	   "There are eight different types of armour: helmet, amulet,\n"+
	   "armour, shield, ring, glove, cloak and boot.\n");

  add_exit("north","monsters1");
  add_exit("south","weapons1");
  
  add_item( ({ "walls", "stone walls", "wall", "stone wall" }),
	   "The walls are made of stone and are bare.");
  add_item( ({ "floor", "wooden floor" }),
	   "The floor is made of rough wooden planks.");
  add_item( "ceiling",
	   "There are some spiderwebs in the ceiling.");
  add_item( ({ "spiderweb", "spiderwebs", "web", "webs" }),
	   "The spiderwebs in the ceiling are too high up for you to reach.");


  add_object("@make_hat()");
  add_object("shield");
  add_object("wristband");
  add_object("ring");
  
} /* reset */


make_hat() {
  /* This is the simplest weapon you can make. */
  object hat;
  hat=clone_object("/std/simple_armour");
  hat->set_name("hat");
  hat->set_value(8);
  hat->set_weight(1);
  hat->set_class(1);
  hat->set_short("A hat");
  hat->set_Type("helmet");
  hat->set_long("This is a plain hat.\n");
  hat->add_property("cloth");
  return hat;
} /* make_hat */
