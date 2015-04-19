/*
inherit "/basic/clean_up";
*/
inherit "/obj/property";

mixed short_description;
mixed long_description;
mapping exits=([]);
mapping items=([]);
mapping clones=([]);
mapping commands=([]);

#define linewrap(x) (stringp(x) ? sprintf("%-=75s\n", x) : x)
#define process(x)  (pointerp(x) ? (mixed)call_other(x[0], x[1]) : x)

void make_clone(string a)
{
  int i;
  mixed b;
  b=clones[a];
  if(pointerp(b))
  {
    for(;i<sizeof(b);i++)
      if(!b[i])
	move_object(clones[a][i]=clone_object(a), this_object());
  } else if(b) {
    object *o;
    o=all_inventory();
    for(;i<sizeof(o);i++)
      if(file_name(o[i])[0..strlen(a)-1]==a)
	b--;
    while(b--)
      move_object(clone_object(a), this_object());
  } else {
    if(!find_object(a) || !environment(find_object(a)))
      move_object(a, this_object());
  }
}

void reset(int a) { map_mapping(clones, "make_clone", this_object()); }

void set_short(mixed m) { short_description=m; }
void set_long(mixed m)  { long_description=linewrap(m); }

void add_exit(mixed a, mixed b)
{
  if(pointerp(a))
    map_array(a, "add_exit", this_object(), b);
  else
    exits[a]=b;
}

void add_item(mixed a, mixed b)
{
  if(pointerp(a))
    map_array(a, "add_item", this_object(), b);
  else
    items[a]=linewrap(b);
}

void add_command(mixed a, mixed b)
{ 
  if(pointerp(a))
    map_array(a, "add_command", this_object(), b);
  else
    commands[a]=b;
}

void add_clone(mixed a, int i, string t)
{
  switch(t)
  {
    case "unique":
      while(i--)
	move_object(clone_object(a), this_object());
	break;
    case "master":
      clones[a]=0;
      if(find_object(a) && environment(find_object(a)))
	return;
      move_object(a, this_object());
      break;
    case "present":
      clones[a]=i;
      while(i--)
	move_object(clone_object(a), this_object());
      break;
    case "exist":
      set_property("no_clean_up");
      clones[a]=allocate(i);
      while(i--)
	move_object(clones[a][i]=clone_object(a), this_object());
      break;
    default:
      write("Error in add_clone().\n");
  }
}

string query_short()
{
  return sprintf("%s (%s).\n",
	process(short_description),
	map_mapping(exits, "contract_dir", "/players/moonchild/roomd"));
}

string short()
{
  if(this_player() && this_player()->test_dark())
    return "It is dark";
  return query_short();
}

varargs string query_long(string s)
{
  if(s)
    return process(items[s]);
  else
    return process(long_description)
	+ (m_sizeof(exits) ?
		linewrap(sprintf("Obvious exits are: %s.",
			implode_nicely(m_indices(exits)))) :
		"No obvious exits.\n");
}

varargs void long(string s)
{
  if(this_player() && this_player()->test_dark())
    write("It is dark.\n");
  else
    write(query_long(s));
}

varargs mixed query_exits(string s, int i)
{
  if(s)
  {
    if(s=exits[s])
      return i ? s : process(s);
  }
  else
    return exits;
}

int id(string s) { return !!query_long(s); }

void init() { add_action("cmd_hook", "", 1); }

int cmd_hook(string s)
{
  mixed q;
  if(q=commands[query_verb()])
  {
    if(pointerp(q))
      q=(mixed)call_other(q[0], q[1], s);
    if(stringp(q))
      write(q);
    return 1;
  }
  if((q=exits[query_verb()]))
  {
    if(pointerp(q))
      q=(mixed)call_other(q[0], q[1], s);
    if(stringp(q) || objectp(q))
      this_player()->move_player(query_verb(), q);
    return 1;
  }
}
