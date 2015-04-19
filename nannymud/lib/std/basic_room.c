/* Written by Profezzorn */
#pragma strict_types
#pragma save_types

inherit "/std/basic";

private mapping exits=([]);

private static string extra_short;
private static string extra_long;

/*** Setup functions ***/

/* Add an exit */
varargs void add_exit(string dir,
		      mixed where,
		      string leave_message,
		      string check_function)
{

  if(objectp(where))
    where="/"+file_name(where);
  else
    where=expand_file_name(where);

  if(check_function)
  {
    if(stringp(check_function) && check_function[0]!='@')
      check_function="@/"+file_name(this_object())+"->"+check_function+"("+dir+")";

    where=({where,leave_message,check_function});
  }
  else if(leave_message)
  {
    where=({where,leave_message});
  }

  exits[dir]=where;

  extra_short=extra_long=0;
}

/* Remove an exit */
void remove_exit(string dir)
{
  _m_delete(exits,dir);
  extra_short=extra_long=0;
}

mapping query_exits() { return exits; }

/*** Functions called by mudlib/driver ***/
int do_cmd(string arg)
{
  mixed where,message, tmp;
  if(where=exits[message=query_verb()])
  {
    if(pointerp(where))
    {
      switch(sizeof(where))
      {
      case 0:
	write("Error in added exit.\n");
	return 1;
      default:

      case 3:
	if(tmp=eval(where[2],arg))
	{
	  if(stringp(tmp))
	    "/std/msg"->msg(tmp);
	  return 1;
	}

      case 2:
	message=eval(where[1],"move");
	if(!message) message=query_verb();

      case 1:
	where=where[0];
      }
    }

    where=eval(where,"move");

    if(stringp(where))
      this_player()->move_player(message,where);

    if(where)
      return 1;
  }

  return ::do_cmd(arg);
}

string short()
{
  if(set_light(0)<=0)
    return "In a dark room";

  if(!extra_short)
  {
    mixed *a;
    int e;

    a=m_indices(exits);
    for(e=0;e<sizeof(a);e++)
    {
      switch(a[e])
      {
      case "northwest": a[e]="nw"; break;
      case "southwest": a[e]="sw"; break;
      case "northeast": a[e]="ne"; break;
      case "southeast": a[e]="se"; break;
	
      case "north":
      case "south":
      case "east":
      case "west":
	a[e]=a[e][0..0];
      }
    }
    extra_short=" <"+implode(a," ")+">";
  }
  return ::short() + extra_short;
}

varargs string compute_extra_long(int recompute)
{
  string tmp,in;
  if(!recompute)
  {
    extra_long=0;
    return 0;
  }

  in=implode(all_inventory()->query_in_long_short()+
	     all_inventory()->query_property("in_long_short"),".\n");

  switch(m_sizeof(exits))
  {
  case 0:
    tmp="  There are no obvious exits.\n";
    break;

  case 1:
    tmp="  There is one obvious exit: "+m_indices(exits)[0]+".\n";
    break;

  default:
    tmp=sprintf("  There are %-=60s\n",
		(string)"/obj/support"->number_to_string(m_sizeof(exits))+
		" obvious exits: "+
		implode_nicely(m_indices(exits))+".");
  }

  if(strlen(in)) tmp=in+".\n"+tmp;

  return tmp;
}

string query_long(string s)
{
  if(set_light(0) <= 0)
  {
    if(s)
      return "It is too dark to see.\n";
    else
      return "A dark room.\n";
  }
    

  if(s) return ::query_long(s);

  if(!extra_long)
    extra_long=compute_extra_long(1);
  return ::query_long() + extra_long;
}

int clean_up(int refs)
{
  if(_isclone(this_object())) return 0;
  if(query_property("no_clean_up") || refs>1 || first_inventory())
    return 1;

  destruct(this_object());
}

/* compatibility */

int query_inorout()
{
  if(query_property("indoors")) return 1;
  if(query_property("outdoors")) return 2;
  return 0;
}

string realm() { return query_property("realm"); }

/* Please check query_exits before query_dest_dir because
 * it is much faster
 */
string *query_dest_dir()
{
  mixed *a,*b,*c;
  int e,d;

  a=m_indices(exits);
  b=m_values(exits);
  c=allocate(sizeof(a)*2);

  for(e=0;e<sizeof(a);e++)
  {
    if(pointerp(b[e])) b[e]=b[e][0];
    c[d]=eval(b[e],"query");
    if(intp(c[d])) continue;
    d++;
    c[d++]=a[e];
  }

  return c[0..d-1];
}
