/*
   Example:
   inherit "/players/earendil/std/door_room";

   void create() {
     ::create();
     set_light(1);

     add_exit("south", "kitchen");
     add_door("north", "backyard", ({"oak door"}), "house_key");
     
     set_short("Back Hall");
     set_long("You are in the back hallway of the house.  There is a "+
	"$DOOR door leading north.");
   }

   You'll need a ::reset(arg) if you want the door to lock back at the
	next reset.  When it does so, it prints a lame message about
	wind blowing through the room, shuttind the door. :)

   If you don't want a door to be lockable, leave off the "house_key"
	field.  That field is of course what you set a key's code to.

   Bugs: Only supports one door per room.
 */
inherit "/std/basic_room";

private int door_open = 0, door_unlocked = 1;
private mixed partner_room;
private string door_code, *door_aliases=({});

varargs void add_door(string dir, mixed where, 
	     	string *aliases, string lock_code)
{
  door_code = lock_code;
  if( pointerp(aliases) )
    door_aliases = ({ "door" }) + door_aliases;
  else
    door_aliases = ({ "door" });
  partner_room = objectp(where) ? "/"+file_name(where) : expand_file_name(where);
  add_exit(dir, where, dir+", through the door", "check_door");

  if( lock_code )
    door_unlocked = 0;
}

void init() {
  ::init();
  if( partner_room ) {
    if( door_code ) {
      add_action("lock_or_unlock", "lock");
      add_action("lock_or_unlock", "unlock");
    }
    add_action("open_or_close", "open");
    add_action("open_or_close", "close");
  }
}

#define CAPVERB capitalize(query_verb())
#define FAIL(X) return notify_fail(X+"\n"),0;
#define END(X) return write(sprintf("%-78=s\n", X)),1

int lock_or_unlock(string str) {
  string with_what;
  object key;

  if( !str )
    FAIL(CAPVERB+" what?");

  if( member_array(str, door_aliases) > -1 ) {
    if( !(key = present("key", this_player())) )
      END("You need a key.");
    write("(with the "+(key->query_name() || "key")+")\n");
    with_what = "key";
  } else if( !sscanf(str, "%s with %s", str, with_what) )
    FAIL(CAPVERB+" what?");
  if( !(key = present(with_what, this_player())) ) {
    if( present(with_what) )
      END("You must pick it up first.");
    END("But you are carrying no "+with_what+"!");
  }
  if( !key->id("key") )
    END("You need to use a key, not a "+with_what+"!");
  if( (string)key->query_code() != door_code )
    END("The key does not fit the lock.\n");
  if( door_unlocked == (query_verb() == "unlock") )
    END("The door is already "+query_verb()+"ed.");
  write("You "+query_verb()+" the door.\n");
  say(this_player()->query_name() + " " + query_verb() + "s the door.\n");
  call_other(this_object(), "set_"+query_verb()+"ed", "silent");
  call_other(partner_room, "set_"+query_verb()+"ed", "loud");
  return 1;
}

/* Just a handy way to get the 'd' appended to 'close' */
string describe_condition(string s) {
  return s == "open" ? "open" : "closed";
}

int open_or_close(string str) {
  if( !str || member_array(str, door_aliases) == -1 )
    FAIL(CAPVERB+" what?");
  if( door_open == (query_verb() == "open") )
    END("But the door is already "+describe_condition(query_verb())+".");
  if( !door_unlocked )
    END("The door is locked.");
  write("You "+query_verb()+" the door.\n");
  say(this_player()->query_name() + " " + query_verb() + "s the door.\n");
  call_other(this_object(), "set_"+describe_condition(query_verb()),
	"silent");
  call_other(partner_room, "set_"+describe_condition(query_verb()),
	"loud");
  return 1;
}
	
void set_open(string how) {
  if( !door_open && how != "silent" )
    tell_room(this_object(), "The door opens.\n");
  door_open = 1;
}

void set_closed(string how) {
  if( door_open && how != "silent" )
    tell_room(this_object(), "The door closes.\n");
  door_open = 0;
}

void set_unlocked(string how) {
  if( !door_unlocked && how != "silent" )
    tell_room(this_object(), "You hear the door's lock click open.\n");
  door_unlocked = 1;
}

void set_locked(string how) {
  if( door_unlocked && how != "silent" )
    tell_room(this_object(), "You hear the door's lock click into place.\n");
  door_unlocked = 0;
}

string query_long(string str) {
  str = ::query_long(str);
  if( !sscanf(str, "%*s$DOOR") ) return str;

  if( sscanf(str, "%*sa $DOOR") )
    return replace(str, "a $DOOR", door_open ? "an open" : "a closed");
  if( sscanf(str, "%*sA $DOOR") )
    return replace(str, "A $DOOR", door_open ? "An open" : "A closed");
  return replace(str, "$DOOR", door_open ? "open" : "closed");
}

void reset(int arg) {
   if( !partner_room || !arg )
	return;
  if( door_open ) {
    tell_room(this_object(), "A gust of wind blows past you.\n");
    set_closed("loud");
    partner_room->set_closed("loud");
  }
  if( door_code && door_unlocked ) {
    set_locked("loud");
    partner_room->set_locked("loud");
  }
}

int check_door(string direction) {
  if( !door_open ) {
    write("The door is closed.\n");
    say(this_player()->query_name()+" tried to go "+direction+" but the "+
	"door is closed.\n");
    return 1;
  }
  return 0;
}

