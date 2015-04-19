#pragma strict_types
#pragma save_types

inherit "/std/basic_thing";

#define MSGD "/std/rfc/msg"

private int strength=1;
private int heal;
private mixed eaten_obj;
private string eat_msg = "\bPRON eat\b$ a thing.\n";

/*** Setup functions ****/
void set_strength(int s) { strength=s; }
void set_heal(int s) { heal=s; }
void set_eat_msg(string m) { eat_msg=m; }
void set_eaten_obj(mixed e)
{
  if(stringp(e)) e=expand_file_name(e);
  eaten_obj=e;
}

/*** Query functions ***/
int query_strength() { return strength; }
int query_heal() { return heal; }
mixed query_eaten_obj() { return eaten_obj; }
string query_eat_msg() { return eat_msg; }
int query_food() { return 1; }

/*** Functions called by mudlib/driver ***/
void init()
{
  add_action("eat_cmd","eat");
  add_action("smell_cmd","smell");
}

/*** Commands ***/
int smell_cmd(string str)
{
  mixed r,smell;

  notify_fail("Smell what?\n");
  if(!str || !id(str)) return 0;

  r=query_property("__remote");
  if(r && r->remote_smell_callback(this_object()))
    return 1;
  
  s=query_property("_smell") || "You smell nothing special.\n";
  write(s);
  say(this_player()->query_name()+" sniffs "+short()+".\n");
  return 1;
}

int eat_cmd(string str)
{
  mixed r;
  notify_fail("Eat what?\n");
  if(!str || !id(str)) return 0;

  r=query_property("__remote");

  if(r && r->remote_prevent_eat(this_object()))
    return 1;

  if(this_player()->query_level() * 8 < strength)
  {
    write("You realize even before you try that you could never eat that much.\n");
    return 1;
  }

  if (!this_player()->eat_food(strength))
    return 1;

  this_player()->heal_self(heal);

  MSGD->msg(eat_msg);

  if(r && r->remote_eat_callback(this_object()))
    return 1;

  r=eval(eaten_obj);
  if(stringp(r)) r=clone_object(r);
  if(r) move_object(r, environment());
  destruct(this_object());
  return 1;
}
