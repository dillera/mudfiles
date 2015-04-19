/* Rewritten using add and set functions in room.c. Banshee 950218 */
/* Rewritten to use /std/basic_room.c Rohan 950712  */
/* Exits changed by Leowon 951219, new exit to the south */

inherit "/std/basic_room";

reset(arg) {
  if (!present("bag"))
    move_object(clone_object("obj/bag"),this_object());
  if(arg) return;
  add_property("outdoors");
  add_property("water");
  set_light(1);
  set_short("Jetty");
  set_long("You are at a jetty. The waves roll in from the east.\n"+
           "A small path leads back to the west.\n");
  add_exit("east","/room/sea");
  add_exit("west","/room/vill_shore");
  add_exit("south","/room/east_shore1");
  add_item("path",
	   "The path leads back to the village.\n");
  add_item("village",
	   "You can't see the village from here.\n");
  add_item("jetty",
	   "It's a very well-built stone jetty which makes you \n"+
	   "wonder if there is a harbour around.\n");
  add_item(({"wave","waves"}),
	   "Quite ordinary waves splash against the jetty.\n");
}

