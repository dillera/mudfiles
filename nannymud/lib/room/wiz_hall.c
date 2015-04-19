/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "/room/room";
int lamp_is_lit;
object leo;

reset(arg) {
  if (!arg) {
    set_light(1);
    set_no_castle_flag(1);
    set_short("wizards hall");
    set_long("You are in the hall of the wizards.\n" +
	     "There is a door to the west and shimmering fields to the \n"+
	     "north and south.\n");
    add_exit("/room/quest_room","north",0,"wiz_check");
    add_exit("/room/castle_room","south",0,"wiz_check");
    add_exit("/room/elevator","west",0,"go_west");
    set_indoors();
  }

  if (!leo) {
    leo = clone_object("obj/leo");
    move_object(leo, this_object()); }

}

init() {
  add_action("hmm","exa");
  add_action("hmm","look at");
  add_action("west", "west");
  add_action("open", "open");
  add_action("close", "close");
  add_action("push", "push");
  add_action("push","press");
  add_action("north", "north");
  add_action("south", "south");
  ::init();
}

hmm(str) {
  string a,b;
  if (!str) return;
  if (sscanf(str,"%sdoor%s",a,b)==2) {
    if (call_other("room/elevator", "query_door_open") &&
	call_other("room/elevator", "query_level"))
      write("The door is open.\n");
    else
      write("The door is closed.\n"); 
    if (lamp_is_lit)
      write("The lamp beside the door is lit.\n"); 
    else write("There is a button beside the door.\n");
    return 1; }
  return;
}

wiz_check() {
  if (call_other(this_player(),"query_level") < 21) {
    write("A strong magic force stops you.\n");
    return 1; }
  else return 0;
}

open(str) {
  if (str != "door")
    return 0;
  if (call_other("room/elevator", "query_level", 0) != 1) {
    write("You can't when the elevator isn't here.\n");
    return 1; }
  call_other("room/elevator", "open_door", "door");
  return 1;
}

close(str) {
  if (str != "door")
    return 0;
  call_other("room/elevator", "close_door", "door");
    return 1;
}

go_west() {
  if (!call_other("room/elevator", "query_door_open") ||
      call_other("room/elevator", "query_level") != 1) {
    write("The door is closed.\n");
    return 1; }
  else return 0;
}

push(str){
  if (str && str != "button")
    return 0;
  if (call_other("room/elevator", "call_elevator", 1))
    lamp_is_lit = 1;
  return 1;
}

elevator_arrives() {
  say("The lamp on the button beside the elevator goes out.\n");
  lamp_is_lit = 0;
}

