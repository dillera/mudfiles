/*
 *  A thing. It can be read, either with a fixed message or a file.
 *  Thomas Padron-McCarthy, padrone@lysator.liu.se, 1990
 *	Clone this object to get a generic thing, then use set_name,
 *	and (all the following are optional) set_aliases, add_alias,
 *	set_short, set_long, set_read, set_read_file, set_more,set_info,
 *	set_can_get, set_can_drop, set_weight and set_value.
 *  This file latest updated: November 28:th 1995 by Brom.
 */

#pragma strict_types

/* Added by Taren 950223 */
inherit "/obj/property";

string name, short_desc, long_desc, read_message, read_file,info;
string *aliases = ({});
status use_more, can_get, can_drop;
int weight, value;

/*---------------------------------------------------------------------------*/

void set_name(string n) { name = n; short_desc = "a " + n; }
void add_alias(string str) {
    if (aliases == 0)
	aliases = ({ });
    aliases += ({ lower_case(str) });
} /* add_alias */
void set_aliases(string  *all_aliases) { 
  if (pointerp(all_aliases))
    aliases = all_aliases;
  else
    aliases = ({});
}
void set_alias(string n) { add_alias(n); }	/* For campatibility */
void set_short(string sh) { short_desc = sh; }
void set_long(string long) { long_desc = long; }
void set_read(string str) { read_message = str; }
void set_read_file(string filename) { read_file = filename; }
void query_read() { return read_message || "FILE:"+read_file; }
void set_more(status f) { use_more = f; }
void set_can_get(status n) { can_get = n; }
void set_weight(int w) { weight = w; }
void set_value(int v) { value = v; }
void set_can_drop(status n) { can_drop = n; }
void set_info(string n) { info = n; }

string query_info() { return info; } /* Added by Ppannion 040217 compatibility reasons */

string query_name() {
    if (name != 0)
	return name;
    /* changed from strlen to sizeof by profezzorn */
    else if (aliases != 0 && sizeof(aliases) > 0)
	return aliases[0];
    else
	return 0;
}

int query_weight() { return weight; }
int query_value() { return value; }

mapping items=([]);

status id(string str) {
    string lstr;
    if(items[str])
      return 1;
    if (!str)
	return 0;
    lstr = lower_case(str);
    if (name != 0 && lstr == lower_case(name))
	return 1;
    /* Check that aliases is an array added by Brom 951128, */
    return pointerp(aliases) && member_array(lstr, aliases) != -1;
} /* id */

int cmd_read(string str) {
    if (!str || !id(str))
	return 0;
    else if (!read_file && !read_message)
	return 0;
    else {
	say(this_player()->query_name() + " reads the " + name + ".\n");
        if (!read_file)
	    write(read_message);
	else if (use_more)
	    this_player()->more(read_file);
	else
	    cat(read_file);
	return 1;
    }
} /* cmd_read */

string short() {
    return short_desc;
}

void add_item(string s, string e) { items[s]=e; }
void remove_item(string s) { items[s]=0; }

void long(string s) 
{
  if(s && items[s])
  {
    write(items[s]);
    return;
  }
  
  if (long_desc)
    write(long_desc);
  else if (read_file || read_message)
    cmd_read(name);
  else
    write("You see nothing special.\n");
} /* long */

void reset(int arg) {
    if (arg)
	return;
    set_name("thing");	/* This also sets the short description. */
    aliases = 0;
    long_desc = 0;	/* Use cmd_read() if no long_desc set! */
    read_message = 0;
    read_file = 0;
    use_more = 0;
    can_get = 1;
    can_drop = 0;
    weight = 1;
    value = 1;
} /* reset */

void init() {
    add_action("cmd_read", "read");
}

int drop() {
    return can_drop;     /* Added by Cel 94, 1=prevent dropping */
}

int get() {
    return can_get;
}


int clean_up(int arg)
{
  if(environment()) return 0;
  while(first_inventory()) destruct(first_inventory());
  if(_isclone(this_object()) ||
     (arg<2 && this_object()==_next_clone(this_object())))
    destruct(this_object());
  return 1;
}


