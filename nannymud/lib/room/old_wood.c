/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";

reset(arg) {
  if(arg) return;

  set_light(1);
  add_property("forest");
  add_property("magical");

  set_short("Old Wood");
  set_long("This is the Old Wood. It is a dark and mysterious place, where\n"+
	   "ancient powers sleep and dream of their past might. You are\n"+
	   "standing in a small glade surrounded by high and old trees.\n");
  add_item(({"tree","trees"}),"The trees are very old and very straight");
  add_exit("north","hill5");
  add_exit("south","hill4");
  add_exit("west", "hill7");
  add_exit("east", "hill2");
}

