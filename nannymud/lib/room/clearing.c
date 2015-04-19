/*inherit "/std/basic_room";*/
inherit "/room/extra/time_room"; /* Temporary */

void reset(int arg) {
  if( !arg ) {
    time_item_setup();
    set_light(1);
    add_property("outdoors");

    add_exit("north", "/room/plane1");
    add_exit("east", "/room/forest1");
    add_exit("west", "/room/forest2");

    set_short("Clearing");
    set_long("The dark and wild forest opens up here into a small "+
	"but peaceful clearing.  The path again descends into the woods "+
	"to the west, but to the north the forest ends and a vast open "+
	"plain begins, stretching for miles.");

    add_item("plain", "The plain stretches for miles to the north, "+
	"ending finally at the foot of the Dragonspine mountains.");
    add_item("forest", "The forest is dark and eerily quiet, but "+
	"the clearing you stand in is peaceful and the air filled "+
	"with the sounds of birds and insects.");
    add_item("birds", "The birds sing merrily.\n");
    add_item("insects", "The hum of bees and the songs of crickets are "+
	"heard in the air.\n");
    add_item("path", "The path you are on leads east and west from "+
	"the clearing.\n");
    add_item(({"mountains", "mountain"}),
	"The Dragonspine mountains rise up in the far north.\n");
  }
}
