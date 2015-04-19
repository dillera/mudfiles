/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "/room/room";

reset(arg) {
  object o, o_next;
   /* sigh, let's do some cleaning /Profezzorn */
  for(o = first_inventory(); o; o = o_next)
  {
    o_next = next_inventory(o);
    if(!sscanf(file_name(o), "obj/player%*s"))
      destruct(o);
  }

  if (!arg)
  {
    set_light(1);
    set_no_castle_flag(1);
    set_short("The void");
    set_long("You come to the void if you fall out of a room, and have nowhere to go.\n"+
	     "If you give the command 'church' you will be transported there.\n");
    set_realm("NT");
  }
}
  
init() {
  add_action("church", "church");
  if(interactive(this_player()) && this_player()->query_level()<20) church();
}

church() {
  call_other(this_player(), "move_player", "away#room/church");
  write("You are transfered to the church...\n");
  return 1;
}

