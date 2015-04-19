/* Written by Mats */

#pragma strict_types
#pragma save_types

inherit "/std/rfc/basic_lock";
 
string direction;
mixed other;
int iscopy;
string first,second;

/******* Setup functions ********/

void set_first(string dir,string room)
{
  first = expand_file_name(room);
  add_property("s_dir",dir);
}

void set_second(string dir,string room)
{
  second = expand_file_name(room);
  add_property("f_dir",dir);
}

/******* Functions called by mudlib ********/

string query_class() { return other?"door":"$N"; }
string query_dir() { return direction; }

varargs mixed copy(string fun, mixed a)
{
  object c;

  if (c = present(other || query_name(),load_object(first))) {
    return call_other(c,fun,a);
  }
}

void set_opened(int n)
{
  iscopy?copy("set_opened",n): ::set_opened(n);
}

void set_unlocked(int n)
{
  iscopy?copy("set_unlocked",n): ::set_unlocked(n);
}

int query_opened()
{
  return iscopy?copy("query_opened"): ::query_opened();
}

int query_unlocked()
{
  return iscopy?copy("query_unlocked"): ::query_unlocked();
}

void install_door()
{
  object e;

  if (!(e = environment(this_object()))) return;

  if (first == "/"+file_name(e)) {
    direction = query_property("f_dir");
    other = query_property("s_dir")+" door";
    first->add_exit(direction,second,0,"@/"+file_name(this_object())+
		    "->check_door()");
  }
  else if (second == "/"+file_name(e)) {
    iscopy = 1;
    direction = query_property("s_dir");
    other = query_property("f_dir")+" door";
    second->add_exit(direction,first,0,"@/"+file_name(this_object())+
		     "->check_door()");
  }
  else return;
    
  if (query_name() == "thing")
    set_name(direction + " door");
  else
    other = 0;

  remove_property("f_dir");
  remove_property("s_dir");
}

#define ROOM_DESC "There is a $S $D to the $X"
#define CHECK_MESS "The $N is closed.\n"

string query_in_long_short()
{
  if (!direction) install_door();

  return replace_vars(eval(query_property("room_desc"))
		      || ROOM_DESC, direction);
}

varargs string query_long(string s)
{
  if (!direction) install_door();

  return query_lock_long(s) || replace_vars(::query_long(s));
}

string check_door(string arg, mixed str)
{ 
  if (str=="query") return 0;
  if (!direction) install_door();
  if (query_opened()) return 0;

  return replace_vars(eval(query_property("check_mess") || CHECK_MESS));
}

int id(string s)
{
  if (!direction) install_door();

  return ::id(s) || s=="door" || s==(direction+" door") ||
    s==direction || id_lock(s);
}

void reset(int arg)
{
  ::reset(arg);
  if (arg) {
    first->compute_extra_long();
    second->compute_extra_long();
  }
}

/******* Command functions ********/

mixed open_it(mixed arg)
{
  if (arg = ::open_it(arg)) {
    first->compute_extra_long();
    second->compute_extra_long();
  }
  return arg;
}
