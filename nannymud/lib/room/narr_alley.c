/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "room/room";
reset(arg) {

  if (arg) return;
  set_outdoors();
  set_light(1);
  add_exit("room/vill_road1","north");
  add_exit("room/bank","east");
  add_exit("room/new/narr_alley2","south");
  add_exit("/room/smithy", "west");
  set_short("Narrow alley");
  set_long("A narrow alley that continues south. A bank lies to the east.\n"+
	   "There is a well here.\n");
  add_item("well","You look down the well, but see only darkness.\n"+
                  "There are some iron handles on the inside.\n"+
	          "It should be possible to go down into it");
} 

init(arg) {
  add_action("go_down", "down");
  add_action("enter_well", "enter"); /* Added "enter well" 950712 Rohan */
  ::init(arg);
} 

enter_well(str) { 
  if (str=="well") return go_down();
  notify_fail("Enter what?\n");
  return 0;
}

go_down() {
  this_player()->move_player("down#room/well");
  return 1;
}

