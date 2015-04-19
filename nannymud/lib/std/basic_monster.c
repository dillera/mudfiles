/* Written by Profezzorn */

#pragma strict_types
#pragma save_types

#define USE_WETCODE
#define MASTER       "obj/daemon/monsterd"

inherit "/std/stationary";
inherit "/obj/living_functions";

private string race;
private int aggressive;

/*** setup_functions ***/
void set_name(string name)
{
  ::set_name(name);
  set_living_name(name);
  cap_name=capitalize(name);
}

void set_aggressive(int delay)
{
  aggressive=delay;
  if(find_call_out("do_attack_someone_if_visible")==-1)
    call_out("do_attack_someone_if_visible",aggressive);
}
void set_race(string r) { race=r; }

void set_level(int l)
{
  int tmp;
  if(l<0) l=1;
  level = l;
  Str = l;
  Int = l;
  Con = l;
  Dex = l;
  if (!l) tmp=1; else tmp=l;
  weapon_class = (MASTER->standard_wc(tmp));
  armour_class = (MASTER->standard_ac(tmp));
  hit_point    = (MASTER->standard_hp(tmp));
  max_sp = spell_points = max_hp = hit_point;
  experience = (int)"room/adv_guild"->query_cost(tmp-1);

  /* This is for level 1 monsters. */
  if (!experience)
    experience = random(500);
} /* set_level */

void set_hp(int hp) { max_hp = hit_point = hp; }
void set_sp(int sp) { max_sp = spell_points = sp; }
void set_ep(int ep) { experience = ep; }
void set_al(int al) { alignment = al; }
void set_wc(int wc) { weapon_class = wc; }
void set_ac(int ac) { armour_class = ac; }
void set_frog() { frog = 1; }

/*** functions called by driver/mudlib ***/

void reset(int arg)
{
  if(!arg)
  {
    enable_commands();
    set_living_name("thing");
    set_short("a thing");
  }
}

void init()
{
  mixed r;

  ::init();

  if(r=query_property("__remote"))
    r->remote_monster_init(this_object());

  if(aggressive && 
     this_player()->query_living() &&
     !this_player()->query_npc())
  {
    remove_call_out("do_attack_someone_if_visible");
    call_out("do_attack_someone_if_visible",aggressive);
  }
}

void do_attack_someone_if_visible()
{
  mixed r;
  object o;
  int livings;

  if(!aggressive || !environment()) return; /* agressive might have changed... */

  enable_commands();
  for(o=first_inventory(environment());o;o=next_inventory(o))
  {
    if(o->query_living() && !o->query_npc())
    {
      if(query_property("see_invisible") ||
	 ((set_light(0) > 0) && o->short()))
      {
	if(r=query_property("__remote"))
	{
	  if(!r->remote_prevent_attack(this_object(),o))
	  {
	    attack_object(o);
	    return;
	  }
	}else{
	  attack_object(o);
	  return;
	}
      }
      livings++;
    }
  }

  if(livings && -1==find_call_out("do_atatck_someone_if_visible"))
    call_out("do_attack_someone_if_visible",10+random(10)+aggressive);
}

int second_life()
{
  mixed r;
  if(r=query_property("__remote"))
    return r->remote_monster_died(this_object());
}

int real_second_life()
{
  mixed r;
  if(r=query_property("__remote"))
    return r->remote_real_monster_died(this_object());
}

string query_race() { return race; }
int query_aggressive() { return aggressive; }

string query_real_name()
{
  return stationary::query_name();
}

string query_name()
{
  return ghost ? "some mist" : capitalize(stationary::query_name());
}

int heart_beat_needed()
{
  return !!attacker_ob;
}

void heart_beat()
{
  age++; /* should really be called heart_beats :) */

  attack();

  if(!heart_beat_needed())
  {
    set_heart_beat(0);

    do_attack_someone_if_visible();
  }
}

int query_npc() { return 1; }

#ifdef USE_WETCODE

string query_long(string l)
{
  string s;
  s=::query_long(l);

  if(!query_item_map()[l]) /* Don't add stilly stuff to items */
  {
    switch(wet_time-time())
    {
    case 1000..0x7fffffff:
      s+=query_gender_string()+" is soaking wet.\n";
      break;
    case 600..999:
      s+=query_gender_string()+" is wet all over.\n";
      break;
    case 300..599:
      s+=query_gender_string()+" is wet.\n";
      break;
    case 100..299:
      s+=query_gender_string()+" is rather wet.\n";
      break;
    case 1..99:
      s+=query_gender_string()+" is damp.\n";
      break;
    default:
    }
  }
  return s;
}

int query_wet()
{
  if(wet_time>time()) return wet_time-time();
}

int set_wet(int hb)
{
  if(!hb) hb=360;
  wet_time=time()+hb*2;
  all_inventory(this_object())->set_wet(hb);
  return 1;
}

#endif
