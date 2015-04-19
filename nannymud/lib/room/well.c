/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "room/room";

hmm(str){
  if (str=="door") {
    if (call_other("room/sub/door_trap", "query_west_door"))
      write("The door is closed.\n");
    else
      write("The door is open\n");
    return 1; }
}

init(){ 
    add_action("west", "west");
    add_action("open", "open");
    add_action("close", "close");
    add_action("pull", "pull");
    add_action("pull", "turn");
    add_action("pull", "move");
    add_action("hmm","exa");
    add_action("hmm","look at");
    ::init();
}

reset(arg){
 if(arg) return;
 set_short("Down the well");
 set_long("You are down in the well. It is wet and slippery.\n" +
	  "There is a door to the west and a lever beside it.\n");
 add_exit("room/maze1/maze1","north");
 add_exit("room/narr_alley","up");
 add_item("lever","The lever can be pulled");
 set_indoors();
}

west() {
  if (call_other("room/sub/door_trap", "query_west_door")==0) {
    this_player()->move_player("west#room/sub/door_trap");
    return 1; }
  write("The door is closed.\n");
  return 1;
}

close(str) {
  if (!str || str!="door")
    return 0;
  write("You can't.\n");
  return 1;
}

open(str) {
  if (!str || str!="door")
    return 0;
  write("You can't.\n");
  return 1;
}

pull(str) {
  if (!str || str!="lever")
    return 0;
  call_other("room/sub/door_trap","toggle_door");
  return 1;
}

