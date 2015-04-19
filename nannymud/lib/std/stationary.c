/* Written by Profezzorn */
/* This is for making objects that can't be moved,
 * such as signs, trees, etc. 
 */

#pragma strict_types
#pragma save_types

inherit "/std/basic";

private string *aliases=({});
private string name="thing";

/*** Setup functions ***/
void set_name(string s) { name=s; }
void add_alias(mixed s)
{
  if(!pointerp(s)) s=({s});
  s-=aliases;
  s-=({name});
  aliases+=s;
}
void remove_alias(string s) { aliases-=({s}); }

/*** functions called by mudlib/driver ***/
string query_name() { return name; }
string *query_aliases() { return aliases; }

int id(string s)
{
  switch(query_verb())
  {
  case "ex":
  case "exa":
  case "exam":
  case "exami":
  case "examin":
  case "examine":
  case "look":
  case "l":
    if(::id(s)) return 1;
  }
  return s==name || member_array(s,aliases)!=-1;
}


string query_info(string s)
{
  return query_property("info_string");
}

/*
 * Clean up....
 */
int clean_up(int arg)
{
  if(environment()) return 0;
  if(query_property("no_clean_up")) return 1;
  while(first_inventory()) destruct(first_inventory());
  if(_isclone(this_object()) ||
     (arg<2 && this_object()==_next_clone(this_object())))
    destruct(this_object());
  return 1;
}
