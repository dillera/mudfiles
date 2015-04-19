/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten to /std/room Rohan 960209 */

inherit "/std/room";
reset(arg) {
  ::reset(arg);
  if(arg) return;
  
  set_light(1);
  add_property("outdoors");
  set_short("Village track");
  set_long("A track going into the village. The track opens up to a road\n" +
	   "to the east and ends with a green lawn to the west. To the\n"+
	   "north is a track to the hills.\n");
  add_exit("west","/room/vill_green");
  add_exit("east","/room/vill_road1");
  add_exit("north","/room/track2");
  add_item("lawn",
	   "It's a green, but somewhat worn lawn.\n");
  add_item("tracks",
	   "Track leads of east, west and north from here.\n");
  add_item("track",
	   "The track looks well used.\n");
  add_item("hills",
	   "Far north you can see some rolling hills. Seems like an interesting place.\n");
  add_item("village",
	   "It looks like a happy little village from here.\n");
}
