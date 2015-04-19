/*
 * A general purpose monster. Clone this object,
 * and call local functions to configure it.
 */

#define USE_WETCODE
#define MASTER       "obj/daemon/monsterd"

/*
 * If you are going to copy this file, in the purpose of changing
 * it a little to your own need, beware:
 *
 * First try one of the following:
 *
 * 1. Do clone_object(), and then configure it. This object is specially
 *    prepared for configuration.
 *
 * 2. If you still is not pleased with that, create a new empty
 *    object, and make an inheritance of this objet on the first line.
 *    This will automatically copy all variables and functions from the
 *    original object. Then, add the functions you want to change. The
 *    original function can still be accessed with '::' prepended on the name.
 *
 * The maintainer of this LPmud might become sad with you if you fail
 * to do any of the above. Ask other wizards if you are doubtful.
 *
 * The reason of this, is that the above saves a lot of memory.
 */

#include <living.h>

/*
 * The heart beat is always started in living.h when we are attacked.
 */

string short_desc, long_desc, alias, alt_name, race;
int move_at_reset, aggressive;
object kill_ob;
status healing;		/* True if this monster is healing itself. */

string chat_head;	/* Vector with all chat strings. */
int chat_chance;

string a_chat_head;	/* Vector with all a_chat strings. */
int a_chat_chance;

object talk_ob;
string talk_func;	/* Vector of functions. */
string talk_match;	/* Vector of strings. */
string talk_type;	/* Vector of types. */
string the_text;

/* have_text should removed,
 * test should be done directly on the_text instead
 * grep and remove :)
 */
int have_text;

object dead_ob, real_dead_ob;
object init_ob;

int random_pick;

public int spell_chance, spell_dam1;
string spell_mess1, spell_mess2;
object create_room = previous_object();

reset(arg)
{
  if (arg) {
    if (move_at_reset)
      random_move();
    return;
  }
  is_npc = 1;
  enable_commands();
}

random_move()
{
  int n;
  switch(random(4))
  {
  case 0: command("west"); break;
  case 1: command("south"); break;
  case 2: command("north"); break;
  case 3: command("east"); break;
  }
}

short() {  return short_desc; }

long() { write (long_desc); }

id(str)
{
  return str == name || str == alias || str == race || str == alt_name;
}

heart_beat()
{
  int c;

  age ++;
#ifdef USE_WETCODE
  if (wet_time > 0 &&
      !environment(this_object())->query_property("waterfilled"))
  {
    wet_time--;
      
    if (!random(40))
      say("Some water drips from " + capitalize(name) + ".\n");
  }
#endif
  if(!test_if_any_here())
  {
    if(have_text && talk_ob)
    {
      have_text = 0;
      test_match(the_text);
    } else {
      set_heart_beat(0);
      if (!healing)
	heal_slowly();
      return;
    }
  }
  if (kill_ob && present(kill_ob, environment(this_object())))
  {
    if (random(2))
      return;			/* Delay attack some */

    attack_object(kill_ob);
    kill_ob = 0;
    return;
  }
  if (!query_property("peaceful") && attacker_ob && 
    present(attacker_ob, environment(this_object())) &&
      spell_chance > random(100)) /* Added property peaceful, Qqqq 95 */
  {
    say(spell_mess1 + "\n", attacker_ob);
    tell_object(attacker_ob, spell_mess2 + "\n");
    call_other(attacker_ob, "hit_player", random(spell_dam1));
  }
  attack();
  if (attacker_ob && whimpy && hit_point < max_hp/5)
    call_out("run_away", 2);

  if(chat_chance || a_chat_chance)
  {
    c = random(100);
    if(attacker_ob && a_chat_head)
    {
      if(c < a_chat_chance)
      {
	if (environment())
	  tell_room(environment(), a_chat_head[random(sizeof(a_chat_head))]);
      }
    } else {
      if(c < chat_chance && chat_chance)
      {
	if (environment())
	  tell_room(environment(), chat_head[random(sizeof(chat_head))]);
      }
    }
  }
  if(random_pick)
  {
    if(random(100) < random_pick)
      pick_any_obj();
  }
  if(have_text && talk_ob)
  {
    have_text = 0;
    test_match(the_text);
  }
}

can_put_and_get(str) { return !!str; }

int busy_catch_tell;

catch_tell(str)
{
  if (busy_catch_tell == time())
    return;
  busy_catch_tell = time();
  if(talk_ob)
  {
    if(have_text)
      test_match(the_text);

    have_text = 1;
    the_text = str;
  }
  busy_catch_tell = 0;
}

/*
 * Call the following functions to setup the monster.
 * Call them in the order they appear.
 */

set_name(n)
{
  name = n;
  set_living_name(n);
  alignment = 0;		/* Neutral monster */
  cap_name = capitalize(n);
  short_desc = cap_name;
  long_desc = "You see nothing special.\n";
}

set_level(l)
{
  int tmp;
  level = l;
  if (level<0) level = 0;	/* Better safe than sorry.*/
  Str = l; Int = l; Con = l; Dex = l;
  if (!l) tmp=1; else tmp=l;
  weapon_class = (MASTER->standard_wc(tmp));
  armour_class = (MASTER->standard_ac(tmp));
  hit_point    = (MASTER->standard_hp(tmp));
  if (max_hp < hit_point)
    max_hp = hit_point;
  spell_points = max_hp;
  experience = call_other("room/adv_guild", "query_cost", tmp-1);
  /* This is for level 1 monsters. */
  if (!experience)
    experience = random(500);
} /* set_level */

/* Optional */
set_alias(a) { alias = a; }
/* Optional */
set_alt_name(a) { alt_name = a; }
/* Optional */
set_race(r) { race = r; }
/* optional */
set_hp(hp) 
{ 
  if (max_hp < hp)
    max_hp = hp;
  hit_point = hp;
}
/* optional */
set_max_hp(mhp) { max_hp=mhp; }
/* optional. Can only be lowered */
set_ep(ep) { if (ep < experience) experience = ep; }
/* optional */
set_al(al) {
 if (al>1000) al = 1000;
 if (al<-1000) al = -1000;
 alignment = al;
}
/* optional */
set_short(sh) { short_desc = sh; long_desc = short_desc + "\n";}
/* optional */
set_long(lo) { long_desc = lo; }
/* optional */
set_wc(wc) { if (wc > weapon_class) weapon_class = wc; }
/* optional */
set_ac(ac) { if (ac > armour_class) armour_class = ac; }
/* optional */
set_move_at_reset() { move_at_reset = 1; }

/* optional
 * 0: Peaceful.
 * 1: Attack on sight.
 */
set_aggressive(a) {
    aggressive = a;
}
/*
 * Now some functions for setting up spells !
 */
/*
 * The percent chance of casting a spell.
 */
set_chance(c) { spell_chance = c; }

/* Message to the victim. */
set_spell_mess1(m) { spell_mess1 = m; } 
set_spell_mess2(m) { spell_mess2 = m; }
set_spell_dam(d) { spell_dam1 = d; }

query_chance() { return spell_chance; }
query_spell_dam() { return spell_dam1; }

/* Set the frog curse. */
set_frog() { frog = 1; }

/* Set the whimpy mode */
set_whimpy() { whimpy = 1; }

/*
 * Force the monster to do a command. The force_us() function isn't
 * always good, because it checks the level of the caller, and this function
 * can be called by a room.
 */
init_command(cmd) { command(cmd); }

load_chat(chance, strs)
{
  sizeof(strs);			/* Just ensure that it is an array. */
  chat_head = strs;
  chat_chance = chance;
}

/* Load attack chat */

load_a_chat(chance, strs)
{
  sizeof(strs);			/* Just ensure that it is an array. */
  a_chat_head = strs;
  a_chat_chance = chance;
}

/* Catch the talk */

set_match(ob, func, type, match)
{
  object old;

  if (sizeof(func) != sizeof(type) || sizeof(match) != sizeof(type))
    return;
  talk_ob = ob;
  talk_func = func;
  talk_type = type;
  talk_match = match;
  /*  say("talk match length " + sizeof(func) + "\n"); */  /* Oros */
}

set_dead_ob(ob) { dead_ob = ob; }
set_real_dead_ob(ob) { real_dead_ob = ob; }

second_life()
{
  if(dead_ob)
    return dead_ob->monster_died(this_object());
}

/*  With second_life, the monster dies anyway!
 *  Because of compatibility, I couldn't just change it,
 *  so I added this. -- Padrone, July 6, 1992
 */
real_second_life()
{
  if(real_dead_ob)
    return real_dead_ob->monster_died(this_object());
}

set_random_pick(r) { random_pick = r; }

add_money(num) { /* Added by Qqqq*/
  if (num > 5000)
    num = 5000;
  return ::add_money(num);
}

pick_any_obj() 
{
  object ob, w;
  int weight, cl;
  string object_short;

  ob = first_inventory(environment(this_object()));
  while(ob) 
  {
    if (ob -> get() && (object_short=ob -> short()))
    {
      weight = ob -> query_weight();
      if (!add_weight(weight)) 
	return say(cap_name + " tries to take " + object_short +
		   " but fails.\n");
      say(cap_name + " takes " + object_short + ".\n");
      move_object(ob, this_object());
      if ((cl = ob -> weapon_class()) && cl > this_object() -> query_wc())
	command("wield "+ ob -> query_name());
      else if (ob -> armour_class())
	command("wear "+ ob -> query_type());
      return;
    }
    ob = next_inventory(ob);
  }
}

set_init_ob(ob) { init_ob = ob; }

init()
{
  if(this_player() == this_object())
    return;

  if(init_ob)
    if(init_ob->monster_init(this_object()))
      return;
  if (attacker_ob)
    set_heart_beat(1);		/* Turn on heart beat */

  if(this_player() && !this_player()->query_npc())
  {
    set_heart_beat(1);
    if (aggressive)
      kill_ob = this_player();
  }
}

query_create_room() { return create_room; }
query_race() { return race; }

test_match(str)
{
  string who, str1, type, match, func, tmp;
  int i;

  type="";
  while(i < sizeof(talk_match))
  {
    if (talk_type[i]) type = "%s "+talk_type[i]+"%s";
    if (talk_func[i]) func = talk_func[i];
    if(sscanf(str,type,who,tmp)==2)
    {
      match = talk_match[i] || "";

      if (sscanf(tmp, match + " %s\n",str1)  ||
	  tmp == match + "\n" ||
	  sscanf(tmp, match + "%s\n",str1) ||
	  tmp ==" " + match + "\n" ||
	  sscanf(tmp, " " + match + " %s\n",str1))
      {
	return call_other(talk_ob, func, str, who, str1);
      }
    }
    i ++;
  }
}

/*
 * The monster will heal itself slowly.
 */
heal_slowly()
{
  hit_point += 120 / (INTERVAL_BETWEEN_HEALING * 2);
  if (hit_point > max_hp) 
    hit_point = max_hp;
  spell_points += 120 / (INTERVAL_BETWEEN_HEALING * 2);
  if (spell_points > max_hp)
    spell_points = max_hp;
  healing = 1;
  /*    
     if (hit_point < max_hp || spell_points < max_hp)
     call_out("heal_slowly", 120);  Commented out by Qqqq since only 77 %
     of the HB were executed
     else 
     healing = 0;
     */
}

query_aggressive() { return aggressive; }
change_ac(num) { armour_class = num; }
change_wc(num) { weapon_class = num; }
set_str(i) { Str = i; }
set_int(i) 
{
  Int = i;
  max_sp = 42 + query_int() * 8;
}

set_con(i) 
{
  Con = i;
  max_hp = (int)((MASTER)->standard_hp(Con));
}

set_dex(i) { Dex = i; }
