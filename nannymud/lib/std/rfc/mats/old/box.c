inherit "/std/simple_container";

string lock_code;
int locked = -1;
int opened = -1;
string long_d;

set_lock(mixed l)
{
  if (stringp(l))
    lock_code = l;
  else
    lock_code = sprintf("%d",l);
  locked = 1;
  set_up(0);
}

set_open() { set_up(1); }

set_closed() { set_up(0); }

set_up(int o)
{
  opened = !!o;
  add_command(({"open %s","close %s"}),"@open_box()");
  /* add_extra_long("@box_dec()"); */
  ::set_long("@box_desc()");
}

status can_put_and_get() { return !!opened; }

set_long(s) /* this shouldn't need to be here */
{
  long_d = s;
}
  
box_desc()
{
  string d;
  if (!long_d)
    d = "";
  else
    d = long_d;
  if (opened== -1) return d;
  d += "It is "+(opened?"open":"closed");
  if (lock_code)
    d += " and "+(locked?"locked":"unlocked");
  return d+".\n";
}

open_box(string arg,string str) {
  string verb;

  if (!id(str)) return 0;
  verb = query_verb();

  if (opened == -1) {
    notify_fail("You can't do that with "+str+".\n");
    return 0;
  }
  if (locked == 1) {
    write("You can't do that. The "+str+" is locked.\n");
    return 1;
  }
  if (opened == (verb == "open")) {
    write("The "+str+" is already "+(verb == "open"?"open":"closed")+".\n");
  } else {
    opened = (verb == "open");
    "/std/msg"->msg("\bPRON "+verb+"\b$ the "+str+".\n");
  }

  return 1;
}

lockunlock(string str, string code, int what)
{
  if (locked == -1)
    return 0;
  if (locked == what)
    return 2;
  if (lock_code == code) {
    locked = what;
    return 1;
  }
  return 3;
}
