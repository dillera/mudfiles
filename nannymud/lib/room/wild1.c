/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten to /std/room 960204 Rohan */
/* Modified by Earendil 960223 to give interesting descriptions */  

/*inherit "/std/room";*/
inherit "/room/extra/time_room"; /* temporary--Earendil */

void reset(int arg) {
  if(arg) return;
  time_item_setup();
  add_property("outdoors");
  add_property("forest");
  add_property("author", "rohan");
  set_light(1);

  add_exit("east", "hump");
  add_exit("west", "forest1");

  set_short("Wilderness");
  set_long("This is the edge of the vast western wilderness on the far "+
	"outskirts of town.  To the east, beyond the river, you see the "+
	"village church, while to the west a forest of tall trees grows "+
	"thick and dense.");

  add_item(({ "forest", "trees" }), "The great western forest begins has its "+
	"east-most edge here.  The trees of the forest are tall, dark and "+
	"brooding.  You sense a hostility of the woods against you, against "+
	"all who walk on two legs.");
  add_item("village", "Only the church is still visible.");
  add_item(({ "church", "steeple" }), "The steeple of the village church "+
	"rises high enough that it can still be seen to the east.");
  add_item("river", "The snaking river winds down from the north, flows "+
	"under the humpbacked bridge to the east, and heads south toward "+
	"the ocean.");
}
