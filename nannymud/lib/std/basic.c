/* Written by Profezzorn */
#pragma strict_types
#pragma save_types

#define MSGD "/std/msg"
#define ERROR(X) tell_object(find_player(creator(this_object())||"")||this_player()||this_object(),"ERROR "+(X))

varargs mixed query_property(string prop);

/* Most query functions call this function on their values before
 * returning them.
 * If the value is a string on the form
 *  "@function(arg)"
 * or
 *  "@object->function(arg)"
 * the named function will be called with that the string 'arg' as 
 * first argument, the second argument to eval() as second argument
 * and this_object() as third argument.
 */

static varargs mixed eval(mixed r,mixed arg)
{
  string tmp;
  mixed ob;

  if(stringp(r) && sscanf(r,"@%s",r) && r[-1]==')')
  {
    if(sscanf(r,"%s(%s",r,tmp))
      tmp=tmp[0..strlen(tmp)-2];

    if(!sscanf(r,"%s->%s",ob,r))
      ob=query_property("__remote") || this_object();
    else
      ob=expand_file_name(ob);

    return (mixed)call_other(ob,r,tmp,arg,this_object());
  }

  return r;
}

/*** short() handling ***/
private string short_desc;

void set_short(string s) { short_desc=s; }
string short() { return eval(short_desc); }


/*** long() and item handling ***/
/* Does not feature 'aliases', because rooms doesn't need it */
private string long_desc="You see nothing special.\n";
private mapping items;

#define WRAP(X) ( (X && !sscanf((X),"%*s\n") && (X)[0]!='@') ? sprintf("%-=77s\n",(X)) : (X) )

void set_long(string l)
{
  long_desc = WRAP(l);
}

int id(string s)
{
  return items && !!items[s];
}

varargs string query_long(string s)
{
  if(items)
  {
    if(items[s])
      return eval(items[s],s);
  }
  return eval(long_desc);
}

mixed query_item_map()
{
  return items || ([]);
}

varargs void long(string s)
{
  if(query_property("simplemore"))
  {
    this_player()->simplemore(query_long());
    this_player()->flush();
  }else{
    write(query_long(s));
  }
}

varargs void add_item(mixed item,string desc)
{
  int e;
  if(mappingp(item))
  {
    mixed *a,*b;
    a=m_indices(item);
    b=m_values(item);
    items=([]);
    for(e=0;e<sizeof(a);e++)
    {
      if(stringp(a[e]))
	items[a[e]]=b[e];
      else
	add_item(a[e],b[e]);
    }
  }
  else
  {
    if(!items) items=([]);
    if(!pointerp(item)) item=({item});

    desc=WRAP(desc);
    for(e=0;e<sizeof(item);e++)
      items[item[e]]=desc;
  }
}

void remove_item(mixed item)
{
  int e;
  if(!items) return;
  if(!pointerp(item)) item=({item});
  for(e=0;e<sizeof(item);e++)
    _m_delete(items,item[e]);
  if(!m_sizeof(items)) items=0;
}

/*** Command handling ***/
private mapping commands;

mapping query_command_map() { return commands || ([]); }

/* Add a command,
 * The first argument is the _full_ command, a verb + ' %s' or
 * an array of the above.
 * The action will be sent to eval(), and if eval() returns a
 * string, this will be written and 1 will be returned. If
 * eval returns an nothing will be written and this int will be returned.
 */
void add_command(mixed verb,mixed action)
{
  int e;
  if(!commands) commands=([]);
  if(!pointerp(verb)) verb=({verb});
  for(e=0;e<sizeof(verb);e++)
    commands[verb[e]]=action;
}

/*
 * Remove a command added with add_command
 */
void remove_command(mixed verb)
{
  int e;
  if(!commands) return;
  if(!pointerp(verb)) verb=({verb});
  for(e=0;e<sizeof(verb);e++) _m_delete(commands,verb[e]);
  if(!m_sizeof(commands)) commands=0;
}

/*
 * Magic function for adding commands,
 * verb is a verb or an array of verbs,
 * item is an item previously added with add_item(),
 * (Only one of the names of the item should be given)
 * action is an action that works the same way as the actions
 * in the add_command.
 * BUGS:
 * Note that this function can give strange results if you
 * have several items with the same description, as it
 * uses the description to identify which item it is.
 */
void add_item_cmd(mixed verb, mixed item, mixed action)
{
  mixed tmp;
  int e;
  string desc;

  if(!commands) commands=([]);
  if(!pointerp(verb)) verb=({verb});
  if(!(desc=query_item_map()[item]) && !id(item) )
    ERROR("add_item_cmd on nonexistant item.\n");

  for(e=0;e<sizeof(verb);e++)
  {
    verb[e]+=" %s";
    tmp=commands[verb[e]];
    if(!mappingp(tmp))
    {
      if(tmp)
	ERROR("Verb conflict between add_command & add_item_cmd!!!\n");
      commands[verb[e]]=tmp=([]);
    }
    tmp[desc]=action;
  }
}

/*
 * remove a command previously added with add_item_cmd
 */
void remove_item_cmd(mixed verb, string item)
{
  mixed tmp;
  int e;

  if(!commands) return;
  if(!pointerp(verb)) verb=({verb});
  item=query_item_map()[item];
  if(!item) return;

  for(e=0;e<sizeof(verb);e++)
  {
    verb[e]+=" %s";
    tmp=commands[verb[e]];
    if(mappingp(tmp))
    {
      _m_delete(tmp,item);
      if(!m_sizeof(tmp))
      {
	remove_command(verb[e]);
	if(!commands) break;
      }
    }
  }
}


/* A minimized version of /obj/property.
 * /Profezzorn
 */

#define PROPD "/obj/daemon/propd"

/* The properties, not accessible from outside this object */
private mapping properties;

/* Add a property, or set the properties alltogether
 * usage:
 * add_property(string property, mixed value);
 * or
 * add_property(mapping properties); 
 * the second usage clears any previous properties
 */
varargs void add_property(mixed prop,mixed value)
{
  int i,size;    

  if(!properties) properties=([]);
  if (stringp(prop))
  {
    if(!value) value=1;

    properties[prop]=value;
    map_array((string *)PROPD->give_me_links(prop),"add_property",this_object(),value);

  }
  else if (mappingp(prop))
  {
    properties = ([ ]);
    value=m_values(prop);
    prop=m_indices(prop);

    for(i=0,size = sizeof(prop);i<size;i++) 
      add_property(prop[i],value[i]);
  }
}

/* Check for a property
 * query_property(string property) retrieves a specific property
 * query_property() returns an array of all possible properties
 */
varargs mixed query_property(string prop)
{
  if (!prop)
  {
    if(!properties) return ({});

#if 0
    /* Only return properties with values != 0*/
    return filter_array(m_indices(properties),"query_property",this_object());
#else
    return m_indices(properties);
#endif
  }

  return properties && eval(properties[prop],prop);
}

/* remove one or more properties
 * give a string or array of strings
 */
void remove_property(mixed prop)
{
  if(!properties) return;

  if (pointerp(prop)) 
  {
    map_array(prop,"remove_property",this_object());
  }
  else
  {
    _m_delete(properties,prop);
    if(!m_sizeof(properties))
      properties=0;
    else
      remove_property((string *)PROPD->give_me_links(prop));
  }
}

void reset(int arg)
{
  if(properties)
    remove_property(regexp(m_indices(properties),"^temp_"));
}

/*** Command handling ***/
void init()
{
  add_action("do_cmd","",1);
}

/* All commands go through here */
int do_cmd(string what)
{
  string total;
  mixed action,tmp;

  if(commands)
  {
    total=query_verb();
    if(what) total+=" "+what;
    action=commands[total];
    if(!action)
      action=commands[query_verb()+" %s"];

    if(action)
    {
      if(mappingp(action))
      {
	if(!what)
	{
	  notify_fail(capitalize(query_verb())+" what?\n");
	  return 0;
	}
	sscanf(what,"the %s",what);
	action=action[tmp=query_item_map()[what]];
	if(!(action && (id(what) || tmp)))
	{
	  if(what &&
	     (id(what) ||
	      present(what,this_object()) ||
	      present(what,this_player())))
	  {
	    notify_fail("You cannot "+query_verb()+" that.\n");
	  }
	  else
	  {
	    notify_fail(capitalize(query_verb())+" what?\n");
	  }
	  return 0;
	}
      }

      action=eval(action,what);

      if(stringp(action))
	MSGD->msg(action);

      if(action)
	return 1;
    }
  }

  return 0;
}
