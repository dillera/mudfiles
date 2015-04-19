/* Rewritten using add and set functions in room.c. Banshee 950217 */

inherit "/room/room";

reset(arg) {
  if(arg) return;
  set_light(1);
  set_long("This is the monitor room where you can follow \n"
	   +"the current fight in the duel place.\n");
  set_short("In the monitor room");
  add_exit("room/dark_duel_entrance", "southeast");
  set_indoors();
}

init() {
  ::init();
  load_object("obj/duel_snooper");
}
