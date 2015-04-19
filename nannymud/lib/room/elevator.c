/* Changed to /std/room and real properties by Rohan 950904 */

inherit "/std/room";

/*
 * This is an elevator.
 * floor 2: church
 */


#define STILL	0
#define DOWN	1
#define UP	2

int level;
int door_is_open;
int time_to_close_door;
int dest;		/* Where we are going. */

int moving_time;	/* How long we are going to move. */

int delay_to_reset;	/* Move back to origin automatically after a delay. */


reset(arg) {
  if (!arg) {
    set_light(1);
    set_short("An elevator");
    add_property("indoors");
    add_property((["realm":"NT"]));
    add_property("no_fight");
  }
  door_is_open = 1;
  level = 2;
  dest = 2;
  moving_time = 0;
  time_to_close_door = 5;
  set_heart_beat(1);
}

init() {
  add_action("press","press");
  add_action("press","push");
  add_action("open_door","open");
  add_action("close_door","close");
  add_action("go_east","east");
  add_action("hmm","exa");
  add_action("hmm","look at");
  ::init();
}

hmm(str) {
  string a,b;
  if (!str) return;
  if (sscanf(str,"%sdoor%s",a,b)==2) {
    if (door_is_open) {
      write("The door is open.\n");
      return 1; }
    if (!door_is_open) {
      write("The door is closed.\n"); 
      return 1; } }
  return 0;
}

press(str) {
  string a,b,button;
  if (!str) return 0;
  if (moving_time > 0) {
    write("The elevator is still moving.\n");
    return 1; }
  if (sscanf(str,"button%s",b)!=1) b=str;
  switch(b) {
  case "1": case "one":
    dest = 1; break;
  case "2": case "two":
    dest = 2; break;
  case "3": case "three":
    dest = 3; break; 
  default: write("Try pressing one of the buttons if you want the elevator to move.\n");
    dest=0;
    break; }
  if (dest==0) return 1;
  if (door_is_open) {
    write("You press the button, but nothing happens. You have to close the door first.\n");
    return 1; }
  if (dest == level) {
    write("You are already at level " + dest + ".\n");
    return 1; }
  if (dest > level) {
    moving_time = dest - level;
    write("The elevator jerks upward.\n");
    say("The elevator jerks upward.\n"); }
  if (level > dest) {
    moving_time = level - dest;
    write("The elevator starts moving downward.\n");
    say("The elevator starts moving downward.\n"); }
  if (dest == 1 || level == 1)
    moving_time += 10;
  moving_time += 1;
  set_heart_beat(1);
  return 1;
}

heart_beat() {
  if (time_to_close_door > 0) {
    time_to_close_door -= 1;
    if (time_to_close_door == 0) {
      say("The door swings shut.\n");
      door_is_open = 0; }
  }
  if (moving_time <= 0)
    return;
  if (--moving_time > 0) {
    say("The elevator continues...\n");
    return; }
  say("The elevator slows down and stops\n");
  set_heart_beat(0);
  level = dest;
  if (level == 3)
    call_other("room/attic", "elevator_arrives", 0);
  if (level == 2)
    call_other("room/church", "elevator_arrives", 0);
  if (level == 1)
    call_other("room/wiz_hall", "elevator_arrives", 0);
  return;
}

open_door(str) {
  if (str != "door") return 0;
  if (door_is_open) {
    write("The door is already open!\n");
    return 1; }
  if (moving_time > 0) {
    write("The door is stuck.\n");
    return 1; }
  door_is_open = 1;
  time_to_close_door = 5;
  set_heart_beat(1);
  write("Ok.\n");
  say(call_other(this_player(), "query_name", 0) + " opened the door.\n");
  return 1;
}
  
close_door(str) {
  if (str != "door")
    return 0;
  if (!door_is_open) {
    write("The door is already closed!\n");
    return 1; }
  door_is_open = 0;
  time_to_close_door = 0;
  write("Ok.\n");
  say(call_other(this_player(), "query_name", 0) + " closed the door.\n");
  return 1;
}

long() {
  write("You are in the elevator. On the wall are three buttons,\n"+
	"numbered 1, 2 and 3.\n"+
	"  There is a door to the east.\n");
  return 1; 
}

go_east() {
  if (moving_time > 0) {
    write("You can't go anywhere when the elevator is moving.\n");
    return 1; }
  if (!door_is_open) {
    write("The door is closed.\n");
    return 1; }
  if (level == 2)
    call_other(this_player(), "move_player", "east#room/church");
  if (level == 1)
    call_other(this_player(), "move_player", "east#room/wiz_hall");
  if (level == 3)
    call_other(this_player(), "move_player", "east#room/attic");
  return 1;
}

query_level() { return level; }

/*
 * This routine is called from various rooms that the elevator connects to.
 */

call_elevator(button) {
  if (door_is_open) {
    write("Nothing happens. Someone must have forgotten to close the door.\n");
    return 0; }
  if (moving_time > 0) {
    write("Nothing happens. The elevator is moving.\n");
    return 0; }
  dest = button;
  if (dest == level) {
    write("The elevator is already here.\n");
    return 0; }
  write("A little white lamp beside the button lights up.\n");
  say("A little white lamp beside the button lights up.\n");
  if (dest > level)
    moving_time = dest - level;
  if (level > dest)
    moving_time = level - dest;
  if (dest == 1 || level == 1)
    moving_time += 6;
  moving_time += 1;
  set_heart_beat(1);
  return 1;
}

/*
 * Only list inventory if not looking at anything special.
 */

can_put_and_get() {
  return 0;
}

/*
 * Called by others to see if the elevator is moving
 */
is_moving() { 
  if (level == dest )
    /* Still */
    return 0;
  if(level > dest)
    /* down */
    return 1;
  /* up */
  return 2;
}

query_door_open() { return door_is_open; } /* Returns 1 when door is open. */


   

