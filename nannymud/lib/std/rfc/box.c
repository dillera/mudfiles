/* Written by Mats */

#pragma strict_types
#pragma save_types

inherit "/std/rfc/basic_lock";
inherit "/std/simple_container";

/******* Functions called by mudlib ********/

varargs string query_long(string s)
{
  return query_lock_long(s) || replace_vars(::query_long(s));
}

int id(string s)
{
  return ::id(s) || id_lock(s);
}

int can_put_and_get(mixed s)
{
  if (stringp(s) && id_lock(s)) return 0;
  return query_opened();
}

string query_class() { return "$N"; }
