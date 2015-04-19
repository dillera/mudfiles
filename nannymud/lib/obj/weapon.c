/*
 * This file defines a general purpose weapon. See below for configuration
 * functions: set_xx.
 */

/*
 * Use this object to create a weapon for your area. There are basically two
 * ways of doing this in a nice manner.
 *
 * 1. Do clone_object(), and then configure it. This object is specially
 *    prepared for configuration.
 *
 * 2. If 1/ is noot enough for your needs, create an empty object and inherit
 *    this object on the first line. This will automatically copy all
 *    variables and functions from the original object. Then, add the
 *    functions you want to change. The original function can still be 
 *    accessed with '::' prepended on the name.
 *
 * The maintainer of this LPmud might become sad with you if you fail
 * to do any of the above. Ask other wizards if you are doubtful.
 *
 * The process described above saves memory and makes it much, much more easy
 * for the admin to keep the game up and running. Do _NOT_ keep own copies of
 * this file; it might get obsolete (and non-compatible) within days.
 */

# define TIME_STAMP

inherit "obj/property";

status wielded;
object wielded_by;
string name_of_weapon;
string cap_name;
string alt_name;
string alias_name;
string short_desc;
string long_desc;
string read_msg;
int class_of_weapon;
int value;
int local_weight;
object hit_func;
object wield_func;
string info;
status magic;
object hit_lines_object;
string *aliases;
#ifdef TIME_STAMP
int time_stamp;
#endif

set_hit_lines_object(obj) { hit_lines_object = obj; }

query_hit_lines_object() { return hit_lines_object; }

get_hit_lines(dam,defender,attacker) {
  if (!hit_lines_object || !objectp(hit_lines_object)) 
    return 0;
  return hit_lines_object->hit_lines(dam,defender,attacker);
}


query_name() { return name_of_weapon; }

long() { write(long_desc); }

reset(arg) 
{
  if (arg) return;
  wielded = 0; value = 0;
}

init() 
{
  if (read_msg) 
    add_action("read", "read");
  add_action("wield", "wield");
  add_action("do_unwield", "unwield");
}

wield(str) 
{
  if (!id(str))
    return 0;
  if (environment() != this_player()) 
    /* write("You must get it first!\n"); */
    return 0;
    
  if (wielded) 
    return write("You already wield it!\n"), 1;

  if(wield_func)
    if(!call_other(wield_func,"wield",this_object())) 
      return 1;
  
  if(this_player()->query_worn()["shield"] && this_object()->query_property("two_handed") )
    return write("You can't wield it because you wear a shield.\n"),1;
  
  call_other(this_player(), "wield", this_object());
  return 1;
}

do_unwield(str, silently)
{
  if (str && !id(str))
    return notify_fail("Unwield what?\n"), 0;
  if(!wielded) 
    return notify_fail("You aren't wielding it!\n"), 0;
  
  if(unwield(silently)[1] && !silently)
  {
    say(this_player()->query_name() +" unwields "+ short() +".\n");
    write("You unwield "+ short() +".\n");
  }
  return 1;
}


set_wielded_by(object who)
{
  wielded_by=who;
  wielded=1;
}

query_wielded() { return wielded ? wielded_by : 0; }

short() { return short_desc; }

weapon_class() { return class_of_weapon; }

id(str) 
{
  return (name_of_weapon && str == name_of_weapon) ||
    (alt_name && str == alt_name) ||
      (alias_name && str == alias_name) ||
       ( aliases && member_array(str, aliases) != -1);
}

/*Added by Qqqq 930804 */

query_wielded_by() { return wielded_by; }

drop(silently) 
{
  int *i;
  if((i=unwield(silently))[1] && !silently)
    tell_object(environment(),"You drop your wielded weapon.\n");
  return i[0];
}

unwield(silently) 
{ 
  string str;
  if (wielded) 
  {
    /*Added cursed ability on weapons, 930804*/
    if (str = query_property("cursed")) {
      if (!silently)
	write(stringp(str) ? str : 
	      "You can't drop it, it seems to be cursed.\n");
      return ({1, 0});
    }
    if (wielded_by) 
      wielded_by -> stop_wielding();
    wielded = 0;
    return ({0, 1});
  } 
  return ({0, 0});

}

un_wield() { wielded && (wielded = 0); }

hit(attacker) {
# ifdef TIME_STAMP
  int tmp;
  if (query_property("no_multi_attack"))
    if (((tmp=time())-time_stamp)<2) return 0;
  time_stamp = tmp;
# endif
  return hit_func ?  hit_func -> weapon_hit(attacker) : 0;
}

set_id(n) { set_name(n); }

set_name(n) {
  name_of_weapon = n;
  cap_name = capitalize(n);
  short_desc = cap_name;
  long_desc = "You see nothing special.\n";
}

read(str) { return id(str) && (write(read_msg), 1); }

query_value() { return value; }

get() { return 1; }

query_weight() { return local_weight; }

set_class(c) { class_of_weapon = c; }

set_weight(w) { local_weight = w; }

set_value(v) { value = v; }

set_alt_name(n) { alt_name = n; }

set_hit_func(ob)
{
 /* Fix by Profezzorn 1/10-95 */
  if(objectp(ob) && !_isclone(ob)) ob=file_name(ob);
  hit_func = ob;
}

set_wield_func(ob) { wield_func = ob; }

set_alias(n) { alias_name = n; }

add_alias(string str) {
  if(aliases == 0) aliases = ({ });
  aliases += ({ lower_case(str) });
} 

set_aliases(string *all_aliases) { aliases = all_aliases;}

set_short(s) 
{
  string *tmp;
  short_desc = s; 
  if(!long_desc && stringp(short_desc))
  {
    tmp = explode(short_desc, " ");
    if(sizeof(tmp))
    {
      s = lower_case(tmp[0]);
      if(s == "a" || s == "an" || s == "the")
	tmp[0] = "the";
      long_desc = 
	sprintf("%-=79s\n", "You see nothing special about "+ 
		implode(tmp, " ") +".");
    }
  }
}

set_long(long) { long_desc = long; }

set_read(str) { read_msg = str; }

set_info(i) { info = i; }

set_magic() { magic = 1; }

query_info() { return info; }

query_magic() { return !!(hit_func || wield_func || magic); }

clean_up(int arg)
{
  if(environment()) return 0;
  while(first_inventory()) destruct(first_inventory());
  if(_isclone(this_object()) ||
     (arg<2 && this_object()==_next_clone(this_object())))
    destruct(this_object());
  return 1;
}

is_weapon() { return 1; } 
