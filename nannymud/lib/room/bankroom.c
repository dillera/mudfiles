inherit "/std/room";

reset(arg) {
  ::reset(arg);
  if (arg) return;
  set_light(1);
  add_property("indoors");

  set_short("The president room");
  set_long("This is the office of the president of Aurora's first National Bank.\n"+
	   "There is a desk in the middle of the room.\n"+
	   "Unfortunately the president is not here right now.\n"+
	   "There is a door leading west.\n");

  add_item(({"floor","marble floor"}), 
           "A beautiful marble floor. It is made out\n"+ 
           "of expensive-looking white marble. It must have cost a fortune.\n");
  add_item(({"wall","walls"}),
           "The walls are painted in a dark blue colour with silver dots.\n");
  add_item("desk","The desk is made of mahogany and beside the desk is a chair with leather seat.\n"); /* Typo fixed. Banshee 960125 */
  add_item("door","@exa_door()");
  add_object("/obj/safe");
  add_command("west","@west()");
  add_item_cmd("open","door","@open()");
} /* reset */

exa_door() {
  string str;
  str="The looks like it is made of silver.\n";
  if ("room/bank"->query_door()) str+="The door is open.\n";
  else str+="The door is closed.\n";
  return str;
} /* exa_door */

west() {
  if ("room/bank"->query_door()) {
    write("The door is closed.\n");
    return 1;
  }
  this_player()->move_player("west","room/bank");
  return 1;
} /* west */

open(str) {
  if (!"room/bank"->query_door()) return 0;
  "room/bank"->open_door_inside();
  say(this_player()->query_name() + " opens the door.\n");
  write("Ok.\n");
  return 1;
} /* open */
