/*  This object is used to build rooms.
 *  There are two ways to use it: either inherit it, with "inherit room/room;",
 *  and set up all the global variables below in reset() in the object
 *  that inherits this file,
 *  or "#include <room.h>" and use the macros that are #defined there.
 *  See /examples/room1 through 12 for examples of how it can be used!
 *
 *  Additions to the /room/room from the original 2.4.5 mudlib
 *  (but this room still works with your old rooms):
 *	items can have aliases
 *	    - write an array of strings instead of just a string
 *	item_commands - now you can do things with the items
 *	extra commands - use the array extra_commands
 *	aliases also for directions (in dest_dir)
 *	    and item_commands and extra_commands
 *	exits can be blocked - use the array blocked_exits
 *	(This one had to be un-implemented: query_long
 *	    - works as "long", but returns a string instead of writing it)
 *	leave_messages - non-standard leave messages
 *	more efficient code - member_array, simplified convert_number, etc.
 *
 *  Latest change of this file: Oct 25, 1992, by Padrone
 *
 *  Obvious exits in shortdesc, Slater Jul 9, 1994
 */

/*---------------------------------------------------------------------------*/

/*  An array with destinations and directions:
 *      ({ "room/church", "north" ... })
 *  The direction can also be an array of synonyms:
 *      ({ "room/church", ({ "north", "leave", "out", "exit" }) ... })
 */
mixed *dest_dir;

/*  Functions to call to see if an exit is blocked.
 *  If this variable is 0, or the corresponding element in it is 0
 *  or nonexistent, or the function returns 0, then you are allowed to leave.
 *  Example: ({ 0, "check_south", 0, 0 ... })
 *  The array doesn't need to be (half) as long as the "dest_dir" array.
 */
string *blocked_exits;

/*  An optional array with directions to print ("X leaves ...").
 *  If this variable is 0, or the corresponding element in it is 0
 *  or nonexistent, the first direction synonym is printed ("X leaves north.").
 *  Example: ({ "into the church", 0, "through a magic door" })
 *  The array doesn't need to be (half) as long as the "dest_dir" array.
 */
string *leave_messages;

/* Short description of the room (withot a trailing newline) */
string short_desc;

/* Long description of the room */
string long_desc;

int can_put_and_get() { return 1; }

/*  An optional array with special items in the room:
 *      ({ "table", "A nice table", "window", "A window" )}
 *  The item name can also be an array of synonyms:
 *      ({ "table", "A nice table", ({ "window", "opening" }), "A window" })
 *  A period and a newline (".\n") will be appended to the description.
 */
mixed *items;

/*  An optional array with things you can do with the items.
 *  The array consists of pairs of a function name and a command:
 *      ({ 0, 0, "open_window", "open" })
 *  The command can also be an array of synonyms:
 *      ({ 0, 0, "open_window", ({ "open", "un-close" }) })
 *  The array doesn't need to be as long as the "items" array.
 */
string *item_commands;

/*  An optional array with commands you can give, other than exits
 *  and commands with the items in the array "items":
 *      ({ "cmd_jump", "jump" ... })
 *  The command can also be an array of synonyms:
 *      ({ "cmd_jump", ({ "jump", "suicide" }) ... })
 */
string *extra_commands;

/*  Facts about this room. Examples: "no_fight", "no_steal"
 *  It is either a string, as "no_fight", or an array,
 *  as ({ "no_fight", "no_steal" }):
 */
mixed property;

/* Set this to true if castles aren't allowed to be dropped here */
status no_castle_flag;

/* No teleports are allowed between different realms, or from/to realm "NT" */
string the_realm;

/* Why re-compute this every single time someone looks at the room? 
   Obvious exits dirstrings for short and long.
   */
string short_dir_string, dir_string;

/* Is this a place indoors or outdoors? 1 = indoors, 2 = outdoors, 0 undef. */
int inorout;

/* Set to something like ({ ({ "west" }), ({"east","grey" , 1 ,0 }) })
 * to make a door to the west, and one to the east, the west one requiring 
 * no key, and the east one a grey one. The one to the east will be closed, 
 * but not locked. Remember to do key->set_code(code or "");
 */

mixed *doors;
mapping n_doors=([]);

set_doors(x) 
{ 
  doors = x; 
}

varargs add_door(dir,type,code,is_closed,is_locked,can_be_locked, long, long2)
{
  string obj2, dir1, dir2;
  int n,i,j;
  string *dir_aliases;

  if(!dest_dir)
  {
    object o;
    if(o=find_player(creator(this_object())||"f"))
      tell_object(o, "You must add the exits before the doors.\n");
    return 0;

  }
  if(n_doors[dir])
    remove_door(dir);

  n = sizeof(dest_dir);
  for (i = 0, j = 0; i < n; i += 2, ++j) 
  {
    dir_aliases = dest_dir[i + 1];
    if((pointerp(dir_aliases) && member_array(dir,dir_aliases)
	!= -1)
       || dir_aliases == dir) 
    {
      obj2=dest_dir[i];
      break;
    }
  }

  dir2=
    (["north":"south", "south":"north", "east":"west", "west":"east", "southwest":"northeast", "northeast":"southwest",
      "southeast":"northwest", "northwest":"southeast", "down":"up","up":"down",])[dir] || "elsewhere";


  n_doors[dir]=make_door( file_name(this_object()), dir, obj2, dir2, 
			 type, is_closed, is_locked, code, !can_be_locked, long, long2);
}

remove_door(dir)
{
  if(n_doors[dir] && objectp(n_doors[dir][0]))
  {
    destruct(n_doors[dir][0]);
    destruct(n_doors[dir][1]);
    n_doors[dir]=0;
  }
}
/*---------------------------------------------------------------------------*/

int doors_added;

int id(string str) 
{
  int i, n;
  mixed item_aliases;		/* string or string* */

  if (!items)
    return 0;
  n = sizeof(items);
  for (i = 0; i < n; i += 2) {
    item_aliases = items[i];
    if ((pointerp(item_aliases) && member_array(str, item_aliases) != -1)
	|| item_aliases == str) {
      return 1;
    }
  }
#if 0
  if(stringp(str) &&
     this_player() &&
     interactive(this_player()) &&
     !present(str,this_object()) &&
     !present(str,this_player()))
  {
    log_file(creator(this_object())+".log",
	     this_player()->query_real_name()+"("+
	     (this_player()->query_level())+") tried to look at '"+str
	     +"' in "+file_name(this_object())+"\n");
  }
#endif
  return 0;
}				/* id */

#define wr(X) write(""+X+"\n");

make_door(obj1, dir1, obj2, dir2, keytype, closed, locked, keycode, can_not_lock, door_long, door_long2)
{
  object door, door2;
 
  door = clone_object("obj/door");
  door -> set_all(obj1 +"^"+ dir1 +"^"+ keytype +"^^"+
		  closed+"^"+locked+"^"+1);
  door->set_code(keycode);
  door->set_can_lock(!can_not_lock);
  door->set_door_long(door_long);

  door2 = clone_object("obj/door");
  door2 -> set_all(obj2 +"^"+ dir2 +"^"+ keytype +"^^"+
		   closed+"^"+locked+"^"+1);
  door2->set_code(keycode);  
  door2->set_can_lock(!can_not_lock);
  door2->set_door_long(door_long2||door_long);

  /* Init the other half of the door */
  door -> set_door(door2);
  door2 -> set_door(door);
  return ({ door, door2 });
}

make_doors()
{
  int q;
  for(q=0;q<sizeof(doors); q++)
  {
    string obj2, dir1, dir2;
    int n,i,j;
    string *dir_aliases;
    dir1=doors[q][0];
    doors[q]+=({ 0,0,0,0 });
    n = sizeof(dest_dir);
    for (i = 0, j = 0; i < n; i += 2, ++j) 
    {
      dir_aliases = dest_dir[i + 1];
      if((pointerp(dir_aliases) && member_array(dir1,dir_aliases)
	  != -1)
	 || dir_aliases == dir1) 
      {
	obj2=dest_dir[i];
	break;
      }
    }

    dir2=
      ([
	"north":"south",
	"south":"north",
	"east":"west",
	"west":"east",
	"southwest":"northeast",
	"northeast":"southwest",
	"southeast":"northwest",
	"northwest":"southeast",
	"down":"up",
	"up":"down",
	])[dir1] || "elsewhere";

    make_door( file_name(this_object()), dir1,obj2, dir2, 
	       doors[q][1], doors[q][2], doors[q][3]);
   }
  doors_added=1;
}

void init() 
{
  int i, n, j, nr_command_aliases;
  mixed command_aliases;	/* string or string* */
  if(doors && !doors_added)
    make_doors();

  if(pointerp(dest_dir)) {
    n = sizeof(dest_dir);
    for (i = 0; i < n; i += 2) {
      command_aliases = dest_dir[i + 1];
      if (command_aliases) {
	if (pointerp(command_aliases)) {
	  nr_command_aliases = sizeof(command_aliases);
	  for (j = 0; j < nr_command_aliases; ++j)
	    add_action("move", command_aliases[j]);
	}
	else
	  add_action("move", command_aliases);
      }
    }
  }

  if (item_commands) {
    n = sizeof(item_commands);
    for (i = 0; i < n; i += 2) {
      command_aliases = item_commands[i + 1];
      if (command_aliases) {
	if (pointerp(command_aliases)) {
	  nr_command_aliases = sizeof(command_aliases);
	  for (j = 0; j < nr_command_aliases; ++j)
	    add_action("do_item_command", command_aliases[j]);
	}
	else
	  add_action("do_item_command", command_aliases);
      }
    }
  }

  if (extra_commands) {
    n = sizeof(extra_commands);
    for (i = 0; i < n; i += 2) {
      command_aliases = extra_commands[i + 1];
      if (command_aliases) {
	if (pointerp(command_aliases)) {
	  nr_command_aliases = sizeof(command_aliases);
	  for (j = 0; j < nr_command_aliases; ++j)
	    add_action(extra_commands[i], command_aliases[j]);
	}
	else
	  add_action(extra_commands[i], command_aliases);
      }
    }
  }	
}				/* init */

int move(string str) 
{
  int i, j, n;
  mixed dir_aliases;		/* string or string* */
  string print_dir;

  if (str) {
    notify_fail("What do you mean, " + query_verb() + " " + str + "?\n");
    return 0;
  }
  n = sizeof(dest_dir);
  for (i = 0, j = 0; i < n; i += 2, ++j) {
    dir_aliases = dest_dir[i + 1];
    if (   (pointerp(dir_aliases) && member_array(query_verb(), dir_aliases) != -1)
	|| dir_aliases == query_verb()) {
      /* Ok, so we've found the direction the player just typed in "dest_dir" */
		
      if (leave_messages && j < sizeof(leave_messages) && leave_messages[j])
	print_dir = leave_messages[j];
      else if (pointerp(dir_aliases))
	print_dir = dir_aliases[0];
      else
	print_dir = dir_aliases;

      if (   !blocked_exits || j >= sizeof(blocked_exits) || !blocked_exits[j]
	  || !call_other(this_object(), blocked_exits[j], str)) {
	this_player()->move_player(print_dir + "#" + dest_dir[i]);
      }
      else {
	write("You couldn't go " + print_dir + ".\n");
	say(this_player()->query_name() + " tries to go " +
	    print_dir + " but fails.\n", this_player());
	/* Why the second argument?
	 * If someone inherits the room, and does enable_commands(),
	 * say() messages would otherwise go to this_player() too.
	 */
      }

      return 1;
    }
  }
  /* We should never come to this point in the code! */
} /* move */

int do_item_command(string str) 
{
  int i, j, k, n, nr_command_aliases;
  mixed item_aliases;		/* string or string* */
  mixed command_aliases;	/* string or string* */

  if (!str) {
    notify_fail("But what do you want to " + query_verb() + "?\n");
    return 0;
  }
  n = sizeof(item_commands);
  for (i = 0, j = 0; i < n; i += 2, ++j) {
    command_aliases = item_commands[i + 1];
    if (   (   pointerp(command_aliases)
	    && member_array(query_verb(), command_aliases) != -1)
	|| command_aliases == query_verb()) {
      /* Ok, so we've found the command the player just gave in "item_commands" */
      if (stringp(items[i]) ? items[i] == str : member_array(str, items[i]) != -1) {
	/* And as argument the player gave one of the items in "items" */
	/* say(this_player()->query_name() + " tries to " +
	   query_verb() + " " + str + ".\n", this_player()); */
	return (int)call_other(this_object(), item_commands[i], str);
      }
    }
  }
  if (id(str) || present(str, this_object()))
    notify_fail("You can't " + query_verb() + " that.\n");
  else if (str[strlen(str) - 1] == 's')
    notify_fail("There are no " + str + " to " + query_verb() + " here.\n");
  else
    notify_fail("There is no " + str + " to " + query_verb() + " here.\n");
  return 0;
}				/* do_item_command */

string convert_number(int n)
{
  switch(n)
  {
  case 0: return "no";
  case 1: return "one";
  case 2: return "two";
  case 3: return "three";
  case 4: return "four";
  case 5: return "five";
  case 6: return "six";
  case 7: return "seven";
  case 8: return "eight";
  case 9: return "nine";
  default: return "lots of";
  }
}
#if 0
string compute_dir_string(mixed *dest_dir) {
    int i, j, n, nr_dir_aliases;
    mixed dir_aliases;	/* string or string* */
    string result;

    if (!dest_dir || sizeof(dest_dir) == 0)
	result = "    No obvious exits.";
    else {
	i = 1;
	n = sizeof(dest_dir);
	if (n % 2 != 0)	/* Some derived rooms use a strange dest_dir! */
	    result = "    There might be some exits here.";
	else {
	    if (n == 2)
		result = "    The only obvious exit is";
	    else
		result = "    There are " + convert_number(n/2) +
		    " obvious exits:";
	    for (i = 0; i < n; i += 2) {
		dir_aliases = dest_dir[i + 1];
		if (pointerp(dir_aliases)) {
		    nr_dir_aliases = sizeof(dir_aliases);
		    if (nr_dir_aliases == 1)
			result += " " + dir_aliases;
		    else {
			result += " " + dir_aliases[0] + " (";
			for (j = 1; j < nr_dir_aliases; ++j) {
			    if (j > 1)
				result += ", ";
			    result += dir_aliases[j];
			}
			result += ")";
		    }
		}
		else
		    result += " " + dir_aliases;
		if (i == n - 4)
		    result += " and";
		else if (i < n - 2)
		    result += ",";
	    }
	    result += ".";
	}
    }
    return result;
} /* compute_dir_string */
#endif

string truncate_dir(string a)
{
  string c;
  sscanf(a, "%s (", a);
  switch(a)
  {
  case "north": return "n";
  case "south": return "s";
  case "east": return "e";
  case "west": return "w";
  case "southwest": return "sw";
  case "northeast": return "ne";
  case "southeast": return "se";
  case "northwest": return "nw";
  case "down": return "d";
  case "up": return "u";
  }
  return a;
} /* truncate_dir */

string compute_dir_string()
{
  mixed *exits;
  int a, i;
  string s;
  if(dir_string)
    return dir_string;
  if(!pointerp(dest_dir) || !sizeof(dest_dir))
  {
    short_dir_string = " <no exits>";
    dir_string = "    No obvious exits.";
    return dir_string;
  }
  exits = allocate((a = sizeof(dest_dir)) / 2);
  for(i = 1; i < a; i += 2)
    exits[i>>1] = dest_dir[i];
  dir_string = ((a /= 2) == 1 ? "    There is one obvious exit: " :
		("    There are "+  convert_number(a)+
		 " obvious exits: "));
  while(a--)  
    if(pointerp(exits[a]))
      exits[a] = exits[a][0]+" ("+ 
	implode_nicely(exits[a][1..sizeof(exits[a])-1]) +")";
  dir_string += implode_nicely(exits) +".";
  short_dir_string = " < "+ implode(map_array(exits, "truncate_dir",
					      this_object()), " ") +" >";
  if(sscanf(short_desc, "%s <%*s>", s) == 2)
    short_desc = s;
  /* 
     If we have manually added dest_dirs, remove them. 
     */
  return dir_string;
} /* compute_dir_string  */

query_dir_string() {return dir_string;}

#if 0
    /* Sigh! This will cause endless grief if a room re-defines long! */

/*  This is similar to "long", but returns the description instead
 *  of just writing it. It used to be simply
 *      return long_desc;
 *  but I think this is better (also look at "query_long_desc").
 *	-- Padrone, July 30, 1992
 *
 */
string query_long(string str) 
{
  int i, n;
  mixed item_aliases;		/* string or string* */

  if(set_light(0) > 0)
  {  
    compute_dir_string();
    if (!str) 
      return long_desc + dir_string +"\n";
    if (!items)
      return 0;
    n = sizeof(items);
    for (; i < n; i += 2) {
      item_aliases = items[i];
      if ((pointerp(item_aliases) && member_array(str, item_aliases) != -1)
	  || item_aliases == str) {
	say(this_player()->query_name() + " looks at " + str + ".\n",
	    this_player());
	return items[i+1] + ".\n";
      }
    }
    return 0;
  }
  return "It is dark.\n";
}				/* query_long */

string query_long_desc(string str) { return query_long(str); } 

#endif

/*  "long" could now be written as
 *	write(query_long(str));
 *  but isn't, for some sort of efficiency reason.
 *	-- Padrone, July 30, 1992
 *
 */

void set_long(string long) {      /* Anna 940909 */
  long_desc=long;
} /* set_long */

mixed long(string str) 
{
  int i, n;
  mixed item_aliases;		/* string or string* */
  
  if(set_light(0) > 0)
  {  
    compute_dir_string();
    if (!str) 
      return write(long_desc + dir_string +"\n"), 0;
    if (!items)
      return;
    n = sizeof(items);
    for (; i < n; i += 2) {
      item_aliases = items[i];
      if ((pointerp(item_aliases) && member_array(str, item_aliases) != -1)
	  || item_aliases == str) {
	say(this_player()->query_name() + " looks at " + str + ".\n",
	    this_player());
	return write(process_string(items[i+1]) + ".\n"), 0;
      }
    }
    return 0;
  }
  return "It is dark.\n";
}				/* long */

void set_short(string short) {       /* Anna 940909 */
  short_desc=short;
} /* set_short */

string short() 
{
  if (set_light(0))
  {
    compute_dir_string();
    return short_desc + (this_player() && present(this_player(), 
						  this_object()) &&
			 this_player() -> query_toggles("oshort") ? 
			 short_dir_string : "");
  }
  return "In a dark room";
}

mixed *query_dest_dir() { return dest_dir; }

mixed *query_items() { return items; }

/* Due to backwards compatibility, there is no query_realm(). */
void set_realm(string str) { the_realm = str; }
string realm(string str) 
{ 
  if (str)
    set_realm(str); 
  else 
    return the_realm; 
}

/*  Does this room has a special property ?
 *  The 'property' variable can be both a string and an array of strings.
 *  If no argument is given, return the 'property' variable.
 */
mixed query_property(string str) 
{
  int i, n;
  
  if (!str)
    return property;
  if (!property)
    return 0;
  if (stringp(property))
    return str == property;
  return member_array(str, property) != -1;
} /* query_property */

void add_property(string str)
{
  string tmp;
  if(!str) return;
  if(!property)
    property=({});
  if(stringp(property))
    {
      tmp=property;
      property=({tmp});
    }
  if(stringp(str))
    property+=({str});
  if(pointerp(str))
    property+=str;
  
}

void remove_property(string str)
{
  string tmp;
  if(!str) return;
  if(!property)
    {
      property=({});
      return;
    }
  if(stringp(property))
    {
      tmp=property;
      property=({tmp});
    }
  if(stringp(str))
    property-=({str});
  if(pointerp(str))
    property-=str;  
}

/*---------------------------------------------------------------------------*/

/*  Call this function to add an exit to the room.
 *  It will do the approprate changes to the variable dest_dir,
 *  and possibly also to blocked_exits and leave_messages,
 *  so be careful if you change those variables yourself
 *  after calling add_exit!
 *  Arguments:
 *	destination - a room, e. g. "/room/church" or "/players/ulrik/hall"
 *	direction - the command leading there, or an array of commands
 *	leave_message (optional) - alternative diection to print when leaving
 *	check_function (optional)
 *	    - if this function returns 1, you couldn't leave
 */
varargs void 
add_exit(string destination, mixed direction, string leave_message,
	 string check_function) 
{
  int old_nr_exits, i;
  
  if (dest_dir == 0)
    dest_dir = ({ });
  old_nr_exits = sizeof(dest_dir) / 2;
  dest_dir += ({ destination, direction });
  if (leave_message) {
    if (leave_messages == 0)
      leave_messages = ({ });
    for (i = sizeof(leave_messages); i < old_nr_exits; ++i)
      leave_messages += ({ 0 });
    leave_messages += ({ leave_message });
  }
  if (check_function) {
    if (blocked_exits == 0)
      blocked_exits = ({ });
    for (i = sizeof(blocked_exits); i < old_nr_exits; ++i)
      blocked_exits += ({ 0 });
    blocked_exits += ({ check_function });
  }
  dir_string = 0;		/* So it will be re-computed */
}				/* add_exit */

/*---------------------------------------------------------------------------*/
void remove_exit(mixed direction) {
  /* remove_exit() handles 2 different kinds of arguments:
   * 1/ A simple string naming the exit you want to remove.
   * 2/ An array of simple strings naming exits you want to remove.
   * The routine also removes relevant parts of blocked_exits and
   * leave_messages.
   * By Brom 950204.
   */
  int i, s, p;
  if (!direction || 
      !pointerp(dest_dir) ||
      !(s=sizeof(dest_dir))) return;

  if (pointerp(direction)) {
    map_array(direction, "remove_exit", this_object());
    return;
  }

  for (i=1; i<s; i+=2)
    if (pointerp(dest_dir[i]) &&
	(-1 != member_array(direction, dest_dir[i]))) break;
    else
      if (direction == dest_dir[i]) break;
  if (i>=s) return;
  i--;
  dest_dir = dest_dir[0..i-1] + dest_dir[i+2..s-1];
  i /= 2;
  /* If the variable is empty, it aint a pointer. Thus, pointerp() will
   * let you know if it is defined. You'd better check, or sizeof() bugs.
   */
  if (pointerp(blocked_exits)) {
    s = sizeof(blocked_exits);
    if (i<s) 
      blocked_exits = blocked_exits[0..i-1] + blocked_exits[i+1..s-1];
  }
  if (pointerp(leave_messages)) {
    s = sizeof(leave_messages);
    if (i<s)
      leave_messages = leave_messages[0..i-1] + leave_messages[i+1..s-1];
  }
  dir_string = 0; /* Get it re-computed. */
} /* remove_exit */

/*---------------------------------------------------------------------------*/

/*  Call this function to add an item to the room.
 *  It will do the approprate changes to the variable items,
 *  and possibly also to item_commands,
 *  so be careful if you change those variables yourself
 *  after calling add_item!
 *  Arguments:
 *	name - the name of the item, or an array of aliases
 *	description - the description (a ".\n" will be appended)
 *	command_function, command (optional)
 *	  - something to do with this item (a string or an array of aliases)
 */
varargs void add_item(mixed name, string description, 
	      string command_function, mixed command) 
{
  int old_nr_items, i;

  if (items == 0)
    items = ({ });
  old_nr_items = sizeof(items) / 2;
  items += ({ name, description });
  if (command_function) {
    if (item_commands == 0)
      item_commands = ({ });
    for (i = sizeof(item_commands); i < 2 * old_nr_items; ++i)
      item_commands += ({ 0 });
    item_commands += ({ command_function, command });
  }
}				/* add_item */

void set_items(mixed all) { items = all; }

/* remove_item() accepts 3 kinds of arguments:
 * 1/ A simple name string. If there is an item with that name, it will
 *    be removed.
 * 2/ An array of name strings. Each element in the array is processed as
 *    a simple name string.
 * 3/ An array of aliases. If this array is found among items, it is removed.
 *    Note that arrays are compared for EXACT equality, that is, if they are
 *    pointers to the same memory.
 * If there is an action and a function defined to go with the item, those are
 * removed too.
 */
mixed remove_item(mixed *name){ 
  /* Added by Taren 941025 */
  /* Touched by Brom 950204 */
    int i, s;
    string old;
    i=member_array(name,items);
    /* In case of an array of items to remove, do so. */
    if (i==-1 && pointerp(name)) {
      map_array(name, "remove_item", this_object());
      return name;
    }
    if(i>-1) {
	old=items[i+1];
	items=items[0..i-1]+items[i+2..sizeof(items)];
	/* Remove corresponding command. */
	if (pointerp(item_commands)) {
	  s = sizeof(item_commands);
	  if (i<s)
	    item_commands = 
	      item_commands[0..i-1] + item_commands[i+2..s-1];
	}
	return old;
    }
    return "";
} /*remove_item*/

void set_inorout(int where) {      /* Anna 940909 */
  if ((where!=1) && (where!=2)) return;
  inorout=where;
  return;
} /* set_inorout */

void set_indoors() {              /* Anna 940909 */
  inorout=1;
} /* set_in */

void set_outdoors() {             /* Anna 940909 */
  inorout=2;
} /* set_out */

void set_no_castle_flag(int arg) { no_castle_flag = arg; }

int query_inorout() { return inorout; }

/*---------------------------------------------------------------------------*/

/* If we can't find an object, we can use this as a last resort: */
status query_mentioned_in_long(string str) 
{
  if (!str || !long_desc)
    return 0;

  return sscanf(lower_case(long_desc), "%*s"+ str +"%*s") == 2;
}				/* query_mentioned_in_long */

/*---------------------------------------------------------------------------*/

/*  Destruct this object when clean_up is called and if ref_count
 *  is zero (no references to this object, i. e. no environment and no
 *  inventory). Otherwise return 1 so that clean_up will be called
 *  again later.
 */

int clean_up(int ref_count) 
{
  object *inv;
  int     i;

  if (ref_count>1 ||  environment() || first_inventory(this_object()))
    return 1;
  
  destruct(this_object());
  return 0;
} /* clean_up */

/*-----------------------------------------------------------------------*/

#if 1
    
/* Do fancy inventories - Profezzorn */

dark(string s,int brief)
{
  string desc;
  mixed *o;
  int e;

  if(set_light(0)<=0) return 0;
  if(!s) s="at room";
  switch(s)
  {
  default:
    return 0;
  case "at room": 
  case "around":
  case "at the room":
  case "move_player":
  }
  if(brief && stringp(desc=short()))
  {
    write(capitalize(desc)+".\n");
    o=all_inventory(this_object()) - ({this_player() });

    for(e=0;e<sizeof(o);e++)
    {
      if(stringp(desc=(string)o[e]->query_in_long_short()))
      {
      }
      else if(sscanf(file_name(o[e]),"players/%*s/%s",desc) && desc=="castle")
      {
      }
      else
      {
	continue;
      }
      o[e]=0;
    }
  } else {
    desc=function_exists("dark",this_object());
    if(function_exists("long",this_object())!=desc ||
       (function_exists("query_long_desc",this_object()) &&
	function_exists("query_long_desc",this_object())!=desc))
    {
      return 0;			/* give up */
    }else{
      compute_dir_string();
      write(long_desc);
      o=all_inventory(this_object()) - ({ this_player() });

      for(e=0;e<sizeof(o);e++)
      {
	if(stringp(desc=(string)o[e]->query_in_long_short()))
	{
	  write(capitalize(desc)+".\n");
	}
	else if(sscanf(file_name(o[e]),"players/%*s/%s",desc) && desc=="castle")
	{
	  if(stringp(desc=(string)o[e]->short()))
	    write(capitalize(desc)+".\n");
	}
	else
	{
	  continue;
	}
	o[e]=0;
      }
      write(dir_string+"\n");
    }
  }
  o=(o->short())-({0});

  for(e=0;e<sizeof(o);e++)
    write(capitalize(o[e])+".\n");

  return 1;
}
#endif
