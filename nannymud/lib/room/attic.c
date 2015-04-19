/* attic.c -- Earendil, July 26, 1995 */
/* You might wish to change this if either my castle or the graveyard of 
 * Azatoth is removed.
 */

inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

string window_status = "closed";
int lamp_is_lit;

void set_window(string how) {
  if( window_status == how )
    return;
  if( how == "open" ) {
    window_status = "open";
    tell_room(this_object(), "The window opens.\n");
    tell_room("/room/steeple", "The window opens.\n");
    add_exit("out", "/room/steeple", 0, "check_window");
    "/room/steeple"->add_exit("in", "/room/attic", 0, "check_window");
  } else {
    window_status = "closed";
    tell_room(this_object(), "The window closes.\n");
    tell_room("/room/steeple", "The window closes.\n");
    "/room/steeple"->remove_exit("in");
    remove_exit("out");
  }
  map_array(all_inventory(), "move_all_livings", this_object());
  "/room/steeple"->redo_actions();
}	

void move_all_livings(object o) {
  living(o) && move_object(o, this_object());
}

int query_window() { return window_status == "open"; }

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("inside");
   
    add_exit("west", "/room/elevator", 0, "check_elevator");

    set_short("Attic");
    set_long("");
    
    add_item(({ "window", "elevator door", "door", "elevator", 
	"lands", "hills", "forest", "path", "castle", "tower", }), 
     "foo");
  }
  if( window_status == "open" ) {
    tell_room(this_object(), "A gust of window sweeps through the attic.\n");
    set_window("closed");
  }
}

int door_open() {
  return ((int) "/room/elevator"->query_level() == 3) &&
	(int) "/room/elevator"->query_door_open();
}

string query_long(string str) {
  if( set_light(0) < 1 ) 
    return str ? "It is too dark to see.\n" : "A dark room.\n";

  if( !str )
    return LB("Attic\n" +
	"This is the attic of the village church.  The air is hot and "+
	"stuffy.  Also, it looks like the room hasn't been cleaned "+
	"up for a long time: dust covers the floor and walls.  There is "+
	"a window which is "+(window_status)+" set in the north wall, and "+
	(door_open() ? "an open" : "a closed") +
	" elevator door to the west." +
	(lamp_is_lit ? "  Beside the door is a glowing light." : "") +
	::query_long(0) );

  switch( str ) {
    case "elevator": case "door": case "elevator door":
      return LB("The elevator door is " + (door_open() ? "open" : "closed") +
	".  Beside the elevator door is a button and a"+
	(lamp_is_lit ? " glowing" : "n unlit") + " lamp.");
    case "button":
      return "You can push it to make the elevator come.\n";
    case "lamp":
      return "It tells whether the elevator is coming or not.\n";
    case "window":
      return LB("The " + window_status + " window looks out over the "+
	"lands to the north of the church.  A path winds north past "+
	"a small castle, ending in a land of rolling hills.");
    case "lands":
      return LB("Immediately north of the church, a path winds through "+
	"a dark and oppressive forest, past a small but cheery castle "+
	"to the left, and on to a land of rolling hills.");
    case "hills":
      return "The hills are misty and you cannot make out much at this "+
	"distance.\n";
    case "path":
      return LB("The path winds its way north past a spooky graveyard "+
	"and a small but peaceful castle to a land of misty hills.");
    case "forest":
      return LB("The forest is dark and oppressive, broken only by "+
	"the path cutting a swath through it, and to the west by "+	
	"a small castle.  Fortunately, the forest is small.");
    case "castle":
      return LB("The castle is small and apparently built more as a "+
	"home than as a fortress for defense.  It is but one story "+
	"high, with many windows, but it has a central tower that "+
	"must afford a nice view of the land.  Behind it is a "+
	"small garden.");
    case "tower":
      return "The tower, jutting up from that castle, must give a nice "+
	"view of the land.\n";
  }
  return ::query_long(0);
}

void init() {
  ::init();
  add_action("_enter_verb", "enter");
  add_action("_open_and_close_verb", "open");
  add_action("_open_and_close_verb", "close");

  add_action("_push_verb", "push");
  add_action("_push_verb", "press");
  add_action("_push_verb", "punch");
}

int _open_and_close_verb(string str) {
  string verb;

  verb = query_verb();

  if( str == "window" ) {
    if( (verb == "close") == (window_status == "closed") ) {
      write("But the window is already " + window_status + ".\n");
      return 1;
    }
    write("You " + verb + " the window.\n");
    say(this_player()->query_name() + " " + verb + "s the window.\n");
    tell_room("/room/steeple", "You see someone inside walk to the window.\n");
    set_window(verb == "close" ? "closed" : "open");
    return 1;
  }

  if( str != "elevator" && str != "door" && str != "elevator door" )
    return notify_fail(capitalize(verb) + " what?\n"), 0;

  if( (int)"/room/elevator"->query_level() != 3 ) {
    write("The elevator is not here!\n");
    return 1;
  }

  if( (int)"/room/elevator"->query_door_open() == (verb == "open") ) {
    write("But the elevator door is already " + 
	(verb == "open" ? "open" : "closed") + ".\n");
    return 1;
  }

  call_other("/room/elevator", verb + "_door", "door");
  return 1;
} 
  
int _enter_verb(string str) {
  if( str == "window" ) 
    return command("out", this_player());
  
  if( str == "door" || str == "elevator" || str == "elevator door" )
    return command("west", this_player());

  notify_fail("Enter what?\n");
}

int check_window() {
  if( !query_window() ) {
    write("The window is closed.\n");
    return 1;
  }
  return 0;
}

int check_elevator() {
  if( !door_open() ) {
    write("The elevator door is closed.\n");
    return 1;
  }
  return 0;		/* Allow egress */
}

int _silly_yes_verb() {
  write("That was a retorical question.\n");
  return 1;
}

int _push_verb(string str) {
  if( str != "button" && str != "elevator button" )
    return notify_fail(capitalize(query_verb()) + " what?\n"), 0;

  if( lamp_is_lit ) {
    write("The lamp is already lit, so it's coming already!\n"+
	"Do you think pushing it repeatedly makes it come any faster?\n");
    say(this_player()->query_name() + " impatiently pushes the "+
	"elevator button.\n");
    add_action("_silly_yes_verb", "yes");
    add_action("_silly_no_verb", "no");
    return 1;
  }
  if( door_open() || (int) "/room/elevator"->query_level() == 3) {
    write("The elevator is already here.  Why bother?\n");
    return 1;
  }
  write("You push the elevator button.\n");
  say(this_player()->query_name() + " pushes the elevator button.\n");
  lamp_is_lit = 1;
  tell_room(this_object(), "The lamp beside the elevator lights up.\n");
  "/room/elevator"->call_elevator(3);
  return 1;
}

void elevator_arrives() {
  tell_room(this_object(), "A small 'ding' comes from the elevator.\n"+
	"The glowing lamp on the elevator button goes out.\n");
  lamp_is_lit = 0;
}

