/* Rewritten using add and set functions in room.c. Banshee 950215 */

inherit "room/room";
int west_door_open;

reset(arg) {
  if (!arg) {
    set_light(1);	
    set_no_castle_flag(1);
    set_short("Room with black walls",);
    set_long("A room with black walls. There is a door to the east,\n" +
	     "and a door to the west.\n");
    add_exit("room/well","east",0,"check_east");		
    add_exit("room/sub/after_trap","west",0,"check_west"); }
  west_door_open=1;
}
	      
init() {
  add_action("open","open");
  add_action("close","close");
  add_action("hmm","exa");
  add_action("hmm","look at");
  ::init(); 
}

hmm(str) {
  string a,b;

  if (!str) return;
  if (sscanf(str,"%seast door%s",a,b)==2) {
    if (west_door_open) write("The east door is closed.\n");
    else write("The east door is open.\n"); 
    return 1; }
  if (sscanf(str,"%swest door%s",a,b)==2) {
    if (west_door_open) write("The west door is open.\n");
    else write("The west door is closed.\n"); 
    return 1; }
  if (sscanf(str,"%sdoor%s",a,b)==2) {
    write("Look at which door, the east or the west?\n");
    return 1; }
  return 0;
}

  
check_east() {
  if (!west_door_open) return 0;
  else {
    write("The east door is closed.\n");
    return 1; }
}

check_west() {
  if (west_door_open) return 0;
  else {
    write("The west door is closed.\n");
    return 1; }
}
  
open(str) {
  if (str!="door" && str!="west door" && str!="east door")
    return 0;
  write("There is no handle, and you can't push it up.\n");
  return 1;
}

close(str) {
  if (str!="door" && str!="west door" && str!="east door")
    return 0;
  write("There is no handle, and you can't push it closed.\n");
  return 1;
}

toggle_door() {
  write("You move the lever.\n");
  say(this_player()->query_name()+" pulled the lever.\n");
  if (west_door_open) {
    tell_room(this_object(), "The west door closed. The east door opened.\n");
    tell_room(environment(this_player()), "The west door opened.\n");
    west_door_open=0; } 
  else {
    tell_room(this_object(), "The west door opens. The east door closed.\n");
    tell_room(environment(this_player()), "The west door closed.\n");
    west_door_open=1; }
}

query_west_door() {
  return west_door_open;
}

















