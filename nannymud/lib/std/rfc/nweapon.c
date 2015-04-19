#pragma strict_types;

/* | Diffrence from the old /obj/weapon.c
   |
   | * set_ego(int) Where int is a level and if the player has less level
   |                than int he can't wield the weapon.
   |                You can also define an ego_object that can take care
   |                of the ego routine.
   | * in the wield function, if the property two_handed is present, you
   |   can't wield the weapon and wear a shield.
   | * There is support for items, add_item(string item, string desc).
   | * Weapon type, set_type(string) sets the type of the weapon.
   |   in get_hit_lines and you have defined "use_daemon" you will use
   |   a 'sound' daemon that gives standard sound for the weapon type
   |   you have choosen.
   |
   |
   |
   |
   |Changelog
   |Date      By            What
   |-------------------------------------------------
   |950506   Taren       First version
   |950513   Taren       Second version
   |
   |
 */

inherit "obj/property";

private mapping vars=([]);
private int time_stamp;
private int weight;
private int value;
private int weapon_class;
private object wielded_by;
private string long_desc;
private string short_desc;
private string name;

/* Standard Functions */

int id(string str)
{
  return !!(name==str||vars["aliases"][str]||vars["items"][str]);
}

string short()
{
  return short_desc;
}

void long(string str)
{
  if(name==str||vars["aliases"][str])    
    write(long_desc);
  else  write(vars["items"][str]+".\n");
}

void init()
{
  if(vars["read_msg"])
    add_action("read","read");
  add_action("wield","wield");
  add_action("unwield","unwield");  
}

int get()
{
  return 1;
}

mixed get_hit_lines(int dam, object defender, object attacker)
{
  if(vars["use_daemon"])
    "/std/messd"->get_weapon_hits_mess(dam,attacker,defender,vars["type"]);
  if(objectp(vars["hits_lines_object"]))
    vars["hits_lines_object"]->get_weapon_hits_mess(dam,attacker,defender);
}

int hit(object attacker)
{
  int tmp;
  if (query_property("no_multi_attack"))
    if (((tmp=time())-time_stamp)<2) return 0;
  time_stamp = tmp;
  return vars["hit_obj"] ? vars["hit_obj"]->weapon_hit(attacker) : 0;
}

/* Actions */

int wield(string str)
{
  string st;
  if(!str) return 0;
  if(!id(str)) return 0;
  if(wielded_by) return write("You already wield it!\n"),1;
  if(objectp(vars["wield_obj"]))
    return vars["wield_obj"]->do_wield(this_object());
  if(this_player()->query_worn()["shield"] && this_object()->query_property("two_handed") )
    return write("You can't wield it because you wear a shield.\n"),1;
  if(vars["ego"]&&this_player()->query_level()<vars["ego"])
    return write("You aren't powerful enough to wield this weapon.\n"),1;
  if(vars["ego_obj"]&&!(st=(string)vars["ego_obj"]->can_wield()))
    {
      write(st ? st : "You aren't powerful enough to wield this weapon.\n");
      return 1;
    }
  call_other(this_player(), "wield", this_object());
  return 1;
}

int unwield(string str,int silent)
{
  notify_fail("Unwield what?\n");
  if(!str) return 0;
  if(!id(str)) return notify_fail("You don't wield it.\n"),0;
  if(str=(string)query_property("cursed"))
    {
      write(stringp(str) ? str: "You can't unwield it, it seems to be cursed.\n");
      return 1;
    }
  if(wielded_by)
    {
      wielded_by->stop_wielding();
      wielded_by=0;
      if(!silent)
	{
	  say(this_player()->query_name() +" unwields "+ short() +".\n");
	  write("You unwield "+ short() +".\n");
	}
      return 1;
    }
}

int read(string str)
{
  return id(str) && (write(vars["read_msg"]),1);
}

int drop(int silent)
{
  int i;
  i=unwield(name,silent);
  if(!silent)
    write("You drop your wielded weapon.\n");
  return i;
}

/* Interface, set functions */

void add_alias(string str)
{
  if(stringp(str))
    {
      if(!vars["aliases"]) vars["aliases"]=([]);
      vars["aliases"][str]=1;
    }
}

void add_item(string str, string desc)
{
  if(stringp(str)&&stringp(desc))
    {
      if(!vars["items"]) vars["items"]=([]);
      vars["items"][str]=desc;
    }
}

void set_name(string str)
{
  name=str;
  short_desc = capitalize(str);
  long_desc = "You see nothing special.\n";
}

void set_short(string str)
{
  short_desc = str;
}

void set_long(string str)
{
  long_desc=str;
}

void set_type(string str)
{
  vars["type"]=str;
}

void set_hit_lines_object(object ob)
{
  vars["hit_lines_object"]=ob;
}

void set_wield_object(object ob)
{
  vars["wield_obj"]=ob;
}

void set_hit_object(object ob)
{
  vars["hit_obj"]=ob;
}

void set_wielded_by(object who)
{
  wielded_by=who;
}

void set_class(int class)
{
  if(class<0) return;
  weapon_class=class;
}

void set_weight(int i)
{
  weight=i;
}

void set_value(int i)
{
  value=i;
}

void set_ego(int class)
{
  vars["ego"]=class;
}

void set_ego_obj(object ob)
{
  vars["ego_obj"]=ob;
}

/* Interface, query functions */

string query_name()
{
  return name;
}

object query_wielded()
{
  return wielded_by;
}

int query_class()
{
  return weapon_class; 
}

int query_value()
{
  return value;
}

int query_magic()
{
  return vars["magic"] || vars["hit_obj"] ;
}

int query_weight()
{
  return weight;
}

string query_type()
{
  return vars["type"];
}


/*
   Clean up....
 */
int clean_up(int arg)
{
  if(environment()) return 0;
  while(first_inventory()) destruct(first_inventory());
  if(_isclone(this_object()) ||
     (arg<2 && this_object()==_next_clone(this_object())))
    destruct(this_object());
  return 1;
}


