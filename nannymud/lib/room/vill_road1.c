/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten to /std/room 960204 Rohan */

inherit "/std/room";

reset(arg) {
  if(arg) return;
  
  add_property("outdoors");
  set_light(1);
  set_short("Village road");
  set_long("A long road going east through the village. The road narrows \n"+
	   "to a track to the west and alleys lead north and south.\n");
  add_exit("west","/room/vill_track");
  add_exit("north","/room/yard");
  add_exit("south","/room/narr_alley");
  add_exit("east","/room/vill_road2");
  add_item("track",
    "The track leads to the village church.\n");
  add_item("road",
    "It's a wide long road going through village.\n");
  add_item(({"alley","alleys"}),
    "Looking north you can see the village pub and to the south \n"+
	   "lies, among other things the post office.\n");
}
