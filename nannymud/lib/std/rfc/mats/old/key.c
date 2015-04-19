inherit "/std/basic_thing";

string key_code;

set_code(string code)
{
  key_code=code;
  add_alias("key");
  add_command("unlock %s","@unlock(0)");
  add_command("lock %s","@unlock(1)");
}

query_code() { return key_code; }

unlock(mixed arg,string str)
{
  string door, key, verb;
  mixed e;
  
  if (!present(query_name(),this_player()))
    return 0;

  verb = query_verb();
  notify_fail(capitalize(verb)+" what?\n");
  if (!str) return 0;

  if (!sscanf(str,"%s with %s",door,key)) {
    key = query_name();
    door = str;
  } else
    notify_fail("You can't unlock with a "+key+".\n");

  if (!(e = present(door))) {
    if (!(e = present(door,environment(this_player())))) {
      mixed p;
      e = environment(this_player());
      if (!e->id(door)) {
	notify_fail("There is no "+door+" here.\n");
	return 0;
      }
      p = e->query_property(door);
      if (intp(p) && p < 0) {
	notify_fail(e->query_long(door));
	return 0;
      }
      if (p)
	e = p;
    }
  }

  if (!present(key)) {
    notify_fail("You don't have a "+key+".\n");
    return 0;
  }

  if (!id(key)) 
    return 0;

  key = query_name();
  arg = atoi(arg);

  switch(e->lockunlock(door,key_code,arg)) {
  case 0:
    write("The "+door+" have no lock.\n");
    return 1;
  case 1: 
    "/std/msg"->msg("\bPRON "+verb+"\b$ the "+door+" with "+key+".\n");
    return 1;
  case 2:
    write("The "+door+" is already "+verb+"ed.\n");
    return 1;
  case 3:
    notify_fail("The "+key+" doesn't fit in "+door+".\n");
    return 0;
  case 4:
    notify_fail("Which one?\n");
    return 0;
  }
}
