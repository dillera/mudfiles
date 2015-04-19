/* Written by Mats */

#pragma strict_types
#pragma save_types

inherit "/std/stationary";

private int opened;
private int unlocked;
private string code;

void reset(int arg)
{
  opened = unlocked = 0;
  if (arg) return;
  set_long("The $N is $S.\n");
} 

/******* Setup functions ********/

varargs void set_lock(string c,string type,string desc)
{
  code = c;
  if (type) set_long("The $N is $S and has a "+type+" lock.\n");
  if (desc) add_property("lock_desc",desc);
}

void set_opened(int n)   { opened   = n; }
void set_unlocked(int n) { unlocked = n; }

/******* Functions called by mudlib ********/

int query_opened()   { return opened; }
int query_unlocked() { return unlocked; }
string query_code()  { return code; }

string query_class() { return "$N"; }

varargs string replace_vars(mixed mess, string extra)
{
  if (!stringp(mess)) return mess;

  if (extra) mess = replace(mess,"$X",extra);
  mess = replace(mess,"$D",query_class());
  mess = replace(mess,"$N",query_name());
  mess = replace(mess,"$S",query_opened()? "open" : "closed");
  return replace(mess,"$V",query_verb());
}

#define LOCK_DESC "You see nothing special about the lock.\n"
#define OPEN_MESS "\bPRON $V\b$ the $N.\n"
#define UNLOCK_MESS "\bPRON $V\b$ the $N with the $X.\n"

varargs string query_lock_long(string s)
{
  if (s && sscanf(s,"%*slock"))
    return eval(query_property("lock_desc")) || LOCK_DESC;
}

int id_lock(string s)
{
  if (s && code) {
    if (s=="lock") return 1;
    if (sscanf(s,"%s lock",s)) return id(s);
  }
}

/******* Command functions ********/

mixed open_it(string name)
{
  if (!id(name)) {
    notify_fail(capitalize(query_verb())+" what?\n");
    return 0;
  }

  if (query_opened() == (query_verb() == "open"))
     return replace_vars("The $N is already $S.\n");

  if (code && !query_unlocked())
    return replace_vars("You can't $V it. The $N is locked.\n");

  set_opened((query_verb() == "open"));

  return replace_vars(eval(query_property("open_mess") || OPEN_MESS));
}

mixed unlock_it(string name)
{
  string key;
  object k;
  mixed tmp;

  if (!name || !sscanf(name,"%s with %s",name,key)) {
    key = query_property("unlocker") || "key";
  }

  if (!id(name) || !code) {
    notify_fail(capitalize(query_verb())+" what?\n");
    return 0;
  }

  if (query_unlocked() == (query_verb() == "unlock"))
    return replace_vars("The $N is already $Ved.\n");

  if (!(k = present(key,this_player())))
    return "You don't have a "+key+".\n";

  key = k->query_name() || key;
  tmp = k->query_code() || k->query_property("code");
  if (intp(tmp) && tmp > query_property("pick_level"))
    tmp = code;

  if (tmp == code) {
    set_unlocked(query_verb() == "unlock");
    return replace_vars(eval(query_property("unlock_mess") || UNLOCK_MESS),key);
  }
  return replace_vars("The $X doesn't fit in the $N.\n",key);
}

int do_cmd(string what)
{
  mixed tmp;

  switch(query_verb()) {
    case "open":
    case "close":
      tmp = open_it(what);
      break;
    case "lock":
    case "unlock":
      tmp = unlock_it(what);
  }

  if (stringp(tmp))
    "/std/msg"->msg(tmp);

  if (tmp) return 1;
  return ::do_cmd(what);
}
