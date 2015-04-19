/* Rewritten using add and set functions in room.c. Banshee 950218 */
/* Rewritten to use /std/basic_room.c Rohan 950712  */

inherit "/std/basic_room";

reset(arg) {
  if(arg) return;
  add_property("outdoors");
  set_light(1);
  set_short("Village shore");
  set_long("The village shore. A jetty leads out to the east and a \n"+
	   "road starts to the west.\n");
  add_exit("west","/room/vill_road2");
  add_exit("east","/room/jetty");
  add_item("jetty","It's a stone jetty leading out into the sea. \n"+
	   "It seems kind of strange that there is no harbour near it.\n");
  add_item("road","The road leads to the village.\n");
  add_item("village","You can see the village to the west.\n");
}
