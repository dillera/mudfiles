/*
   
   This object is a standard soft_drink object and works
   like /obj/food.c or /obj/armour.c
   
   To use this you can do:
   inherit "obj/soft_drink";
   ......
   or,
   object ob;
   ob = clone_object("obj/soft_drink");
   ob->set_name("apple juice");
   
   *  For more documentation look at /doc/build/drinks
   
   
   These functions are defined:
   
           set_name(string)	To set the name of the item. For use in id().
   
   Two alternatie names can be set with the calls:
   
           set_alias(string) and set_alt_name(string)
   
           set_short(string)	To set the short description.
   
	   set_long(string)	To set the long description.

	   set_value(int)	To set the value of the item.

	   set_weight(int)	To set the weight of the item.

	   set_strength(int)	To set the healing power of the item. If you
				don't wish the item to have healing powers
				just set this value to 0.

	   set_drinker_mess(string)
				To set the message that is written to the 
				player when he drinks the item.

	   set_drinking_mess(string)
				To set the message given to the surrounding
				players when this object is drunk.

	   set_empty_container(string)
				The container of the liquid inside. For
				example "bottle" or "jug".


	For an example of the use of this object, please read:
*	/doc/examples/apple_juice.c

*/

inherit "obj/property";

#define TP this_player()
#define NAME TP -> query_name()

string name, short_desc, long_desc, drinking_mess;
string drinker_mess = "Gloink Glurk Glburp.\n";
string alias, alt_name, empty_container = "bottle";
int value, strength, weight = 1, full = 1, heal, pub_drink;
mapping id_map = ([]);

init()
{
  add_action("drink", "drink");
}

make_id()
{
  id_map = 
    ([
      name : 1, alt_name: 1, alias: 1, empty_container : 1,
      empty_container +" of "+ name : 1,  
      empty_container +" of "+ alt_name : 1,
      empty_container +" of "+ alias : 1 
      ]);
}

reset(arg) { 
  if(arg) return; 
  make_id();
  add_property("special_item", "The "+empty_container+" is returned to it's maker.\n"); 
}

prevent_insert(target_o)
{ 
  if(full)
  {
    if(pub_drink)
    {
      if (target_o)
      {
	full = 0;
	(deep_inventory(target_o) + ({target_o})) -> set_wet(strength*30);
	return 	write("As you put " + short_desc + " in the " +
		      target_o ->short() + " you spill it out!\n"), 1;
      } 
    }
    return write("You don't want to ruin " + name + ".\n"), 1;
  }
  return 0;
}

set_pub() { pub_drink = 1; }

set_not_pub() { pub_drink=0; }

id(str)
{
  if(id_map == ([]));
     make_id(); 
  if (full)
    return id_map[str] || str == "drk2" && pub_drink;
  else
    return str == empty_container || str == "empty " + empty_container;
}

short() 
{ 
  return (full ? (short_desc || 
		  (short_desc = "a"+ (vocp(empty_container) ? "n " : " ")+
		   empty_container +" of "+ name)) : 
	  "an empty "+ empty_container); 
}

vocp(c)
{
  if(stringp(c))
    switch(c[0])
    {
     case 'a': case 'e': case 'i': case 'o': case 'u':
      return 1;
    }
  return 0;
}

long() 
{
  write(full ? (long_desc || capitalize(short() +".\n")) : capitalize(short() +".\n"));
}

get() { return 1; }

drink(str)
{
  if (!full || !str || !id(str))
    return 0;
  
  
  if(TP -> query_level() * 8 < strength)
  {
    write("This is much too much for you to drink! "+
	  "You drool most of it on the ground.\n");
    say(NAME +" tries to drink "+ short_desc +
	" but drools most of it on the ground.\n");
    full = 0;
    return 1;
  }
  
  if(!TP -> drink_soft(strength) && !TP -> query_npc())
    return 1;
  
  full = 0;
  if (!heal)    
    TP -> heal_self(strength);
  else 
    TP -> heal_self(heal);  
  remove_property("special_item");
  say(NAME +(drinking_mess ? drinking_mess : " drinks "+ short_desc +".\n"));
  return write(drinker_mess), 1;
}

min_cost() { return 4 * strength + (strength * strength) / 10; }

query_name() { return full ? name : empty_container; }

set_name(n) { name = n; make_id(); }

set_short(s, i) 
{
  string *tmp;
  !i && (short_desc = s); 
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

set_long(l) { long_desc = l; }

set_value(v) { intp(v) && (value = v); }

set_weight(w) { w > 0 && (weight = w); }

set_heal(h) { intp(h) && (heal = h); }

set_strength(s) { if (intp(s)) { strength = s; if (!heal) heal = s; } }

set_alias(a) { alias = a; make_id(); }

set_alt_name(an) { alt_name = an; make_id(); }

set_drinking_mess(dm) { drinking_mess = dm; }

set_drinker_mess(dm) { drinker_mess = dm; }

set_empty_container(ec) { empty_container = ec; make_id(); }

set_full(num) { intp(num) && (full = num); }

/*
 * Things that other objects might want to know.
 */

query_value() { return full ? value : 10; }

query_empty() { return !full; }

query_full() { return full; }

query_weight() { return weight; }

query_strength() { return strength; }

query_heal() { if (!heal) return strength; else return heal; }

int clean_up(int arg)
{
  /* vi destruktar oss om vi {r en klon eller om det inte 
     finns n{gra kloner av oss */
  if(full)
    return 1;
  if(_isclone(this_object()) || (arg < 2 && this_object() == 
				 _next_clone(this_object())))
    destruct(this_object());
  return 1;
}
