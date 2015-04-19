inherit "std/room";
reset(arg) {
  if (arg) return;
  add_property("outdoors");
  add_property("water");
  set_light(1);
  add_exit("east","/players/ingis/coast_1");
  add_exit("west","/players/ingis/coast_2");
  add_exit("northwest","/players/akasha/akilles/river5");
  add_exit("south","/players/akasha/akilles/delta1");
  set_short("Along the south coast");
  set_long(          
	   "This is the road along the south coast, passing the river\n"+
           "over a wooden bridge. The river floats from the north into\n"+
           "the deltalands in the south.\n");
  add_item(({"river","water"}), 
	   "The river is floating with constant speed into the deltaland.\n");
  add_item(({"road","stones"}),
	   "The road is made out of large flat stones.");
  add_item("bridge",
	   "It's just an ordinary wooden bridge flat and booring.");
} /* reset */


