/* Written by Profezzorn */
#pragma strict_types
#pragma save_types

inherit "/std/basic_thing";

private static int time_stamp;
private static object wielded_by;
private int weapon_class;
private string type;

/*** Setup functions ***/

void set_class(int c) { weapon_class=c; }
void set_type(string t) { type=t; }
void set_hit_line_object(mixed ob) { add_property("__hit_line_object",ob); }

/*** Functions called by mudlib/driver ***/

object query_wielded() { return wielded_by; }
int weapon_class() { return weapon_class; }
string query_type() { return type; }

int query_magic()
{
  return query_property("magical") || !!query_property("__remote");
}

string *get_hit_lines(int dam, object defender, object attacker)
{
  mixed o;
  o=query_property("__hit_line_object");
  if(!o && type) o="/obj/daemon/messd";
  if(o)
    return (string *)o->get_hit_lines(dam,defender,attacker,type); 
}

int hit(object attacker)
{
  mixed o;

  if(query_property("no_multi_attack"))
    if(time() - time_stamp < 2)
      return 0;
  time_stamp=time();
  
  if(o=query_property("__remote"))
    return (int)o->remote_weapon_hit(attacker);

  return 0;
}

/* only called by player.c/living.c */
void set_wielded_by(object who)
{
  object o;
  if(wielded_by=who)
    if(o=query_property("__remote"))
      o->remote_do_wield(this_object());
}

/* only called by player.c/living.c */
void un_wield()
{
  mixed o;
  if(wielded_by)
    if(o=query_property("__remote"))
      o->remote_do_unwield(this_object(),wielded_by);

  wielded_by=0;
}

int drop(int silently)
{
  string str;

  if(wielded_by)
  {
    if(str=query_property("cursed"))
    {
      if(!silently)
	write(stringp(str)?str:"You can't drop it, it seems to be cursed.\n");
      return 1;
    }


    wielded_by->stop_wielding();
    if(!silently)
      tell_object(environment(),"You drop your wielded weapon.\n");
  }
  return ::drop(silently);
}

string extra_look()
{
  if(wielded_by)
    return ::extra_look();
  return query_property("__default_extra_look");
}


/*** Commands ***/

int do_cmd(string str)
{
  mixed o;

  if(::do_cmd(str)) return 1;

  switch(query_verb())
  {
  case "wield":
    if(!str || this_object()!=present(str,environment()))
    {
      notify_fail("Wield what?\n");
      return 0;
    }
    
    if(environment() != this_player())
    {
      write("You don't have it.\n");
      return 1;
    }
    
    if(wielded_by)
    {
      write("You are already wielding it.\n");
      return 1;
    }
    
    if(this_player()->query_worn()["shield"] && query_property("two_handed"))
    {
      write("You can't wield it because you are wearing a shield.\n");
      return 1;
    }
    
    if(o=query_property("__remote"))
      if(o->remote_prevent_wield(this_object()))
	return 1;
    
    this_player()->wield(this_object());
    
    return 1;
    
    
  case "unwield":
    notify_fail("Unwield what?\n");
    if(!str || !id(str)) return 0;
    
    if(!wielded_by)
    {
      write("You are not wielding it.\n");
      return 1;
    }
    
    if(str=query_property("cursed"))
    {
      write(stringp(str) ? str :
	    "You can't unwield it, it seems to be cursed.\n");
      return 1;
    }
    
    wielded_by->stop_wielding();
    
    say(this_player()->query_name() +" unwields "+ short() +".\n");
    write("You unwield "+ short() +".\n");

    return 1;
  }

  return 0;
}

status is_weapon() { return 1; }




