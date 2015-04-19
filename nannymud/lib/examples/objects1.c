inherit "std/room";

reset(arg)
{
  ::reset(arg); /* You have to have this in order to make the objects return
                   at the next reset. */
  if (arg) return;
  
  add_property("indoors");
  set_light(1);
  set_short("Example room");
  set_long(
	   "In this room the simplest of objects is show, a thing.\n" +
	   "It is based on /std/basic_thing and you cant make it much\n"+
           "more simple than this.\n");
  
  add_exit("south","room6");
  add_exit("north","weapons1");
  
  add_item( ({ "walls", "stone walls", "wall", "stone wall" }),
	   "The walls are made of stone and are bare.");
  add_item( ({ "floor", "wooden floor" }),
	   "The floor is made of rough wooden planks.");
  add_item( "ceiling",
	   "There are some spiderwebs in the ceiling.");
  add_item( ({ "spiderweb", "spiderwebs", "web", "webs" }),
	   "The spiderwebs in the ceiling are too high up for you to reach.");


  add_object("/examples/stone");
  add_object("@make_statue()");
  
} /* reset */


make_statue() {
  object statue;
  statue = clone_object("/std/basic_thing");
  statue -> set_name("statue");
  statue -> set_value(0);
  statue -> set_short("A statue");
  statue -> add_property("non-gettable"); /* It is not possible to get it */
  statue -> set_long("It is a statue of a brave knight. He is wearing\n"+ 
    "a platemail and wielding a sword.\n");

  /* Things can also have items. */
  statue -> add_item("platemail","It is a golden platemail with a sun engraved.");
  statue -> add_item("sword","It is the famous Excalibur.");
  statue -> add_item("sun","The sun is engraved into the platemail."); 

  return statue; /* The function has to return the object. */
} /* statue */
