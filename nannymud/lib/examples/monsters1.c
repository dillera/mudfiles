inherit "std/room";

reset(arg) {
  ::reset(arg); /* You have to have this in order to make the objects return
                   at the next reset. */
  if (arg) return;
  
  add_property("indoors");
  set_light(1);
  set_short("Example room");
  set_long(
	   "Here are the first mosters. These two are quite simple.\n"+
	   "In these two only the most basic things are used.\n"+
	   "You will find more advanced monsters to the north.\n");
  
  add_exit("north","monsters2");
  add_exit("south","armours1");
  
  add_item( ({ "walls", "stone walls", "wall", "stone wall" }),
	   "The walls are made of stone and are bare.");
  add_item( ({ "floor", "wooden floor" }),
	   "The floor is made of rough wooden planks.");
  add_item( "ceiling",
	   "There are some spiderwebs in the ceiling.");
  add_item( ({ "spiderweb", "spiderwebs", "web", "webs" }),
	   "The spiderwebs in the ceiling are too high up for you to reach.");


  add_object("@make_rat()");
  add_object("@make_fairy()");
} /* reset */


make_rat() {
  /* This is the simplest monster you can make. */
  object rat;
  rat=clone_object("/std/basic_monster");
  rat->set_name("rat");
  rat->set_level(3);
  rat->set_race("rat");
  rat->set_short("A rat");
  rat->set_long("This is a big smelly rat.\n");
  rat->set_neuter();
  return rat;
} /* make_rat */

make_fairy() {
  /* This is a little more complicated monster. */
  object fairy;
  fairy=clone_object("/std/basic_monster");
  fairy->set_name("fairy");
  fairy->set_level(5);
  fairy->set_race("fairy");
  fairy->set_short("A fairy");
  fairy->set_long("It is a tiny little fairy fairy with wings.\n");
  fairy->add_item(({"wing","wings"}),
		  "The wings are transparent and almost invisible.\n");
  fairy->set_female();

  /* This fairy is a little tougher than it looks. */
  fairy->set_ac(3);
  fairy->set_wc(5);
  fairy->set_hp(120);

  return fairy;
} /* make_fairy */






