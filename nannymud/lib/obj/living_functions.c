#include <log.h>
#include <move_flags.h>
#pragma save_types
#pragma strict_types

/*
 * Rules of thumb (by Profezzorn)
 * 1. Don't code special cases into living that applies only to players
 *    if you can code it into player.c instead.
 */

#define USE_FOLLOW     /* Experimental "follow" code, see player.c also */
#define MAX_FOLLOWERS 10

#define USE_WETCODE    /* Experimental "wet" code */
/* #define USE_MULTS    /* Experimental multipliers (turned off) */


#define KILL_NEUTRAL_ALIGNMENT		10
#define ADJ_ALIGNMENT(al)		((-al - KILL_NEUTRAL_ALIGNMENT)/4)
#define NAME_OF_GHOST			"some mist"

/* Define this to use the 'linked list' armour system instead of mappings.. */
#undef OLD_ARMOUR_SYSTEM

#define HLD "obj/daemon/hld" /* A log-daemon for $ and heals. */

#define USE_PLACE_CHECK

/* This is for keeping control on wizards that run around is areas.
 * Some wizards tend to gather info about quests with their wizchar.
 * Undefine it to turn it of /Bive 960107 */

/* #define USE_PLACE_CHECK
   wizards would get the file_name of the room they've been in,
   written to them. How this happens, well...
*/

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

/*
 * Inherit this file in objects that "lives".
 * The following variables are defined here:
 * .......
 */

inherit "/obj/hook"; /* Qqqq, 95 */

#ifdef USE_FOLLOW
static object *follow_list;  /* Objects following us */
static object follow_ob;    /* Object we are following */

static int follow_update();
static int do_follow(string dest_dir, mixed dest_ob, object dest);
#endif

int money;		           /* Amount of money on player. */
string msgin, msgout;	           /* Messages when entering or leaving a room. */
int level;		           /* Level of monster. */
static int armour_class;	   /* What armour class of monster. */
int hit_point;		           /* Number of hit points of monster. */
int max_hp, max_sp;
int experience;		           /* Experience points of monster. */
int frog;		           /* If the living is a frog */
string mmsgout;		           /* Message when leaving magically. */
string mmsgin;		           /* Message when arriving magically. */
static object attacker_ob;	   /* Name of player attacking us. */
static object alt_attacker_ob;	   /* Name of other player also attacking us. */
static int weapon_class;	   /* How good weapon. Used to calculate damage. */
static object name_of_weapon;	   /* To see if we are wielding a weapon. */
#ifdef OLD_ARMOUR_SYSTEM
static object head_armour;	   /* What armour we have. */
#else
static mapping worn_armour = ([]); /* A mapping containing all worn armours
				      on this format: [ "type":object ] */
#endif
int ghost;		           /* Used for monsters that can leave a ghost. */
static int local_weight;	   /* weight of items */
static object hunted, hunter;	   /* used in the hunt mode */
static int hunting_time;	   /* How long we will stay in hunting mode. */
static string cap_name;	           /* Capitalised version of "name". */
int spell_points;	           /* Current spell points. */
static string spell_name;
static int spell_cost, spell_dam;
int age;		           /* Number of heart beats of this character. */
int is_invis;		           /* True when player is invisible */
int whimpy;		           /* Automatically run when low on HP */
int dead;		           /* Are we alive or dead? */
/*
 * All characters have an aligment, whatever that is.
 * This value is updated when killing other players.
 */
int alignment;
int gender;	                   /* 0 means neuter ("it"), 1 male ("he"),  2 female ("she") */

#define WARRAY ({ "east", "west", "north", "south", "out", "in", "northeast",\
		    "southeast", "northwest", "southwest", "up", "down" })
/*
 * Character stat variables.
 */

int Str, Int, Con, Dex;

/*
 * The following routines are defined for usage:
 * stop_fight		Stop a fight. Good for scroll of taming etc.
 * hit_player		Called when fighting.
 * transfer_all_to:	Transfer all objects to dest.
 * move_player:		Called by the object that moves the monster.
 * query_name:		Gives the name to external objects.
 * attacked_by		Tells us who are attacking us.
 * show_stats		Dump local status.
 * stop_wielding	Called when we drop a weapon.
 * stop_wearing		Called when we drop an armour.
 * query_level		Give our level to external objects.
 * query_value		Always return 0. Can't sell this object.
 * query_npc		Return 1 if npc otherwise 0.
 * get			Always return 0. Can't take this object.
 * attack		Should be called from heart_beat. Will maintain attack.
 * query_attack
 * drop_all_money	Used when the object dies.
 * wield		Called by weapons.
 * wear			Called by armour.
 * add_weight		Used when picking up things.
 * heal_self		Enable wizards to heal this object.
 * restore_spell_points Enables wizards to heal spell points.
 * can_put_and_get	Can look at inventory, but not take things from it.
 * attack_object	Called when starting to attack an object.
 * test_if_any_here	For monsters. Call this one if you suspect no enemy
 *			is here any more.
 *			Return 1 if anyone there, 0 if none.
 * force_us		Force us to do a command.
 * query_spell_points	Return how much spell points the character has.
 * query_sp             Same as query_spell_points, which will be rm'd after
 *                      some time (and after grep'ing the mud).
 * reduce_hit_point	Reduce hit points, but not below 0.
 * add_hp               Add hp (pos or neg) to the object. Sanity checks.
 * add_sp               Add hp (pos or neg) to the object. Sanity checks.
 */


/*** Prototypes ***/

mixed query_property(string str);
void attack_object(object ob);
void stop_fight();
varargs int add_hp(int hps, int ignore);
int stop_wielding() ;

/*** Msgin / msgout handling ***/
int setmin(string m) { msgin=m; return 1; }
int setmout(string m) { msgout=m; return 1; }
int setmmin(string m) { mmsgin=m ; return 1; }
int setmmout(string m) { mmsgout=m; return 1; }

string query_msgin()   { return msgin;   }
string query_msgout()  { return msgout;  }
string query_mmsgin()  { return mmsgin;  }
string query_mmsgout() { return mmsgout; }

/*-Collected all the simple query functions here /Profezzorn, 960209-*/

int query_frog() { return frog; }
int query_living() { return 1; }
int query_npc() { return 1;}
int query_dead() { return dead; } /* Angmar 920809 */
int query_level() { return level; }
int query_ghost() { return ghost; }
  
string query_name() { return ghost ? NAME_OF_GHOST : cap_name; }
string query_real_name() {  return lower_case(cap_name||""); }

int query_alignment() { return alignment; }
int query_hp() { return hit_point; }

/* Added by Brom 951023. Will replace query_spell_points in future. */
int query_sp() { return spell_points; }
int query_wimpy() { return whimpy; }
int query_spell_points() { return spell_points; }

/*Added by Qqqq 920111*/
int query_max_sp() { return max_sp; }

/* Angmar 901104 */
int query_max_hp() { return max_hp; }

int query_money() { return money; }
int query_exp() { return experience; }

int query_wc() { return weapon_class; }
int query_ac() { return armour_class; }
int query_age() { return age; }
int query_str() { return Str; }
int query_int() { return Int; }
int query_con() { return Con; }
int query_dex() { return Dex; }

/* Those (query_real_*) should be grep'ed for, replaced and rm'd. */
int query_real_str() { return Str; }
int query_real_int() { return Int; }
int query_real_con() { return Con; }
int query_real_dex() { return Dex; }

#ifdef USE_MULTS
query_str_mult() { return Str_Mult; }
query_int_mult() { return Int_Mult; }
query_con_mult() { return Con_Mult; }
query_dex_mult() { return Dex_Mult; }

set_str_mult(i) { }
set_int_mult(i) { }
set_con_mult(i) {}
set_dex_mult(i)  {  }
void reset_mults()  {  }
#endif

/*----------- Most of the gender handling here: ------------*/

int query_gender() { return gender; }
int query_neuter() { return !gender; }
int query_male() { return gender == 1; }
int query_female() { return gender == 2; }

void set_gender(int g) {
  if (g == 0 || g == 1 || g == 2)
    gender = g;
}
void set_neuter() { gender = 0; }
void set_male() { gender = 1; }
void set_female() { gender = 2; }

string query_gender_string() 
{
  switch(gender)
  {
   case 0:  return "neuter";
   case 1:  return "male";
   default: return "female";
  }
}

string query_pronoun() 
{
  switch(gender)
  {
   case 0:  return "it";
   case 1:  return "he";
   default: return "she";
  }
}

string query_possessive() 
{
  switch(gender)
  {
   case 0:  return "its";
   case 1:  return "his";
   default: return "her";
  }
}

string query_objective() 
{
  switch(gender)
  {
   case 0:  return "it";
   case 1:  return "him";
   default: return "her";
  }
}

/* Moved invis basics from player.c to living.c   /Gwendolyn 940608 */

status query_invis() { return is_invis; }

varargs status invis(mixed arg)
{
   if(is_invis)
   {
      if(!arg)
	 tell_object(this_object(), "You are already invisible.\n");
      return 1;
   }
/*  Make it look like a wiz with level > 22 logged out when going invis  */
   if(level > 22 && interactive(this_object()))
      catch("/obj/daemon/mudwhod"->send_logout(this_object()));
   is_invis = 1;
   if(!arg)
   {
      tell_object(this_object(), "You are now invisible.\n");
      say(cap_name + " " + mmsgout + ".\n", this_object());
   }
   cap_name = "Someone";
   return 1;
}

varargs status vis(mixed arg)
{
   if(!is_invis)
   {
      if(!arg)
	 tell_object(this_object(), "You are not invisible.\n");
      return 1;
   }
/*  Make it look like a wiz with level > 22 logged in when going vis  */
   if(level > 22 && interactive(this_object()))
      catch("/obj/daemon/mudwhod"->send_login(this_object()));
   is_invis = 0;
   cap_name = capitalize(query_real_name());
   if(!arg)
   {
      tell_object(this_object(), "You are now visible.\n");
      say(cap_name + " " + mmsgin + ".\n", this_object());
   }
   return 1;
}
/*------------------------------------------------*/


/* This function should be grep:ed for and if not used inlined */
void tell_player_or_npc(object *o,string player_msg,string npc_msg)
{
  int e;
  if(!sizeof(o)) return;
  do{
    tell_object(o[e],interactive(o[e])?player_msg:npc_msg);
  }while(++e<sizeof(o));
}

/* This function could be inlined in move_player */
static varargs int real_move_player(string dir,mixed dest,int moveflags)
{
  /* cleaned by profezzorn 9/2-94 */
  /* Rather trashed... Hopefully working again now   /Gwendolyn */
  /* Cleaned again 10 Jun 94, by Milamber */
  /* made compatible by Profezzorn 6/8-94 */

  string tmp_name;
  string dir_name,min,mout,npcmin;
  object last_room; /* Qqqq, 95 */
#ifdef USE_FOLLOW
  follow_update();
#endif
  if (block_hook("block_move_player_hook",({dir, dest}))) /* Qqqq 95 */
    return -1;
  last_room = environment(); /* Qqqq 95 */

  if (attacker_ob && present(attacker_ob))
  {
    hunting_time = 20;
    if (!hunter)
      tell_object(this_object(), "You are now hunted by " +
		  attacker_ob->query_name() + ".\n");
    hunter = attacker_ob;
  }

  tmp_name = query_name();

  if(!query_invis() && environment())
  {
    if(sscanf(dir,"%s\n%s\n%s",dir_name,min,mout)!=3)
    {
      dir_name=dir;
      if(dir_name=="X")
      {
	if(!mmsgout) mmsgout = "disappears in a puff of smoke";
	if(!mmsgin) mmsgin = "arrives in a puff of smoke";
	mout = mmsgout;
	npcmin=min = mmsgin;
      } else {
	string *f;
	int i;
	if(!msgout)  msgout = "leaves";
	if(!msgin) msgin = "arrives";
	mout = msgout + " " + dir_name;
	npcmin=min = msgin;
	if(pointerp(f = (mixed *) load_object(dest)->query_dest_dir()) &&
	   (i = member_array(file_name(environment()), f)) != -1 &&
	   stringp(f[i+1]))
	{
	  switch(f[i+1])
	  {
	   case "west":
	   case "east":
	   case "north":
	   case "south":
	   case "northwest":
	   case "southwest":
	   case "northeast":
	   case "southeast":
	   case "metawest":
	   case "metaeast":
	   case "metanorth":
	   case "metasouth":
	    min += " from the " + f[i+1];
	    break;

	   case "up":
	    min += " from above";
	    break;

	   case "down":
	    min += " from below";
	    break;

	   case "metaup":
	    min += " from metaabove";
	    break;

	   case "metadown":
	    min += " from metabelow";
	    break;

	   default:
	    break;
	  }
	}
      }
    }else{
      npcmin=min;
    }

    if (!(moveflags & MOVE_NO_LEAVE_MESS)) {    
      if(set_light(0)>0)
	{
	  /* it is light in here */
	  if(mout[-1] != '\n') mout += ".\n";
	  say(tmp_name + " " + mout);
      } else {
	say("Someone left.\n");
      }
    }
    
    move_object(this_object(), dest);

    if (!(moveflags & MOVE_NO_ENTER_MESS)) {
      if(set_light(0)>0)
	{
	  /* it is light in here too */
	  if(npcmin!=min)
	    {
	      if(min[-1] != '\n') min +=".\n";
	      if(npcmin[-1] != '\n') npcmin +=".\n";
	      tell_player_or_npc(all_inventory(environment())-({this_object()}),tmp_name+" "+min,tmp_name+" "+npcmin);
	  }else{
	    if(min[-1] != '\n') min +=".\n";
	    say(tmp_name+" "+min);
	  }
      } else {
	say("Someone arrived\n");
      }
    }
  }
  else {
    move_object(this_object(), dest);
  }

  hook("move_player_hook",({dir, last_room})); /* Qqqq 95 */  

  if (hunted && present(hunted))
    attack_object(hunted);

  if (hunter && present(hunter) && !is_invis)
    hunter->attack_object(this_object());

  
#ifdef USE_FOLLOW
  /* Make the players/NPCs follow me unless I'm teleporting,
     is invisible or is a ghost */
  if (dir != "X" && !ghost && !query_invis())
    do_follow(dir, environment(), environment());
#endif

  if (query_npc())
    return 1;

  this_object() -> update_prompt();
  if(level > 19 && interactive(this_object()))
  {
     object obj;
     string fn, a, fn_check;
     int b;
     fn = file_name(environment());
#ifdef USE_PLACE_CHECK
  /* Let's log all places a wizard enters and do not have read-access to... */
	fn_check = fn;
	fn_check += ".c";
	if(this_object()->query_level() < 23) {
	  if(file_size(fn_check) == -1) {
	    write_file("log/place_log",this_object()->query_real_name()+" into "+fn_check+" "+ctime(time())+"\n");
	  }
	}
#endif
     while(fn[0] == '/')
	fn = fn[1..10000];
     fn = "/" + fn;
     if(obj = present("wizsoul", this_object()))
	fn = replace(fn, "/" + (string) obj->mk_path("."), "");
     a = fn + "/";
     b = strlen(query_real_name());
     if(a[0..9+b] == "/players/" + query_real_name() + "/")
	fn = "~" + fn[9+b..1000];
     else if(fn[0..8] == "/players/")
	fn = "~" + fn[9..1000];
     if(sscanf(fn, "%s#%d", a, b) == 2)
	fn = "Clone of " + a + " (" + b + ")";
     tell_object(this_object(), fn +"\n");
  }

  if (!(moveflags & MOVE_NO_LOOK)) {
    if(!environment()->dark("move_player", 
			    this_object()->query_toggles("brief")))
      {
	if (set_light(0) < 1)
	  tell_object(this_object(), "A dark room.\n");
	else 
	  this_object()->display_look(environment(this_object()));
      }
  }
}

/*
 * This routine is called from objects that moves the player.
 * Special: direction "X" means teleport.
 * The argument is "how#where".
 * The second optional argument is an object to move the player to.
 * If the second argument exists, then the first argument is taken
 * as the movement message only.
 * The different flags to moveflags can be found in /include/move_flags.h
 */
varargs int move_player(string dir_dest,mixed optional_dest_ob,int moveflags)
{
  if (optional_dest_ob ||
      sscanf(dir_dest,"%s#%s",dir_dest,optional_dest_ob)==2)
    return real_move_player(dir_dest, optional_dest_ob,moveflags);
}

void transfer_all_to(mixed dest)
{
  int e;
  object *arms;
  object ob;
  object next_ob;

  stop_wielding();
  arms = (m_values(worn_armour) - ({0}));
  for (e=0; e < sizeof(arms); e++)
    arms[e] -> do_remove();
  worn_armour = ([]);

  ob = first_inventory(this_object());
  while(ob) {
    next_ob = next_inventory(ob);
    if (ob->query_weapon() && ob == name_of_weapon)
    {
      /* New-style weapon */
      ob->unwield();
      stop_wielding();
    }
    
    /* Beware that drop() might destruct the object. */
    if (!ob-> drop(1) && ob)
      move_object(ob, dest);
    ob = next_ob;
  }
  local_weight = 0;
  if (money == 0)
    return;
  ob = clone_object("obj/money");
  ob-> set_money(money);
  move_object(ob, dest);
  money = 0;
}

static mixed *kill_callbacks;

void add_kill_callback(string fun,object o)
{
  if(!o)
    o=previous_object();
  if(!kill_callbacks)
    kill_callbacks=({ ({ o,fun }) });
  else
    kill_callbacks+=({ ({ o,fun }) });
}

void reset_kill_callbacks() { kill_callbacks = 0; }

/*
 * This function is called from other players when they want to make
 * damage to us. We return how much damage we received, which will
 * change the attackers score. This routine is probably called from
 * heart_beat() from another player.
 * Compare this function to reduce_hit_point(dam).
 */

static mixed hit_callback;
void set_hit_callback(object o,string f) { hit_callback=({o,f}); }
void clear_hit_callback() { hit_callback=0; }

varargs int hit_player(int dam,int ignore_flag)
{
  mixed party,o;
  int old_hp,e;
  mixed tmp;
  int all_shares, * shares;

  /* Fix by profezzorn */
  /* Fixed by Taren, Tue Jul 11*/
  if(this_player() && this_player() !=this_object())
    this_player()->attacked_by(this_object());

/* #ifdef PARANOIA */
  if(ignore_flag)
  {
    log_file("IGNOREFLAG", ctime(time()) + ": " +
	     query_real_name() + " was hit (" + dam + ") by " +
	     (this_player() ?
	      (function_exists("query_real_name", this_player()) ?
	       this_player()->query_real_name() :
	       file_name(this_player())) :
	      "<No this_player()>") + "\n" +
	     (previous_object() ?
	      "   previous_object() = " + file_name(previous_object()) +
	      " (" + previous_object()->query_real_name() + ")" :
	      "   No previos_object()") + "\n");
  }
/* #endif */
  if (!ignore_flag && environment(this_object()) && 
      environment(this_object())->query_property("no_fight")) 
  {
    write("You try to hit " + query_name() + " but reconsider.\n");
    tell_object(this_object(), this_player()->query_name() +
		" tries to hit you, but reconsiders.\n");
    stop_fight();
    this_player()->stop_fight();
    return 0;
  }
  
  if (!ignore_flag) /* Qqqq 95 */
    if (block_hook("block_hit_player_hook",
		   ({dam,this_player(),previous_object()})))
      return 0;
    else {
      tmp = hook("value_hit_player_hook", 
		 ({ dam, this_player(),previous_object() }) );
      if (pointerp(tmp) && sizeof(tmp)) {
        int i;
        for (i=0; i<sizeof(tmp); i++) {
	  if (intp(tmp[i]))
	    dam += tmp[i];
	}
      }
    }

  if(!attacker_ob)
    set_heart_beat(1);
  if (!attacker_ob && this_player() != this_object())
    attacker_ob = this_player();
  else
    if (!alt_attacker_ob && attacker_ob != this_player() &&
	this_player() != this_object())
      alt_attacker_ob = this_player();

  if(ghost)
    return 0;			/* Or someone who is dead */

  dam -= random(armour_class + 1);
  if (dam <= 0)
    return 0;
  if (dam > (hit_point+1))
    dam = (hit_point+1);

  add_hp(-dam,1);

  if (hit_point>=0) 
  {
    hook("hit_player_hook",
	 ({dam,this_player(),previous_object()}));
    if(hit_callback)
      catch(call_other(hit_callback[0],hit_callback[1],dam,previous_object()));
    return dam;
  }
  /* Don't damage wizards too much ! */
  if (level >= 20 && !query_npc())
  {
    hit_point=1;
    tell_object(this_object(), "Your wizardhood protects you from death.\n");
    return 0;
  }


  /*  This is how I think "second_life" should have worked from the start.
   *  If "real_second_life" exists and returns non-zero,
   *  the monster doesn't die at all! -- Padrone, July 6, 1992
   */
  
  old_hp = hit_point;		/* Can be changed in "real_second_life"! */
  if (this_object()-> real_second_life() || 
      block_hook("block_die_hook",
		 ({dam, this_player(), previous_object(), ignore_flag})) ||
      ((this_player() || attacker_ob || alt_attacker_ob || previous_object())->
       block_hook("block_kill_hook",
		  ({dam, this_object(), previous_object(), ignore_flag})))
    )
  {
    dam += old_hp;		/* Subtract excess damage, hp is less than 0 */
    if (hit_point < 0)
      hit_point = 0;
    return dam;
  }
    
  /* We died ! */
  if (query_invis())
    this_object()->vis();
  if (hunter)
    hunter-> stop_hunter();
  hunter = hunted = 0;
  experience = 2 * experience / 3; /* Nice, isn't it? */
  hit_point = 10;
  /* The player killing us will update his alignment! */
  /* If he exist */

#define OLD_XP_SHARING

#ifdef OLD_XP_SHARING
  if(attacker_ob) 
  {
    attacker_ob-> add_alignment(ADJ_ALIGNMENT(alignment));
    
    if(party = present("party object",attacker_ob))
      catch(party->share_exp(experience/50));
    else 
      attacker_ob->add_exp(experience/50);

    catch(all_inventory(attacker_ob) -> 
	  _signal_killed_something(this_object()));
  }
#else
  
  o=all_inventory(environment());
  for(e=0;e<sizeof(o);e++)
  {
    if(o[e] &&
       o[e]!=attacker_ob &&
       o[e]!=alt_attacker_ob &&
       o[e]->query_attack()!=this_object() &&
       o[e]->query_alt_attack() != this_object())
    {
      o[e]=0;
    }
  }
  o-=({0});
  shares=allocate(sizeof(o));
  for(e=0;e<sizeof(shares);e++)
  {
    shares[e]=(o[e]->query_exp() / 1000)+1;
    if(o[e]==attacker_ob) shares[e]*=4/3;
    if(o[e]==alt_attacker_ob) shares[e]*=5/6;
    all_shares+=shares[e];
  }
  for(e=0;e<sizeof(shares);e++)
  {
    o[e]->add_alignment(ADJ_ALIGNMENT(alignment * shares[e] / all_shares));
    o[e]->add_exp(experience / 50 * shares[e] / all_shares);
  }
  if(attacker_ob)
    catch(all_inventory(attacker_ob)->_signal_killed_something(this_object()));
  
#endif
  {
    object corpse;
    dead = 1;
    if(o = present("DEATH_SAVE", this_object()))
    {				/* profezzorn 930617 */
      tmp = file_name(o);       /* Paranoia.. */
      catch(e = o -> save_me_from_death(this_object()));
      if(e)
      {
	dead=0;
	log_file("KILLED", query_real_name()+
		 " died, but was saved by "+tmp+".\n");
	if(o) 
	  destruct(o);
	return dam;
      }
    }
    corpse = clone_object("obj/corpse");
    corpse -> set_name(query_name()||query_real_name());
    corpse -> set_up(this_object(), attacker_ob);
    catch(transfer_all_to(corpse));
    move_object(corpse, environment(this_object()));
    hook("die_hook",
	 ({this_player(), previous_object(), corpse}));
    (this_player() || attacker_ob || alt_attacker_ob || previous_object())->
      hook("kill_hook",
	   ({this_object(), previous_object(), corpse}));
    if(kill_callbacks)
    {
      for(i=0;i<sizeof(kill_callbacks);i++)
	catch(call_other(kill_callbacks[i][0],kill_callbacks[i][1]));
    }

    catch(environment()->living_died(corpse));
    /* I readded this /Taren*/
    catch(all_inventory()->_signal_environment_died(corpse)); 

    /* Two calls to environment? why? /Profezzorn */
    catch(tmp=environment()->death_callback(corpse));
    if(tmp)
      return dam;
    if(previous_object() &&
       previous_object()!=this_object() &&
       previous_object()!=environment())
      catch(previous_object()->kill_callback(corpse));
  
    if(!attacker_ob && interactive(this_object()))
    {
      log_file("KILLED", "BUG!! No attacker_ob, prev_obj = " + 
	       file_name(previous_object()) + " by "+
	       creator(previous_object())+"\n");
    }
      
    log_file("MONSTER_KILL",query_real_name()+
	     " ("+creator(this_object())+") by: "+
	     (this_player()?
	      this_player()->query_real_name()+" ("+
	      this_player()->query_level()+")":"nobody (0)")+
	     " at: "+time()+
	     " in: "+(environment()?file_name(environment()):"--")+
	     "\n");
	       
    if (!(this_object()->second_life())) 
    {
      if(this_player() &&
	 this_player()!=this_object() &&
	 this_player()->query_name())
      {
	say(this_player()->query_name()+" killed "+cap_name+".\n", 
	    this_player());
      }
	  
      say(cap_name + " died.\n"); /* Mats 910903 */
      
      destruct(this_object());
    }
    return dam;
  }
}


/*
 * This routine is called when we are attacked by a player.
 */
void attacked_by(object ob) 
{
  if (!ob) return;

  if (!attacker_ob) 
  {
    attacker_ob = ob;
    set_heart_beat(1);
    return;
  }
  if (!alt_attacker_ob && ob != attacker_ob) 
  {
    alt_attacker_ob = ob;
    return;
  }
}

string query_stats() 
{
  return 
    sprintf("Str: %d\nInt: %d\nCon: %d\nDex: %d\n",
	    query_str(),query_int(),query_con(),query_dex());
}

void show_stats() 
{
   int i;
   string str;

   if(!(str = (string) this_object()->short()))
      str = query_real_name() || "None";
   write(capitalize(str) + 
	 "\nLevel:  " + level);
   if(this_object()->query_paragon())
     write(" (+"+this_object()->query_paragon_level()+")");
   write("\nCoins:  " + money +
	 "\nHp:     " + hit_point + " (max: " + max_hp + ")" +
	 "\nSpell   " + spell_points + " (max: " + max_sp + ")");
   write("\nEp:     " + experience);
   write("\nAc:     " + armour_class);
#ifdef OLD_ARMOUR_SYSTEM
   write("\nArmour: " +
	 (!head_armour ? "None" : (string) head_armour->rec_short()));
#else
   write("\nArmour: " +
	 implode_nicely((string *) ((m_values(worn_armour) - ({0}))->query_name())));
#endif
   write("\nWc:     " + weapon_class);
   write("\nWeapon: " +
	 (!name_of_weapon ? "None" :
	  (string) name_of_weapon->query_name() +
	  (name_of_weapon->query_magic() ? "   [magical]" : "") +
	  "   (/" + file_name(name_of_weapon) + ")"));
   write("\nCarry:  " + local_weight + "/" +
	 ((int) this_object()->query_str() + 10));
   if(attacker_ob)
      write("\nAttack: " + (string) attacker_ob->query_name());
   if(alt_attacker_ob)
      write("\nAlt attack:" + (string) alt_attacker_ob->query_name());
   write("\nAlign:  " + alignment);
   write("\nGender: " + (string) this_object()->query_gender_string() + "\n");
   if(str = (string) this_object()->query_quests())
      write("Quests:   " + str + "\n");
   if(i = (int) this_object()->query_quest_points())
      write("Quest points (currently): " + i + "\n");
   if(i = (int) this_object()->query_my_quest_max())
      write("My_Quest_Max: " + i + "\n");
   if(str = (string) this_object()->query_puzzles())
      write("Puzzles:  " + str + "\n");
   if(i = (status) this_object()->query_playerkilling())
      write("Playerkiller: Yes\n");
   write((string) this_object()->query_stats());
#ifdef USE_MULTS
   write((string) this_object()->query_stat_mults());
#endif
   this_object()->show_age();
   /* Nu ska vi se om man kan haffa lite fuskande personer... */
   if(this_player(1) && 
      (int) this_player(1)->query_level() < 23)
   {
     "/obj/daemon/syslogd"->do_log("STATCHECK: "+
	       (string) this_player(1)->query_real_name() + " [" +
	       (int) this_player(1)->query_level() + "]" +
	       (this_player(1) ?
		" (" + (string) this_player(1)->query_real_name() + ")" : "") +
	       " checked stats on " + (string) this_object()->query_name() +
	       " (" + file_name(this_object()) + ")" +
	       (environment() ? 
		" in '" + file_name(environment()) + "'" : "") + "\n");
   }
}

int stop_wielding() 
{
  if(!name_of_weapon) 
    return 0;  
  name_of_weapon-> un_wield(dead);
  hook("unwield_hook",({weapon_class,name_of_weapon}));
  name_of_weapon = 0;
  weapon_class = 0;
  return 1;
}

#ifndef OLD_ARMOUR_SYSTEM
mapping query_worn() { return worn_armour; }
#endif

int calc_armour_class()
{
  int ac_tmp;
  object o;
#ifdef OLD_ARMOUR_SYSTEM
  if(objectp(head_armour))
  {
    armour_class = head_armour-> tot_ac();
    o = present("guild_mark", this_object());
    if(o && function_exists("affect_armour_class", o))
    {
      ac_tmp = o -> affect_armour_class(armour_class);
      if(ac_tmp < armour_class && ac_tmp >= 0)
	armour_class = ac_tmp;
    }
  } else {
    armour_class = 0;
    head_armour = 0;
  }
#else
  int i, *classes;
  mixed tmp;

  i = sizeof(classes = (m_values(worn_armour)-({0})) -> armour_class());
  /* Patch by Brom 941112. Monsters might have natural levels of armour.
     Players wont. */
  if (query_npc()) {
    armour_class = (int)("obj/daemon/monsterd"->standard_ac(level));
    armour_class *= 3; }
  else
    armour_class = 0;
  /* Patch ended. */

  while(i--)
    armour_class += classes[i];

  /* Added by Taren, 95 Aug 29 */
  tmp = hook("value_wear_hook",
	     ({worn_armour,armour_class}));

  if (pointerp(tmp) && sizeof(tmp)) {
    for (i=0; i<sizeof(tmp); i++) {
      if (intp(tmp[i]))
	armour_class += tmp[i];
    }
  }
  
  armour_class /= 3;
  o = present("guild_mark", this_object());
  if(o && function_exists("affect_armour_class", o))
  {
    ac_tmp = o -> affect_armour_class(armour_class);
    if(ac_tmp < armour_class && ac_tmp >= 0)
      armour_class = ac_tmp;
  }
  return armour_class;
#endif
}

void silent_stop_wearing(string type) 
{
#ifdef OLD_ARMOUR_SYSTEM
  if(!head_armour) {
    /* This should not happen! */
    log_file("wearing_bug", "armour not worn!\n");
    write("This is a bug, no head_armour\n");
    return;
  }
  head_armour = head_armour-> remove_link(type);
#else
  hook("unwear_hook",({type,worn_armour[type]}));
  _m_delete(worn_armour, type);
#endif
  calc_armour_class();
}

void stop_wearing(string name) 
{
#ifdef OLD_ARMOUR_SYSTEM
  if(!query_npc() && !dead)
    say(cap_name +" removes "+ name +".\n");
#else
  name = previous_object() -> query_type() || name;
  if(!query_npc() && !dead && worn_armour[name])
    say(cap_name +" removes "+ query_possessive() +" "+
	(load_object("obj/support") -> _name(worn_armour[name])) +".\n");
#endif
  silent_stop_wearing(name);
}

#if 0
/* functions that return zero just waste memory /Profezzorn */

/* This object is not worth anything in the shop ! */
int query_value() { return 0; }

/* It is never possible to pick up a player ! */
int get() { return 0; }
#endif

/*
 * Return true if there still is a fight.
 */
int attack()
{
  int max;
  string name_of_attacker,dmg_type,*hit_array;
  mixed tmp,whit;
  int tmp_spell_dam, tmp_spell_cost;
  string tmp_spell_name;

  /* Profezzorn, 951130 */
  tmp_spell_dam=spell_dam;
  tmp_spell_cost=spell_cost;
  tmp_spell_name=spell_name;
  spell_name=0;
  spell_cost=spell_dam=0;
  
  if(query_npc() &&
     alt_attacker_ob &&
     present(alt_attacker_ob) &&
     !random(30) &&
     !query_property("no_attack_switches"))
  {
    tmp=attacker_ob;
    attacker_ob=alt_attacker_ob;
    alt_attacker_ob=tmp;
  }

  /* Qqqq, 95 */
  if (!attacker_ob)
    return 0;

  name_of_attacker = attacker_ob -> query_name();  

  if (!name_of_attacker || name_of_attacker == NAME_OF_GHOST ||
      environment(attacker_ob) != environment(this_object())) 
  {
    if (!hunter && name_of_attacker && !attacker_ob-> query_ghost())
    {
      tell_object(this_object(), "You are now hunting "+  
		  attacker_ob-> query_name(0) + ".\n");
      hunted = attacker_ob;
      hunting_time = 10;
    }
    attacker_ob = 0;
    if (!alt_attacker_ob)
      return 0;

    attacker_ob = alt_attacker_ob;
    alt_attacker_ob = 0;
    if (attack() && attacker_ob) 
      return tell_object(this_object(),"You turn to attack "+
			 attacker_ob->query_name() +".\n"), 1;
    return 0;
  }

  if (query_property("peaceful") ||
      block_hook("block_attack_hook",({attacker_ob,tmp_spell_name})))
    return 0;

  /* Qqqq, 95 */
  tmp = hook("value_choose_target_hook",
	     ({attacker_ob,alt_attacker_ob}));  
  if (pointerp(tmp) && (max = sizeof(tmp)))
    while(max--)
      if (objectp(tmp[max]) && living(tmp[max]) &&
	  environment(tmp[max]) == environment(this_object())) {
	attacker_ob = tmp[max];
	break;
      }

  name_of_attacker = attacker_ob -> query_name();

  if (tmp_spell_cost && tmp_spell_name)
  {
    string a,b,c,d;
    spell_points -= tmp_spell_cost;
    /* some general spell stuff by Profezzorn 930923 */
    if(sscanf(tmp_spell_name, "%s\n%s", a, b))
    {
      if(sscanf(b,"%s\n%s",b,c)) sscanf(c,"%s\n%s",c,d);
	
      write(a +"\n");
      if(b) 
	tell_object(attacker_ob, b +"\n");
      if(c) 
	tell_room(environment(), c +"\n", ({this_object(), attacker_ob}));
      if(d)
	catch(d->_spell_callback(a,attacker_ob));
    } else {
      tell_object(attacker_ob, "You are hit by a "+ tmp_spell_name +".\n");
      write("You cast a "+ tmp_spell_name +".\n");
    }
  } else {
    tmp_spell_name = 0;
  }
  if(name_of_weapon)
    whit = name_of_weapon -> hit(attacker_ob);
  if(whit != "miss") {
    tmp = ((weapon_class + whit) * 2 + query_dex()) / 3;
    if (tmp == 0)
      tmp = 1;
    if (attacker_ob && attacker_ob-> query_name() != NAME_OF_GHOST) {
      hook("attack_hook",({attacker_ob,tmp_spell_name}));
      /* Added this in case a monster dies inside a hook - Leowon 951219 */
      if(attacker_ob)
	tmp = attacker_ob-> hit_player(random(tmp) + tmp_spell_dam);
      else
	tmp = 0;
    }
  } else {
    tmp = 0;
  }

  /* This piece of code is back until further notice /Profezzorn */
  tmp -= tmp_spell_dam;
  if (!query_npc())
  {
    if (name_of_weapon && tmp > 20 && random(2) && random(1000) < tmp - 20)
    {

      if(! name_of_weapon->query_property("weapon_can't_break"))
      {
	/* Log the break */
	if (level < 20)
	  log_file("BAD_SWORD",
		   name_of_weapon -> short() + ", " +
		   creator(name_of_weapon) + " (" +
		   file_name(name_of_weapon) + ") (** break ***)\n" );
	tell_object(this_object(), "CRACK!\nYour weapon broke!\n");
	weapon_class = 0;
	destruct(name_of_weapon);
	return 1;
      }
      /* Log it even if it doesn't break */
      if(level < 20)
	log_file("BAD_SWORD", name_of_weapon->short() + ", " +
		 creator(name_of_weapon) + " ("+
		 file_name(name_of_weapon)+")\n");
    } 
  }
  tmp += tmp_spell_dam;
  
  hit_array = 0;
  if(!tmp)
  {
    /* If stop_fight() is called from hit_player(), attacker_ob will now
     * be zero, and then we don't want any hit messages.
     * Brom 950225
     */
    if (!objectp(attacker_ob))
    {
      tell_object(this_object(), "You missed.\n");
      return 1;
    }

    if (name_of_weapon) 
      hit_array = name_of_weapon -> get_hit_lines(0,attacker_ob,this_object());
    
    if (!pointerp(hit_array) || sizeof(hit_array) != 3)
    {
      tell_object(this_object(), "You missed.\n");
      if(attacker_ob)
      {
	tell_object(attacker_ob, cap_name +" missed you.\n"); 
	say(cap_name +" missed "+ name_of_attacker +".\n", attacker_ob);
      } else {
	say(cap_name +" missed "+ name_of_attacker +".\n");
      }

    } else {
      tell_object(this_object(),  hit_array[1]);
      if(attacker_ob)
      {
	tell_object(attacker_ob, hit_array[0]);
	say(hit_array[2], attacker_ob);
      } else 
	say(hit_array[2]);
    }
    return 1;
  }
  if(tmp<0)
  {
    log_file("NEGATIVE_DAMAGE", 
	     sprintf("on: %O by: %s , %s\n", 
		     attacker_ob,query_real_name(),ctime(time())));
  }
  experience += tmp;
  tmp -= tmp_spell_dam;

  /* Does the enemy still live ? */

  if(attacker_ob && attacker_ob-> query_name(0) != NAME_OF_GHOST) {
    string how, what;
    if(name_of_weapon) 
      hit_array = name_of_weapon->get_hit_lines(tmp,attacker_ob,this_object());
    else {
      /* Added damage types in monsters - Leowon 960317 */
      dmg_type = query_property("damage_type");
      if(dmg_type)
	hit_array =
	  load_object("/obj/daemon/messd")->get_hit_lines(tmp,
							  attacker_ob,
							  this_object(),
							  dmg_type);
    }
    if(pointerp(hit_array) && sizeof(hit_array) == 3) {
      tell_object(attacker_ob,hit_array[0]);
      tell_object(this_object(),hit_array[1]);
      say(hit_array[2], attacker_ob);
      return 1;
    }
    
    switch(tmp)
    {
    case -1000..2: /* Was 1 only, it might have been a bug /Taren 951117 */
      how = " in the stomach";
      what = "tickled";
      break;
    case 3..4 :
      how = "";
      what = "grazed";
      break;
    case 5..9 :
      how = "";
      what = "hit";
      break;
    case 10..14 :
      how = " hard";
      what = "hit";
      break;
    case 15..19 :
      how = " very hard";
      what = "hit";
      break;
    case 20..29 :
      how = " with a bone crushing sound";
      what = "smashed";
      break;
    default :
      how = " to small fragments";
      what = "massacred";
    }
    tell_object(this_object(), "You " + what + " " + name_of_attacker + 
		how + ".\n");
    tell_object(attacker_ob, cap_name + " " + what + " you" + how +
		".\n");
    say(cap_name + " " + what + " " + name_of_attacker + how +
	".\n", attacker_ob);
    return 1;
  }
  if (objectp(this_object()))
    tell_object(this_object(), "You killed " + name_of_attacker + ".\n");

  attacker_ob = alt_attacker_ob;
  alt_attacker_ob = 0;
  if (attacker_ob)
    return 1;
}

object query_attack() { return attacker_ob; }
object query_alt_attack() { return alt_attacker_ob; }

/* Grep, if not found, inline */
int drop_money(int howmuch,int silent) 
{
  object mon;
  if (howmuch < 0)
  {
    if (!silent)
    {
      tell_object(this_object(), "You don't have negative coins.\n");
      return 0;
    }
  }

  if (money < howmuch)
  {
    if (!silent)
    {
      tell_object(this_object(), "You don't have that much money.\n");
      return 0;
    }
  }

  mon = clone_object("obj/money");
  mon -> set_money(howmuch);
  mon -> set_volatile();
  move_object(mon, environment());
  if (!silent)
  {
    say(cap_name + " drops "+(howmuch > 1 ? howmuch +"gold coins" : 
			      "a gold coin") +".\n");
    tell_object(this_object(),  "You drop "+
		(howmuch == money ? "all your money" :
		 (howmuch > 1 ? howmuch+" gold coins" : "a gold coin"))+".\n");
  }
  this_object()->add_money(-howmuch);
  return 1;
}

/* Grep, if not found, inline */
void drop_all_money(int verbose) { drop_money(money, !verbose); }

int silent_wield(object w)
{
  object o;
  int wc,wc_tmp;
  mixed tmp;
  string str;

  /*Qqqq 930804 Added cursed ability on weapons*/
  if(name_of_weapon && (str = (string)name_of_weapon -> 
			query_property("cursed"))) 
    return write(stringp(str) ? str :
		 "You can't unwield it, it seems to be cursed.\n"), 0;
  wc = ((int)w -> query_class() || (int)w -> weapon_class());
  if (!wc)
    return 0;

  if (block_hook("block_wield_hook", /*Added by Taren, 95 Aug 29*/
		 ({wc,w})))
    return 1;
  
  if (w -> query_weapon() && !w -> wield())
      return 0;

  if (name_of_weapon)
    stop_wielding();
  
  w -> set_wielded_by(this_object());
  weapon_class = wc;
  name_of_weapon = w;
  
  /* wield hook added by Taren, 95 Aug 29 */
  tmp = hook("value_wield_hook",({wc,w}));  
  if (pointerp(tmp) && sizeof(tmp)) {
    int i;
    for (i=0; i<sizeof(tmp); i++) {
      if (intp(tmp[i]))
	weapon_class += tmp[i];
    }
  }
  o = present("guild_mark", this_object());
  if(o && function_exists("affect_weapon_class", o)) 
  {
    wc_tmp = (int)o -> affect_weapon_class(weapon_class);
    if(wc_tmp < weapon_class && wc_tmp >= 0)
      weapon_class = wc_tmp;
  }
  hook("wield_hook",({weapon_class,w,wc}));   
  return 1;
}

/* Wield a weapon. */
int wield(object w) 
{
  int i;
  if(!(i = silent_wield(w)))
    write("You cannot wield that!\n");
  else
    write("Ok.\n");
  return i;
}

object *query_wielded() { return name_of_weapon ? ({ name_of_weapon }) : 0; }
object *query_wield() { return query_wielded(); }

mapping wear_callbacks = ([]);
void add_wear_callback()
{
  wear_callbacks[previous_object()]=1;
}

void remove_wear_callback()
{
  _m_delete(wear_callbacks, previous_object());
}

/* Wear some armour. */
mixed silent_wear(object a) 
{
#ifdef OLD_ARMOUR_SYSTEM
  object old;
  
  if(head_armour) {
    old = head_armour->test_type(a->query_type());
    if(old)
      return old;
    old = head_armour;
    a-> link(old);
  }
  head_armour = a;
#else
  string type;
  type=a->query_type();
  if(m_sizeof(wear_callbacks))
  {
    if(sizeof(m_indices(wear_callbacks)->wear_callback(type, a)- ({ 0 }) ))
      return 1;
  }

  if (block_hook("block_wear_hook", /*Added by Taren, 95 Aug 29*/
		 ({type,a})))
    return 1;

  hook("wear_hook",({type,a})); 
  
  if(worn_armour[type]) return worn_armour[type];
  worn_armour[type] = a;
#endif
  calc_armour_class();
  return 0;
}

mixed wear(object a) 
{
  object old;
  
  if(old = silent_wear(a)) 
    return old;
#ifdef OLD_ARMOUR_SYSTEM
  say(cap_name + " wears " + a-> query_name() + ".\n");
#else
  say(cap_name +" wears "+ 
      (load_object("obj/support") -> a_name(a)) +".\n");
#endif
  write("Ok.\n");
  return 0;
}

#ifdef OLD_ARMOUR_SYSTEM
object query_head_armour() { return head_armour; }
#else
object query_head_armour() 
{ 
  log_file("FIX", "query_head_armour: "+file_name(previous_object())+"\n"); 
  return m_sizeof(worn_armour) ? m_values(worn_armour)[0] : 0;
}
#endif

/* This grotesque hack by Brom 951213 (and I am sober).
 * Added the test flag to it, then it will be possible to
 * easily test if we can carry.
 */
varargs int add_weight(int w,int test) 
{
  if (test==1)
    return !((w + local_weight) > (query_str() + 10) && level < 20);
  if (w + local_weight > query_str() + 10 && level < 20)
    return 0;
  local_weight += w;
  return 1;
}

/* Ugh!! this should be changed / Profezzorn */
int query_weight() { return local_weight; }

/*
* A lot uses this, too much to fix right now...
* log_file("FIX", "restore_spell_points: "+file_name(previous_object())+"\n"); 
* Grep, change to reduce_spell_points and remove please
*
* New, designed functions to change HP/SP.
* Brom 951016.
* Names are short and to the point, checks are legio but can be
* blocked by using a flag.
*/

varargs int add_hp(int hps, int ignore)
{

  if(!ignore)
    if(block_hook("block_add_hp_hook",({hps,hit_point,previous_object(),ignore})))
      return hit_point;
  
  hook("add_hp_hook",({hps,hit_point,previous_object(),ignore}));

  
  hit_point += hps;

  if (!ignore)
  {
    if (hit_point < 1) hit_point = 1;
    if (hit_point > max_hp) hit_point = max_hp;
  }
/* This logg commented since the logg file grew with 300k in 15 min...
   It must be done differently.
   Also look at add_sp().
   Brom 960108.

  else
  {
    if (this_player(1) &&
	this_player(1) != this_object() &&
	this_object() != previous_object() )
      "obj/daemon/syslogd" -> do_log("ADD_HP : " +hps+" : "+hit_point+
				     this_player() -> query_real_name() +
				     " on " + 
				     this_object() -> query_real_name() +
				     " (" + query_verb() + ")\n");
  }
*/

  if(hit_callback)
    {
      log_file("FIX","hit_callback "+file_name(hit_callback[0])+" "+file_name(previous_object())+"\n");
      catch(call_other(hit_callback[0],hit_callback[1],hps,previous_object()));
    }
  return hit_point;
} /* add_hp */

varargs int add_sp(int sps, int ignore)
{
  if(!ignore)
    if(block_hook("block_add_sp_hook",({sps,spell_points,previous_object(),ignore})))
      return spell_points;
  hook("add_sp_hook",({sps,spell_points,previous_object(),ignore}));
  
  spell_points += sps;
  if (!ignore)
  {  
    if (spell_points > max_sp)
      spell_points = max_sp;
  }
/* Commented out, same reasons as add_hp()
  else
  {
    if (this_player(1) &&
	this_player(1) != this_object() &&
	this_object() != previous_object() )
      "obj/daemon/syslogd" -> do_log("ADD_SP : " +sps+" : "+spell_points+
				     this_player() -> query_real_name() +
				     " on " + 
				     this_object() -> query_real_name() +
				     " (" + query_verb() + ")\n");
  }
*/
  return spell_points;
} /* add_sp */

void heal_self(int h) 
{
  if (h <= 0) return;
  add_hp(h);
  add_sp(h);
#ifdef HLD
  HLD -> logs(this_player(), this_player(1), this_object(), previous_object(), h, 2);
#endif
}

/* In the long term, these should all be remoevd /Profezzorn */
int restore_spell_points(int h) { return add_sp(h); }
int reduce_spell_points(int h)  { return add_sp(-h); }
int reduce_hit_point(int dam) { return add_hp(-dam); }

/******************************************/

int can_put_and_get(mixed str)
{
  if(ghost) return 0;
  /* hmmm /Profezzorn */
  return str != 0;
}

void attack_object(object ob)
{
  if (ob-> query_ghost() || environment()->query_property("no_fight"))
    return;
  set_heart_beat(1);		/* For monsters, start the heart beat */
  if (attacker_ob == ob) {
    attack();
    return;
  }
  if (alt_attacker_ob == ob) {
    alt_attacker_ob = attacker_ob;
    attacker_ob = ob;
    attack();
    return;
  }
  if (!alt_attacker_ob)
    alt_attacker_ob = attacker_ob;
  attacker_ob = ob;
  attacker_ob -> attacked_by(this_object());
  attack();
}


/* Ugh, what is this doing in living??? This should be moved to
 * player.c!!!
 */
void zap_object(object ob)
{
#ifdef PARANOIA
  log_file("ZAP",sprintf("%s zapped %O, %s\n",query_real_name(),ob,ctime(time())));
#endif
  ob-> attacked_by(this_object());
  say(cap_name + " summons a flash from the sky.\n");
  write("You summon a flash from the sky.\n");
  experience += ob-> hit_player(100000);
  write("There is a big clap of thunder.\n\n");
}

/* Grep for these three and replace with do_spell_object */
void missile_object(object ob)
{
  if (spell_points < 10) {
    write("Too low on power.\n");
    return;
  }
  spell_name = "magic missile";
  spell_dam = random(20);
  spell_cost = 10;
  attacker_ob = ob;
}

void shock_object(object ob)
{
  if (spell_points < 15) {
    write("Too low on power.\n");
    return;
  }
  spell_name = "shock";
  spell_dam = random(30);
  spell_cost = 15;
  attacker_ob = ob;
}

void fire_ball_object(object ob)
{
  if (spell_points < 20) {
    write("Too low on power.\n");
    return;
  }
  spell_name = "fire ball";
  spell_dam = random(40);
  spell_cost = 20;
  attacker_ob = ob;
}

int do_spell_object(int cost,int dam,string name,object ob,int xdam)
{
  if (spell_points < cost)
  {
    write("Too low on power.\n");
    return 0;
  }
  if (!ob) ob = attacker_ob;
  if (!ob) ob = alt_attacker_ob;
  if(!ob) return 0;
  set_heart_beat(1);
  spell_name = name;
  spell_dam = random(dam) + xdam;
  spell_cost = cost;
  if (attacker_ob != ob)
  {
    alt_attacker_ob = attacker_ob;
    attacker_ob = ob;
  }
  return 1;
}

/*
* If no one is here (except ourself), then turn off the heart beat.
*/
  
int test_if_any_here()
{
  object ob;
  ob = environment();
  if (!ob)
    return 0;
  ob = first_inventory(environment());
  while(ob) {
    if (ob != this_object() && living(ob) && !ob-> query_npc())
      return 1;
    ob = next_inventory(ob);
  }
  return 0;
}

void show_age() {
  int i,t;
  
  write("Age:\t");
  i = age;
  if (t=i/43200) {
    write(t + " day"+(t==1?" ":"s "));
    i %= 43200;
  }
  if (t=i/1800) {
    write(t + " hour"+(t==1?" ":"s "));
    i %= 1800;
  }
  if (t=i/30) {
    write(t + " minute"+(t==1?" ":"s "));
    i %= 30;
  }
  write(i*2 + " seconds.\n");
}

void stop_hunter()
{
  if (hunter != 0)
  {
    hunter = 0;
    tell_object(this_object(), "You are no longer hunted.\n");
  }
}

/*
* This function remains only because of compatibility, as command() now
* can be called with an object as argument.
*/
int force_us(string cmd)
{
  if (!this_player(1) || this_player(1)->query_level() <= level ||
      query_ip_number(this_player(1)) == 0)
  {
    tell_object(this_object(), this_player()->query_name() +
		" failed to force you to " + cmd + "\n");
    return 0;
  }
  return command(cmd);
}

/* This is used by the shop etc. */
void add_money(int m) 
{
  if (!intp(m)) return;
  money += m;
}

varargs string run_away(int silently) 
{
  /* out and exit added by Pell 910627 */
  /* ne se sw nw added by Mats 921124 */
  /* warray made local variable by Profezzorn 930514 */
  /* Usage of dest_dirs if avaible by Slater */
  /* Random on dest_dirs by Slater 940406 */
  /* Optimized by Profezzorn 950601 */
  /* Hookified by Qqqq 960203 */
  mixed exits,exits2;
  int nr, i,max;
  mixed dest;
  object here;

  here = environment();
  if (block_hook("block_run_away_hook",
		 ({here,previous_object(),silently})))
    return 0;

  exits = WARRAY;

  if(mappingp(exits2=here->query_exits()))
  {
    exits+=m_indices(exits2);
    exits = exits & exits;
  }else{
    exits2=here->query_dest_dir();
    if(pointerp(exits2) && (nr=sizeof(exits2)) && !(nr & 1))
    {
      i=sizeof(exits);
      exits+=allocate(sizeof(exits2)/2);
      while(nr) {
	if(pointerp(exits[i++]=exits2[--nr]))
	  exits[i-1]=exits[i-1][0];
	nr--; /*Fixed by Taren, 950711*/
      }
      exits = exits & exits;
    }
  }

  nr=sizeof(exits);
  do{
    dest=exits[i=random(nr)];
    command(dest);
    exits[i]=exits[--nr];
  }while(nr && environment() == here);

  if(!silently)
  {
    if (here == environment())
    {
      say(cap_name + " tried, but failed to run away.\n", this_object());
      tell_object(this_object(),
		  "Your legs tried to run away, but failed.\n");
    } else {
      tell_object(this_object(), "Your legs run "+dest+" with you!\n");
    }
  }
  
  hook("run_away_hook",
       ({here,previous_object(),silently}));
  return dest;
}

void stop_fight() {
  attacker_ob = alt_attacker_ob;
  alt_attacker_ob = 0;
}

void set_str(int i) 
{
  if (i < 1 || i > 20) return;
  Str = i;
}

void set_int(int i) 
{
  if (i < 1 || i > 20)  return;
  Int = i;
  if(this_object()->query_super_player()) {
    max_sp = 42 + query_int() * 8 + (this_object()->query_super_level())*2;
    if(max_sp>300) max_sp=300;
  }
  max_sp = 42 + query_int() * 8;
}

void set_con(int i) 
{
  if (i < 1  || i > 20) return;
  Con = i;
  if(this_object()->query_super_player()) {
    max_hp = 42 + query_con() * 8 + (this_object()->query_super_level())*2;
    if(max_hp>300) max_hp=300;	
  }
  max_hp = 42 + query_con() * 8;
}

void set_dex(int i) 
{
  if (i < 1 || i > 20) return;
  Dex = i;
}

/* Experimental support for "follow" */

#ifdef USE_FOLLOW

static int is_follower(object ob)
{
  int i;
  
  if (follow_list)
    for (i = 0; i < sizeof(follow_list); i++)
      if (follow_list[i] == ob)
	return i;
  
  return -1;
}

object *query_followers() { return follow_list; }

/* If argument 'ob' exists, check if that object is following us
 * else return the object we are following
 */
varargs mixed query_follow(object ob)
{
  if (!ob)
    return follow_ob;
  
  if (is_follower(ob) != -1)
    return ob;
  
  return 0;
}


/* If argument 'ob' exists, follow that object
 * else register caller as a follower to us
 */
varargs object start_follow(object ob) 
{
  int i;
  object ob2;
  
  
  if (ob)
  {
    /* Get serious, we can't follow ourself, can we? */
    if (ob == this_object())
      return 0;
    
    /* Check for follow loops */
    ob2 = ob;
    while ((ob2 = ob2->query_follow()) && ob2 != this_object())
      ;
    if (ob2)
      return 0;
    
    if (ob->start_follow() == 0)
      return 0;
    
    follow_ob = ob;
    return ob;
  }
  else
  {
    ob = previous_object();
    
    /* Get serious, we can't follow ourself, can we? */
    if (!ob || ob == this_object() ||
	(interactive(ob) && ob->query_level()<20))
      return 0;
    
    if (is_follower(ob) != -1)
      return 0;
    
    if (!follow_list)
      follow_list = allocate(MAX_FOLLOWERS);
    
    for (i = 0; i < sizeof(follow_list); i++)
      if (follow_list[i] == 0)
      {
	follow_list[i] = ob;
	return ob;
      }
  }
  return 0;
}


/* If argument 'ob' exists, stop it from following us
 * else stop following someone
 */
object stop_follow(object ob)
{
  int i;
  
  if (ob)
  {
    /* Remove it from our follower-list */
    i = is_follower(ob);
    if (i == -1)
      return 0;
    follow_list[i] = 0;
    
    /* Is it following us? */
    if (ob->query_follow() != this_object())
      return 0;
    
    /* Make it stop following us */
    if (ob->stop_follow() == 0)
      return 0;
    
    return ob;
  }
  else
  {
    /* Stop following someone */
    if (!follow_ob)
      return 0;
    
    ob = follow_ob;
    follow_ob = 0;
    
    /* Are we in it's follow-list? */
    if (ob->query_follow(this_object()) == 0)
      return 0;
    
    /* Remove us from it's follow-list */
    if (ob->stop_follow(this_object()) == 0)
      return 0;
    
    return ob;
  }
  return 0;
}

/* Make sure only players/NPCs in the same room are in the follow list */
static int follow_update()
{
  int i, c;
  
  
  c = 0;
  if (follow_list)
    for (i = 0; i < sizeof(follow_list); i++)
    {
      if (follow_list[i])
      {
	/* Follower isn't in the same room */
	if (environment(follow_list[i]) != environment(this_object()))
	  stop_follow(follow_list[i]);
	
	/* Follower isn't following us! Strange.. */
	else if (follow_list[i]->query_follow() != this_object())
	  stop_follow(follow_list[i]);
	
	else
	  c++;
      }
    }
  
  if (c == 0)
    follow_list = 0;
  
  return c;
}



/* Make the players/NPCs follow me */
static int do_follow(string dir_dest,mixed optional_dest_ob,object dest)
{
  int i, c;
  
  
  c = 0;
  if (follow_list)
    for (i = 0; i < sizeof(follow_list); i++)
      if (follow_list[i])
	if (dest->query_prevent_follow(follow_list[i]))
	  follow_list[i]->stop_follow();
	else
	{
	  follow_list[i]->follow_player(dir_dest, optional_dest_ob);
	  c++;
	}
  
  return c;
}


/* Handle remote follow trigger */
int follow_player(string dir_dest,mixed optional_dest_ob)
{
  /* Only the object we are following may call this function */
  if (query_follow() != previous_object())
    return 0;
  
  tell_object(this_object(), "You follow " +
	      capitalize(previous_object()->query_real_name()) +
	      ".\n");
  return real_move_player(dir_dest, optional_dest_ob);
}


#endif

#ifdef USE_WETCODE

int wet_time;

int query_wet() { return wet_time; }

varargs int set_wet(int time)
{
  if (!time)
    time = 360;
  
  if (wet_time == 0 && interactive(this_object()))
    call_out("say_wet", 1, time);
  
  set_heart_beat(1);
  wet_time = time;
  all_inventory(this_object()) -> set_wet(time);
  return 1;
}

void say_wet(int time)
{
  switch(time)
  {
  case 0 .. 200 :
    tell_object(this_object(), "You get rather wet.\n");
    break;
  case 201 .. 400 :
    tell_object(this_object(), "You get wet.\n");
    break;
  case 401 .. 600 :
    tell_object(this_object(), "You get wet all over.\n");
    break;
  case 601 .. 1000 :
    tell_object(this_object(), "You get soaking wet.\n");
    break;
  default :
    tell_object(this_object(), "Suddenly, you are very wet!\n");
  }
} /* say_wet */


void reset_wet()
{
  wet_time = 0;
  tell_object(this_object(), "You are dry again.\n");
}

#endif

status clean_up(int arg)
{
  if(environment()) return 0;
  while(first_inventory()) destruct(first_inventory());
  if(_isclone(this_object()) ||
     (arg<2 && this_object()==_next_clone(this_object())))
    destruct(this_object());
  return 1;
}

status e_comm()
{
  if(!previous_object() || 
     file_name(previous_object()) != "open/commander/rdesc")
    return 0;
  enable_commands();
  return 1;
}
