/*
 * File: /std/room.c
 * This is the new room.c to be used by people out there.
 * Typical setup in reset:
 *
 *  void
 *  reset(int arg)
 *  {
 *      ::reset(arg);
 *      if (!arg) {
 *          set_long("...");
 *          set_short("...");
 *          add_item("foo", "It is a very green foo.\n");
 *          add_item(({ "bar", "bars" }), "Bar bar bar!\n");
 *          add_exit("north", "players/zellski/room/frog_pond");
 *          add_property("SKIP_OBVIOUS", 1);
 *      }
 *  }
 * 
 * Properties used by this room at present:
 *    SKIP_OBVIOUS    - do not print out obvious exits.
 *    NO_CLEANUP      - do not clean this room out when unused. (destruct)
 *    NO_COMPOSITE    - do not describe things compositely, eg
 *                      "43 red fnords are here"
 */


inherit "/std/physical";

private static mapping exits;
private static mapping get_msgs;
private static object *affecting;

#define Composite(arr) (string) "/lib/composite"->composite_line(arr)

/*
 * External access functions.
 */

varargs void
add_item(mixed item, string desc, string get_msg)
{
    if (get_msg) {
	if (stringp(item))
	    get_msgs[item] = get_msg;
	else {
	    int i;
	    for (i = 0; i < sizeof(item); i++)
		get_msgs[item[i]] = get_msg;
	}
    }
    ::add_item(item, desc);
}

nomask varargs void
add_exit(string dir, mixed dest, string fun, mixed fun_obj)
{
    if (fun)
	dest = ({ dest, fun });
    if (!exits[dir])
	filter_array(all_inventory(this_object()), "sync_exit", this_object(),
		     dir);
    if (fun_obj) {
	if (objectp(fun_obj))
	    exits[dir] = dest + ({ explode(file_name(fun_obj), "#")[0] });
	else if (stringp(fun_obj))
	    exits[dir] = dest + ({ fun_obj });
    } else
	exits[dir] = dest;
}

static void
sync_exit(object player, string exit)
{
    if (living(player))
	command("!Sync! " + exit, player);
}

nomask void
remove_exit(string dir)
{
    exits = m_delete(exits, dir);
}

mixed
query_exit(string dir)
{
    return exits[dir];
}

mapping
query_exits()
{
    return exits + ([ ]);
}

string
query_get_msg(string str)
{
    return fun_call(get_msgs[str]);
}

mapping
query_get_msgs()
{
    return get_msgs + ([]);
}

varargs nomask void
remove_affect_long(object obj)
{
    if (!obj)
	obj = previous_object();
    if (member_array(obj, affecting) != -1)
	affecting -= ({ obj });
}

varargs nomask
void affect_long(object obj)
{
    if (!obj)
	obj = previous_object();
    if (member_array(obj, affecting) == -1)
	affecting += ({obj});
}

/*
 * Internal functions.
 */

string
brief_exits()
{
    return "   [" + implode(m_indices(exits), " ") + "]";
}

string
describe_inventory()
{
    object *arr;
    object *live;
    string output;
    int i;
    arr = all_inventory() - ({ this_player() });
    if (query_property("NO_COMPOSITE")) {
	output = "";
	for (i = 0; i < sizeof(arr); i++) {
	    /* Checking to see if item has a short. Electra 930521 */
	    if (arr[i]->short())
		output += capitalize(arr[i]->short()) + ".\n";
	}
	return output;
    }
    live = filter_array(arr, "is_live", this_object());
    return Composite(live) + Composite(arr - live);
}

static int
is_live(object ob)
{
    return living(ob);
}

private string
verbose_exits()
{
    string line;
    string *tmp;
    if (!m_sizeof(exits)) {
	return "      No obvious exits.\n";
    }
    line = "      Obvious exit" + (m_sizeof(exits) > 1 ? "s" : "") + ": ";
    if (m_sizeof(exits) > 1) {
        tmp = m_indices(exits);
        line += implode(tmp[1 .. sizeof(tmp) - 1], ", ");
	line += " and " + m_indices(exits)[0];
    } else
	line += m_indices(exits)[0];
    return line + ".\n";
}

string
describe_room(int modifier)
{
    if (set_light(0) <= 0)
	return "It is dark.\n";
    switch (modifier) {
	case 0:
	    return query_long() + describe_inventory();
	case 1:
	    return query_short() + "\n";
	case 2:
	    return query_short() + (query_property("SKIP_OBVIOUS") ? "" :
		   brief_exits()) + "\n" +
		   describe_inventory();
    }
    return 0;
}

static int
add_exit_action(string exit)
{
    add_action("room_move_somewhere", exit);
    return 1;
}

nomask int
room_move_somewhere()
{
    mixed dest;
    dest = exits[query_verb()];
    if (dest) {
	if (pointerp(dest)) {
	    switch (sizeof(dest)) {
		case 2:
		    if (call_other(this_object(), dest[1]))
			return 1;
		    break;
		case 3:
		    if (call_other(dest[2], dest[1]))
			return 1;
		    break;
		}
		dest = dest[0];
	}
	this_player()->move_player(query_verb(), dest);
	return 1;
    }
    return 0;
}

/*
 * Functions accessed by the mudlib.
 */

void
create()
{
    ::create();
    if (!get_msgs)
	get_msgs = ([ ]);
    if (!exits)
	exits = ([ ]);
    if (!affecting)
	affecting = ({ });
}
 
void
reset(int arg)
{
    ::reset(arg);
    if (!arg) {
	if (!get_msgs)
	    get_msgs = ([ ]);
	if (!exits)
	    exits = ([ ]);
	if (!affecting)
	    affecting = ({ });
    }
}

string
query_short()
{
    return (set_light(0) > 0) ? ::query_short() : "A dark room";
}

varargs string
query_long(string str)
{
    int i;
    if (str)
	return ::query_long(str);
    str = ::query_long();
    affecting -= ({ 0 });
    for (i = 0; i < sizeof(affecting); i++)
	str += affecting[i]->affect_environment();
    if (!query_property("SKIP_OBVIOUS"))
	str += verbose_exits();
    return str;
}

int
search(string str)
{
    notify_fail("You find nothing special.\n");
    return 0;
}

void
init()
{
    add_action("search", "search");
    add_action("add_exit_action", "!Sync!");
    filter_array(m_indices(exits), "add_exit_action", this_object());
}

string
nice_exits()
{
    return brief_exits();
}

int
try_get(string item)
{
    if (get_msgs[item])
	write(str(fun_call(get_msgs[item], item)));
    else {
        if (query_item(item))
	    write("You can't take that.\n");
        else return 0;
    }
    return 1;
}

int
clean_up()
{
    if (query_property("NO_CLEANUP") ||
      file_name(this_object()) == "std/room")
	return 0;
    if (!sizeof(filter_array(deep_inventory(this_object()), "clean_up_check",
			     this_object())))
	destruct(this_object());
    return 1;
}

static int
clean_up_check(object obj)
{
    return query_ip_number(obj) || obj->query_property("NO_CLEANUP");
}

varargs string
query_long_desc(string str)
{
    return ::query_long(str);
}
