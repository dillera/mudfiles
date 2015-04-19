/* obj/armour.c
 *
 * This file defines a general purpose armour.
 *
 *
 * Revision:
 *   910425 MeTa     Some fixes
 *   910427 Lpd      Cleanup
 *
 *
 * See below for configuration functions 'set_xx' and
 * query functions 'query_xx'.
 *
 * The following configuration functions must be called:
 *
 *    set_class, set_weight and set_name
 *
 * And it is recommended that you call 'set_value'
 *
 *
 *
 * If you are going to copy this file, in the purpose of changing
 * it a little to your own need, beware:
 *
 * First try one of the following:
 *
 * 1. Do clone_object(), and then configure it. This object is specially
 *    prepared for configuration.
 *
 * 2. If you still is not pleased with that, create a new empty
 *    object, and make an inheritance of this object on the first line.
 *    This will automatically copy all variables and functions from the
 *    original object. Then, add the functions you want to change. The
 *    original function can still be accessed with '::' prepended on the name.
 *
 * The maintainer of this LPmud might become sad with you if you fail
 * to do any of the above. Ask other wizards if you are doubtful.
 *
 * The reason of this, is that the above saves a lot of memory.
 */

/* #define OLD_ARMOUR_SYSTEM   
   Undefine this to use the 'linked list' armour system instead of mappings..
   */
#undef OLD_ARMOUR_SYSTEM

#ifndef OLD_ARMOUR_SYSTEM
#define Worn_by (worn && environment())
#else
#define Worn_By worn_by
#endif

inherit "obj/property";

string name, alias, short_desc, long_desc, type, xlook, info;
string wear_func;
string *aliases;
object worn_by, next;
int worn, class, weight, value;


reset(arg) 
{
  if(arg)
    return;
  type       = "armour";
  name       = "armour";
  long_desc  = "You see nothing special about it.\n";
  short_desc = "A piece of armour";
  class      = 0;
  value      = 0;
  weight     = 1;
}

#ifdef OLD_ARMOUR_SYSTEM
link(ob) { next = ob; }

remove_link(str) 
{
  object ob;
  
  if (str == name) 
  {
    ob   = next;
    next = 0;
    return ob;
  }
    
  if (next)
    next = next->remove_link(str);
  
  return this_object();
}

rec_short() 
{
  if(next)
    return name + ", " + next->rec_short();   
  return name;
}

test_type(str) 
{
  if (str == type)
    return this_object();
  
  if (next)
    return next->test_type(str);
  
  return 0;
}

tot_ac() 
{
  int x;

  if (next)
    x = class + next->tot_ac();
  else
    x = class;
    
  if (previous_object()->is_armour())
    return x;
  else
    return x/3;
}

#else 

remove_link() 
{
  log_file("FIX", "old_armour_call: "+file_name(previous_object())+"\n"); 
  return this_object();
}

link() 
{
  log_file("FIX", "old_armour_call: "+file_name(previous_object())+"\n"); 
}

tot_ac() 
{
  log_file("FIX", "old_armour_call: "+file_name(previous_object())+"\n"); 
  if(Worn_by)
    return Worn_by -> calc_armour_class();
}

test_type(str)
{
  return (str == type) && this_object();
}
#endif

init() 
{
  add_action("wear", "wear");
  add_action("remove", "remove");
}

short() 
{
  if (!short_desc)
    return 0;
    
  if(query_property("cursed"))
  {
    if (worn)
      return short_desc + " (worn & cursed)";
  } else {
    if (worn)
      return short_desc + " (worn)";
  }    
  return short_desc;
}


long(str) 
{
  if (long_desc)
    write(long_desc);
}


id(str) 
{
  return str == name || str == alias || str == type || str == short_desc ||
  (aliases && member_array(str, aliases) != -1);
}




extra_look() 
{
  if (worn)
    return xlook;
    
  return 0;
}

do_wear() 
{
  object ob,weapon;
  int i;  

  if(type=="shield")
  {
    weapon=this_player()->query_wielded();
    if(weapon)
    {
      for(i=0;i<sizeof(weapon);i++)
	if(weapon[i]->query_property("two_handed"))
	  return write("You can't wear a shield while wielding a two handed weapon.\n"),1;
    }
  }

  
#ifdef OLD_ARMOUR_SYSTEM
  next = 0;
#endif
  ob = this_player() -> wear(this_object());
  
  if (!ob) 
  {
#ifdef OLD_ARMOUR_SYSTEM
    Worn_by = this_player();
#else
    worn_by = this_player();	/* Other objects inherit this one! */
#endif
    worn = 1;
    if(wear_func)
      call_other(wear_func,"wear_func",this_object(),this_player());
    return 0;
  }
  return ob;
}

wear(str) 
{
  object ob,weapon;
  int i;  
  if(str == "all") {
    if(!worn && (environment() == this_player()))
      do_wear();
    notify_fail("");
    return 0;
  }

  if (!id(str)) {
    notify_fail(capitalize(query_verb()) + " what?\n");
    return 0;
  }
    
  if (environment() != this_player()) {
    write("You must get it first!\n");
    return 1;
  }
    
  if (worn) {
    write("You already wear it!\n");
    return 1;
  }
  
    
  if(!(ob = do_wear()))
    return 1;
  if(ob==1)
    return 1;
  str = ob->query_armour_block();
  if(str)
    write(str);
  else
  {
    write("You already have an armour of type " + type + ".\n");
    write("Worn armour: " + ob->short() + ".\n");
  }
  return 1;
}


get() {
    return 1;
}

drop(silently) 
{
  if (worn) 
  {
    if (query_property("cursed") && query_verb() != "quit")
    {
      if (!silently)
	write("It seems to be stuck.\n");
      return 1;
    }

    if(Worn_by)			/* Profezzorn */
      Worn_by -> stop_wearing(type);
    worn = 0;
#ifdef OLD_ARMOUR_SYSTEM
    Worn_by = 0;
#else
    worn_by = this_player();	/* Other objects inherit this one! */
#endif
    if (!silently)
      tell_object(environment(this_object()), "You drop your worn armour.\n");
  }

  return 0;
}


do_remove() 
{
#ifdef OLD_ARMOUR_SYSTEM
  Worn_by->stop_wearing(name);
  Worn_by = 0;
#else
  Worn_by -> stop_wearing(type);
  worn_by = this_player();	/* Other objects inherit this one! */
#endif
  worn    = 0;
}

remove(str) 
{
  if ((!id(str) && str != "all") || !worn)
    return notify_fail("Remove what?\n"), 0;
  
  if(query_verb() != "quit" &&
     query_property("cursed") &&
     !(Worn_by->query_npc())) 
    /* Check added by Brom 940625. NPC's should always be able
       to drop cursed items,  especially when they die. */
    return write(short() + " seems to be stuck.\n"), 1;
  
  do_remove();

  if(str == "all")
    return notify_fail(""), 0;
  return write("Ok.\n"), 1;
}


/*
 * Obsolete functions for backwards compatibility
 */
set_id(n) { name = n; }
set_ac(a) { class = a*3; }
  
/*
 * Configuration functions
 */
  
/* Looks, extra ids, value and weight. */

set_name(n)        { name = n; short_desc = n; long_desc = n + ".\n"; }
set_short(s)       { short_desc = s; long_desc = s + ".\n"; }
set_long(l)        { long_desc = l; }
set_value(v)       { value = v; }
set_weight(w)      { weight = w; }
set_alias(a)       { alias = a; }
set_extra_look(el) { xlook = el; }
set_wear_func(ob)  { wear_func=ob; }


add_alias(string str) {
  if(aliases == 0) aliases = ({ });
  aliases += ({ lower_case(str) });
} 

set_aliases(string *all_aliases) { aliases = all_aliases;}


/* Armour specials. */
set_class(c)       { class = c; }
/*Added by Qqqq*/
set_type(t)        { type = t; }
set_arm_light(l)   { set_light(l); }

/* Misc functions */
set_info(i)        { info = i; }

/*
 * Query functions
 */
query_name()      { return name; }
query_value()     { return value; }
query_weight()    { return weight; }
armour_class()    { return class; }
query_ac()        { return class/3; }
/*Changed by Qqqq*/
query_type()      { if (class && type == "robe") type = "cloak"; return type; }
#ifdef OLD_ARMOUR_SYSTEM
query_worn()      { return worn && Worn_by; }
#else
query_worn()      { return Worn_by; }
#endif
query_info()      { return info; }
is_armour()       { return 1; }


clean_up(int arg)
{
  if(environment()) return 0;
  while(first_inventory()) destruct(first_inventory());
  if(_isclone(this_object()) ||
     (arg<2 && this_object()==_next_clone(this_object())))
    destruct(this_object());
  return 1;
}
