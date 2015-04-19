/*********************************************************
 *                                                       *
 *   OBS!!!  All changes should be added to  ChangeLog   *
 *           M-x add-change-log-entry                    *
 *                                                       *
 *********************************************************/

inherit "/obj/env_var";
inherit "/obj/simplemore";
inherit "/obj/new_wiz_line";
inherit "/players/milamber/metaboard"; /* The 'boards' command */
inherit "/obj/support";
inherit "/obj/password";


static object wiztool;
static string away_string;

string auto_load;	/* Special automatically loaded objects. */

#define MAX_IDLE      30*60  /* 30 minutes */

#define MAX_PLAYERS   50
#define MAX_ARCHWIZ   100    /* Infinite nr of arches ... */
#define MAX_WIZARDS   20

#define CURRENT_DATE  ctime(time())[4..15]
#define TICK_STATUE   15*60  /* 15 minutes per statue "tick" */
#define MAX_STATUE    4      /* 1 hour */
/* #define HLD "obj/daemon/hld" */ /* A log-daemon for $ and heals. */
#define LOG_ATTEND           /* Log attendence. */
/* #define NO_LVL_1_SHOUT  */
static string statue_type;

#define USE_WETCODE   /* Experimental "wet" code */
/* #define USE_FOLLOW    / Experimental "follow" code, see living.c also */

/* #define OLD_ARMOUR_SYSTEM   
   Undefine this to use the 'linked list' armour system instead of a mapping.
   */
#undef OLD_ARMOUR_SYSTEM


/* #define PARANOIA */

public mapping toggles; /* The mapping with all toggles */

#include <log.h>
#include <living.h>
#include <var.h>
#include <tune.h>   /* Milamber */

#define WIZ 1
#define ARCH 0

int term_rows;
int term_cols;
string term_type;

static status is_linkdead;

int tot_value;          
static int nologin;

string title;		/* Our official title. Wiz's can change it. */
string password;	/* This players crypted password. */
static string password2;/* Temporary when setting new password */
string al_title;
int intoxicated;	/* How many ticks to stay intoxicated. */
int stuffed;            /* How many ticks to stay stuffed */
int soaked;             /* How many ticks to stay soaked */
int headache, max_headache;
static int old_hp, old_sp; /* Used by hpinfo */
string called_from_ip;	/* IP number was used last time */
string called_from_host;/* Hostname was used last time */
string called_from_user;/* Username was used last time */
string last_login_time; /* When we last did login */
int last_login_time_int; /* When did we last login in secs */
int last_logout_time_int; /* When did we last logout in secs */
static string ident_user;      /* Username on localhost  */
string allow_from_host; /* Hosts to be able to login from */
static string rlogin_name;
int use_rlogin;         /* flag to set if rlogind should log us in */
string plan_desc;       /* A plan to the finger command */
string quests;		/* A list of all solved quests */
int my_quest_max;	/* The max number of quest points when this player was created! */
string puzzles;		/* A list of all solved puzzles */
status playerkilling;	/* Is this player allowed to kill other players? */
static int time_to_save;/* Time to autosave. */
status fake_ghost;      /* Special for Ramses guild..sigh..  */
string testchar;        /* Are we a testchar and in that case whose? */
string secondchar;      /* Are we a wizards second char? */
static string saved_where;   /* Temp... */
string mailaddr;        /* Email address of player */
string wwwaddr;         /* the http address of the player*/
static string host;      /* Name of players remote host */
static string xx_realname; /* Used to prevent some guys from cheating.. /pen */

status view_news;       /* Used at login to view news or not.  /Guildmaster */
int last_newsread;
static int busy;
public string last_tell_from;
static int time_to_count_weight = 20;

int paragon;         /* 0 means not a paragon all other is */
int paragon_level;   /* level of paragon, 19+(paragon_level) */

string prompt = "> ";

slow_heart_beat()
{
  int heal;
  hook("slow_beat_hook",({attacker_ob}));  

  remove_call_out("slow_heart_beat");
  call_out("slow_heart_beat", INTERVAL_BETWEEN_HEALING * 2);
  
  if(!attacker_ob && !alt_attacker_ob)
  {
    remove_call_out("restart_dah_heart_beat");
    set_heart_beat(0);
  }
    
  if (!--hunting_time)
  {
    if (hunter) 
      hunter->stop_hunter();
    hunted = hunter = 0;
  }

  if(!interactive(this_object()))
  {
    set_heart_beat(0);
    if (!is_linkdead)
    {
      if (!ghost && !query_invis())
	say("Suddenly, " + capitalize(name) + " turns into stone!\n");
      disable_commands();
      is_linkdead = 1;
      if (environment(this_object()))
	save_me(1);
      time_to_save = age + 500;
      statue_type = "a nice";
      call_out("remove_statue", TICK_STATUE, 0);
      catch("/obj/daemon/whod"->register_logout());
#ifdef LOG_ATTEND
      catch(write_file("log/ATTEND",
		       sprintf("%d - %-11s %d\n",
			       time(), name, sizeof(users() - ({ 0 }) ))));
#endif
    }
    remove_call_out("restart_dah_heart_beat");
    all_inventory(this_object())->_signal_lost_connection();
    hook("connection_hook",({"lost"}));
    return;
  } else  {
    if (is_linkdead)
    {
      if (!ghost && !query_invis())
	say(capitalize(name) + " turns alive again!\n");
      is_linkdead = 0;
      remove_call_out("remove_statue");
    }
  }
  
  if (level < 22 && query_idle(this_object()) >= MAX_IDLE - 
      INTERVAL_BETWEEN_HEALING)
    /*Changed by Qqqq*/
  {
    if(query_idle(this_object()) > MAX_IDLE)
    {
      tell_object(this_object(), "** Maximum idle time reached.\n");
      _disconnect(this_object());
    } else {
      tell_object(this_object(), "** Maximum idle time soon reached.\n");
    }
  }
  age += INTERVAL_BETWEEN_HEALING;

  if (age > time_to_save) 
  {
    if (!toggles["abrief"])
      write("Autosave.\n");
    if(environment(this_object())) 
      save_me(1); /* Angmar */
  }

  if (level > 19 || ghost)
    return;

  if (wet_time > 0 && environment() &&
      !environment()->query_property("waterfilled"))
  {
    wet_time -= INTERVAL_BETWEEN_HEALING;
    if(wet_time < 0)
      wet_time = 0;
    if (!random(4))
    {
      say("Some water drips from " + cap_name + ".\n");
      write("Some water drips from you.\n");
    }
  }
  if (intoxicated && !random(4)) 
  {
    int n;
    switch(random(7))
    {
     case 0:
      say(cap_name + " hiccups.\n");
      write("You hiccup.\n");
      break;
     case 2:
      say(cap_name + " seems to fall, but takes a step and recovers.\n");
      write("You stumble.\n");
      break;
     case 4:
      write("You feel drunk.\n");
      say(cap_name + " looks drunk.\n");
      break;
     case 6:
      say(cap_name + " burps.\n");
      write("You burp.\n");
    }
  }  
  
  /* No obvious effects of being stuffed or soaked */
  
  if (hit_point < max_hp || spell_points < max_sp || intoxicated || headache)
  {
    if (headache) 
    {
      headache--;
      if (headache <= 0)
	tell_object(this_object(), "You no longer have a headache.\n");
    }
    /* You don't heal if:
     * you are evil or worse, and stand on holy_ground, or
     * you are good or better and stand on unholy_ground.
     */
    if (environment(this_object()) 
	&& (((this_object() -> query_alignment() < -KILL_NEUTRAL_ALIGNMENT*100+1) &&
	     environment(this_object()) -> query_property("holy_ground")) ||
	    ((this_object() -> query_alignment() > KILL_NEUTRAL_ALIGNMENT*100) &&
	     environment(this_object()) -> query_property("unholy_ground")))
	)
      tell_object(this_object(),
		  "You feel uncomfortable here.\n");
    else
    {
      /*
	 
      if (hit_point < max_hp) 
      {
	
	add_hp(1);
	if (intoxicated)
	  add_hp(3);
	if (hit_point > max_hp)
	  hit_point = max_hp;
      }
      if (spell_points < max_sp) 
      {
	add_sp(1);
	if (intoxicated)
	  add_sp(3);
	if (spell_points > max_sp)
	  spell_points = max_sp;
      }
      */
      
      if(!this_object()->query_property("disable_slow_healing"))
      {
	heal=1;
	if(intoxicated)
	  heal+=3;
	add_hp(heal,0,1);
	add_sp(heal);
      }
    }
    if (intoxicated) 
    {
      intoxicated--;
      if (intoxicated <= 0) 
      {
	headache = max_headache;
	max_headache = 0;
	tell_object(this_object(),"You suddenly without reason"+
		    " get a bad headache.\n");
	add_hp(-3);
	if (hit_point < 0)
	  hit_point = 0;
      }
    }
  }
  
  if(stuffed && (stuffed -= INTERVAL_BETWEEN_HEALING) < 0)
    stuffed = 0;
  
  if(soaked && (soaked -= INTERVAL_BETWEEN_HEALING) < 0)
    soaked = 0;
  show_hpinfo();
  if(!time_to_count_weight--)
  {
    count_weight();
    time_to_count_weight = 20;
  }
}

review() 
{
  write("mout:\t"  + cap_name+" "+ msgout +"."+
	"\nmin:\t" + cap_name+" "+msgin +"."+
	"\nmmout:\t" + cap_name+" " +mmsgout +"."+
	"\nmmin:\t" + cap_name+" "+mmsgin + ".\n");
  return 1;
}


status query_prevent_shadow(object ob)
{
  return !sscanf(file_name(ob), "local/%*s");
}

void _exit()
{
  catch("/obj/daemon/whod"->register_logout());
#ifdef LOG_ATTEND
  if (   interactive(this_object())
      && name
      && name != "logon"
      && name != "guest")
    catch(write_file("log/ATTEND",
		     sprintf("%d - %-11s %d\n",
			     time(), name, sizeof(users() - ({ 0 }) ))));
#endif
}

string query_real_name() { return name; }

static int filter_users_fun(object ob, int level)
{
  if (level > 0)
    return (ob && (int)ob->query_level() >= level);
  else
    return (ob && (int)ob->query_level() <= -level);
}

mixed *filter_users(int level)
{
  return filter_array(users(), "filter_users_fun", this_object(), level);
}


/* logon() is called when the players log on. */
static logon()
{
  int st;

  time_to_save = 500;
  enable_commands();
  catch(st=load_object("/obj/daemon/entryd")->enter());
  if(!st)
    write(replace(read_file("etc/LOGO"), "$$$$$$$$$$$$$$$",
		  sprintf("%- 15s", version())));

  
  if(sizeof(users()) >= MAX_PLAYERS + MAX_ARCHWIZ + MAX_WIZARDS)
  {
    write("*** NannyMUD is full. Please come back later.\n");
    destruct(this_object());
    return;
  }

  
  if(nologin = cat("/etc/NOLOGIN"))
    write("*** Login limited to Arch Wizards ***\n");    
  else
    cat("/etc/WELCOME");

  
  if(name == "logon")
  {
    write("What is your name: ");
    input_to("logon2");
    call_out("time_out", 120);
    return 1;
  }
}



static object other_copy;

void fiddle_obj(object ob)
{
  /* Now you automatically wear/wield objects when returning
     from being linkdead - Slater 940531 
     */

  /*
   * Don't move the soul.
   */
  if (!ob -> query_prevent_move_from_statue())
  {
    mixed w, a;
    if (ob == name_of_weapon && ob->query_weapon())
    {
      ob -> unwield();
      stop_wielding();
    }
    w = (mixed)ob -> query_wielded();
#ifdef OLD_ARMOUR_SYSTEM
    a = (object)ob -> query_worn();
#else
    if( ob -> armour_class () )
      ob -> do_remove();
#endif
    ob -> drop();
    
    if (ob)
    {
      move_object( ob, this_player() );
      if(w) wield( ob );
      if(a) ob->do_wear( );
    }
  }
}

nomask go_active()
{
  int e;
  object *o;
  if(previous_object() &&
     sscanf(file_name(previous_object()),"obj/player#%*s"))
  {
    if(is_linkdead)
    {
      is_linkdead=0;
      clear_cached_quest_points();
      enable_commands();
      slow_heart_beat();
      add_standard_commands();
    
      if (level >= 20) wiz_setup();

      /* Run _backwards_ !!! */
      o=all_inventory(this_player());
      for(e=sizeof(o)-1;e>=0;e--) o[e]->init();

      if(environment())
      {
	environment()->init();
	o=(all_inventory(environment())-({this_player()}));
	for(e=sizeof(o)-1;e>=0;e--) o[e]->init();
      }
      remove_call_out("remove_statue");
    }
    catch("/obj/daemon/whod"->register_player());
    hook("connection_hook",({"regain"}));
    
    say(capitalize(name) + " enters the game.\n");
    update_prompt(); /* This probably doesn't work (because of this_player(1) != this_player()..... hmmm */
    look();
  }
}

nomask static try_throw_out(str)
{
#ifndef OLD_ARMOUR_SYSTEM
  int i;
  object *arm;
#endif
  object ob;
  string err;

  /* This should login people if the statue is gone after we answer 'yes' */
  if(!other_copy)
  {
    move_player_to_start3();
    return;
  }

  enable_commands();
  all_inventory()->_signal_throw_out();


  if (str == "" || (str[0] != 'y' && str[0] != 'Y')) 
  {
    write("Welcome another time then!\n");
    destruct(this_object());
    return;
  }

  /* Profezzorn (fiddle obj and the stuff below this if statement
   * will be removed later
   */
  if(other_copy &&
     sscanf(file_name(other_copy),"obj/player#%*s"))
  {
    if(interactive(other_copy)) _disconnect(other_copy);
    update_prompt(); /* let's see if this works */
    exec(other_copy,this_object());
    other_copy->go_active();
    destruct(this_object());
    return;
  }

#ifndef OLD_ARMOUR_SYSTEM
  /* Move armour from old object */
  i = sizeof(arm = m_values(worn_armour = (other_copy -> query_worn())));
  armour_class = other_copy -> query_ac();

  while(i--)
  {
    if(arm[i])
    {
      if(err=catch(move_object(arm[i], this_object())))
	_syslog("Couldn't move armour "+file_name(arm[i])+" "+err+".\n");
    }
  }
#endif


  ob = first_inventory(other_copy);
  while(ob) 
  {
    object next_ob;
	
    next_ob = next_inventory(ob);
    if (err = catch(fiddle_obj(ob)))
    {
      int old_level;

      _syslog("try_throw_out(): buggy object: " + file_name(ob)+ err + ".");
      old_level = level;
      level = 99;
      err=catch(_destruct(ob));
      level = old_level;
    }
    ob = next_ob;
  }
  ob = environment(other_copy);
  other_copy-> save_me(2);
  if (other_copy)
    destruct(other_copy);	/* Is this really needed ? Yes.*/
    
  if (restore_object("players/" + name))
    write("Points restored from the other object.\n");
  if(!toggles)
    toggles = (["wimpy" : whimpy, "brief" : 0, "abrief" : 0 ]);
  
  host = 0;
  move_player_to_start(ob);
  count_weight(); /* Fix to the correct weight and drop excessive stuff */
#ifdef LOG_ENTER
  log_file("ENTER", " (throw)\n");
#endif
}

static logon2(str) 
{
  int num_users;
  
  if (!str || str == "") 
  {
    destruct(this_object());
    return;
  }
  if (name != "logon") 
  {
    illegal_patch("logon2 " + name);
    destruct(this_object());
    return;
  }
  str = lower_case(str);
  if (!valid_name(str)) 
  {
    input_to("logon2");
    write("Give name again: ");
    return;
  }
  if (file_size("banish/" + str +".o") >= 0) {
    write("That name is reserved.\n");
    destruct(this_object());
    return;
  }

  if (!restore_object("players/" + str)) 
    write("New character.\n");


  if (!intp(level)) /* Better safe, than a bug in player. */
    level = 0;

  switch(level)
  {
  case -1000..19:
    num_users = sizeof(users()) - sizeof(filter_users(20));    
    if ((nologin || num_users >= MAX_PLAYERS) && level <= 21)
    {
      if(nologin)
	write("Login has been restricted to Archwizards only.\n");
      else
	{
	  write("Sorry, NannyMud has reached the maximum number of\n"+
		"mortals logged on, Please come back later.\n");
	  log_file("FULL", time() + " " + name + " " + level + "\n");
	  catch(load_object("/obj/daemon/statisticsd")->add_new_to_full(name,level));
	}
      destruct(this_object());
      return;
    }
    break;
  case 21:
    num_users = sizeof(filter_users(20)) - sizeof(filter_users(22));
    if (nologin || num_users >= MAX_WIZARDS)
    {
      if(nologin)
	write("Login has been restricted to Archwizards only.\n");
      else
	{
	  write("Sorry, NannyMud has reached the maximum number of\n"+
		"wizards logged on, Please come back later.\n");
	  log_file("FULL", time() + " " + name + " " + level + "\n");
	  catch(load_object("/obj/daemon/statisticsd")->add_new_to_full(name,level));
	}
      destruct(this_object());
      return;
    }
    break;
  case 22..999999:
    num_users = sizeof(filter_users(22));
    if (num_users >= MAX_ARCHWIZ)
    {
      write("Sorry, no more Arches can logon at this point.\n");
      write("Please come back later.\n");
      destruct(this_object());
      return;
    }
    break;
  default:
    break;
  }

  
  
  if(!check_access()) 
  {
    write("You may not login from this site!\n");
    "/obj/daemon/syslogd"->do_log("ILLEGAL LOGIN: "+ name +
	     " from " + (ident_user ? ident_user +"@" : "") +
	     query_ip_name()+".\n");
    destruct(this_object());
    return;
  }
 
  /* Patch by Brom and Taren 950813.
   * Seconds and wizard CAN'T logg on together any more.
   * Will perhaps solve some problems we've had with this...
   */
  if (query_level() < 20) 
  {
    if (query_secondchar() &&
	find_player(query_secondchar())) 
    {
      write("You are not allowed to log in your second character while your\n"+
	    "wizard is still logged in.\n");
      destruct(this_object());
      return;
    }
  }
  else
    if (query_level() < 23)
    {
      object *u;
      u = users() -> query_secondchar();
      if (member_array(query_real_name(), u)!=-1)
      {
	write("You are not allowed to log in as long as your second\n"+
	      "character is logged on.\n");
	destruct(this_object());
	return;
      }
    }
  

  _query_ident();  /* Start lookup */
      
  host = 0;
  time_to_save = age + 500;
  /*
   * Don't do this before the restore !
   */
  name = str;			/* Must be here for a new player. */
  dead = ghost;

  if(query_invis())
    cap_name = "Someone";
  else
    cap_name = capitalize(name);

  local_weight = 0;
  armour_class = 0;
  name_of_weapon = 0;
  weapon_class = 0;
  attacker_ob = 0;
  alt_attacker_ob = 0;

  /* If this is a new character, we call the adventurers guild to get
   * our first title !
   */
  if(level < 8) 
  { /* new player or newbie. Need to get rid of some assholes */
    if(!"obj/daemon/newbied" -> allowed_newbie(query_ip_number(this_object()),
					       lower_case(name))) 
    {
      write("New players are not allowed to login from your site.\n");
      write("Please mail nannymud@lysator.liu.se if you wish to start playing.\n");
      write("State the name of the character you wish to play in the letter.\n");
      destruct(this_object());
      return;
    }
  }
  if (name == "guest") 
  {
    if("obj/daemon/newbied"->allowed_newbie(query_ip_number(this_object()),
					    "guest"))
    {
      this_object()->check_password("");
      return;
    } else {
      write("Guest login denied from your host.\n");
      destruct(this_object());
      return;
    }
  }
#ifdef LOG_ATTEND
  if (   name
      && name != "logon"
      && name != "guest")
    catch(write_file("log/ATTEND",
		     sprintf("%d + %-11s %d\n",
			     time(), name,
			     sizeof(users() - ({ 0 }) ))));
#endif
  if (level != -1)
    input_to("check_password", 1);
  else
    input_to("new_password", 1);
  write("Password: ");
  return;
}

/* Called by command 'save' */
save_character() 
{
  save_me(1);
  return write("Ok.\n"), 1;
}

reset(arg) 
{
  if (arg) return;
  if(name) return;
    
  if (creator(this_object())) 
  {
    illegal_patch("Cloned player.c");
    destruct(this_object());
    return;
  }

  statue_type = "a nice";
  level = -1;
  is_linkdead = 0;
  name = "logon";
  cap_name = "Logon";
  msgin = "arrives"; msgout = "leaves";
  mmsgin = "arrives in a puff of smoke";
  mmsgout = "disappears in a puff of smoke";
  setenv(TITLE, "the title less");
  al_title = "neutral";
  term_rows = 24;
  term_cols = 80;
  term_type = "unknown";
  host = 0;
  gender = -1;			/* Illegal value, so it will be changed! */
  my_quest_max = 0;		/* Illegal value, so it will be changed! */
  view_news = 1;		/* We want new users to read the news.   */
}

/* Enable other objects to query our hit point. */
query_hit_point() { return hit_point; }

fix_title(string var, string value)
{
  string newtitle, tmp;

  if(!var || var != TITLE)
    return;
  
  if(!value)
  {
    title = "";
    return;
  }

  if(sscanf(value, "%s$N", tmp) == 1)
  {
    newtitle = replace(value, "$N", capitalize(name));
    if(!newtitle)
      setenv(TITLE, capitalize(name) + "  the title less");
    else
    {
      if(sscanf(newtitle, "%s" + capitalize(name), tmp) != 1)
	setenv(TITLE, capitalize(name) + " " + newtitle);
      else
	setenv(TITLE, newtitle);
    }
    return;
  }
  if(sscanf(value, "%s" + capitalize(name), tmp) != 1)
    setenv(TITLE, capitalize(name) + " " + value);

  title = getenv(TITLE);
}
  
short()
{
  string newtitle;

  if (query_invis())
    return 0;
  if (ghost)
    return "ghost of " + cap_name;
  if (!interactive(this_object()))
    return statue_type + " statue of " + cap_name;
  if (frog)
    return cap_name + " the frog";
  if(newtitle = getenv(TITLE)) 
    return newtitle +  " (" + al_title + ")";
  return cap_name + " " + title + " (" + al_title + ")";
}

long(str) 
{
  string cap_pronoun;

  if (!interactive(this_object())) 
  {
    write(capitalize(statue_type) + " stone statue of " + cap_name + ".\n");
    return;
  }

  cap_pronoun = capitalize(query_pronoun());
  write(short() + ".\n");
#ifdef USE_WETCODE
  if (wet_time > 0 && !environment(this_object())->query_property("waterfilled"))
    write(cap_pronoun +
	  " is soaking wet, dripping water all over the place.\n");
#endif
  if (ghost || frog)
    return;

  if (hit_point < max_hp/10) {
    write(cap_pronoun + " is in very bad shape.\n");

  }
  else if (hit_point < max_hp/5) {
    write(cap_pronoun + " is in bad shape.\n");
  }
  else if (hit_point < max_hp/2) {
    write(cap_pronoun + " is not in a good shape.\n");
  }
  else if (hit_point < max_hp - 20) {
    write(cap_pronoun + " is slightly hurt.\n");
  }
  else
    write(cap_pronoun + " is in good shape.\n");

  if (query_playerkilling())
    write(capitalize(query_possessive()) + " conscience will not stop "
	  + query_objective() +" from killing other players.\n");

}

coins()
{
  if(ghost)
    return notify_fail("You have no physical belongings as a ghost.\n"),0;
  return write(money ? "You have "+ money +" gold coin"+ 
	       (money == 1 ? "" : "s") +".\n" :
	       "You don't have any money.\n"), 1;
}

score(arg) 
{
  string tmp;
  object ob;
  
  
  if (ghost) {
    write("You are in an immaterial state with no scores.\n");
    return 1;
  }
  
  if (arg)
  {
    write("Str: " + this_object()->query_str() + "\n");
    write("Dex: " + this_object()->query_dex() + "\n");
    write("Int: " + this_object()->query_int() + "\n");
    write("Con: " + this_object()->query_con() + "\n");
    return 1;
  }
  write(short()+".\n");
  write("You have " + experience + " experience points, " +
	money + " gold coins, ");
  write(query_cached_quest_points()+ " quest points ("+
	(my_quest_max * QUEST_POINTS_PERCENT_TO_WIZ) / 100  +").\n");
  write(hit_point + " hit points(" + max_hp + ") and ");
  write(spell_points + " spell points("+max_sp+").\n");
  if (hunter)
    write("You are hunted by " + capitalize(hunter->query_name()) + ".\n");
#ifdef USE_FOLLOW
  if (ob = query_follow())
    write("You are following " + capitalize(ob->query_name()) + ".\n");
#endif
  
  if (intoxicated || stuffed || soaked)
  {
    tmp = "You are ";
    
    if (intoxicated)
    {
      tmp += "intoxicated";
      if (stuffed && soaked)
	tmp += ", ";
      else
      {
	if (stuffed || soaked)
	  tmp += " and ";
	else
	  tmp += ".\n";
      }
    }
    
    if (stuffed)
    {
      tmp += "satiated";	
      if (soaked)
	tmp += " and ";
      else
	tmp += ".\n";
    }
    if (soaked)
      tmp += "not thirsty.\n";
    write(tmp);
  }
  if (toggles["wimpy"])
  {
    write("Wimpy mode [" + (toggles["wimpy"] > 1 ? toggles["wimpy"] : "on") +
	  "]\n");
  }
  if (query_playerkilling())
    write("Your conscience will not stop you from killing other players.\n");
  show_age();
  return 1;
}

/* Identify ourself. */
id(str, lvl) 
{
  if (!str)
    return 0;
  
  str = lower_case(str);
  if (query_invis() && lvl <= level) /* WRONG /Profezzorn */
    return 0;
  if (ghost)
    return str == "ghost of " + name;
  if(str == name)
    return 1;
  if(!this_object()) return 0;
  if (!query_ip_number(this_object()))
    return str == "statue" || str == "statue of " + name;
  return 0;
}

query_title() 
{
  string newtitle;
  if(newtitle = getenv(TITLE)) 
  {
    newtitle = replace(newtitle, "$N", cap_name);
    if (!newtitle) return title;
    return newtitle;
  }
  return title;
}

query_al_title() { return al_title; }

static do_log()
{
  write_file("/log/SECURE/PLAYERS",name+
	     " lvl:"+level+
	     " gc:"+money+
	     " xp:"+experience+
	     " Str:"+Str+
	     " Int:"+Int+
	     " Dex:"+Dex+
	     " Con:"+Con+
	     " qpmax:"+my_quest_max+
	     " quests:"+quests+
	     "\n");
	     
}

set_level(lev) 
{
  object scroll;
  
#if 1
  /* Padrone loggar listigt l{bbiga luringar: */
  log_file("LEVEL-TEMP-LOG", ctime(time()) + ": " +
	   query_real_name() + " to " + lev + "\n");
  if (previous_object())
    log_file("LEVEL-TEMP-LOG", "previous_object() = " +
	     file_name(previous_object()) + " (" +
	     previous_object()->query_real_name() + ")\n");
  if (this_player())
    log_file("LEVEL-TEMP-LOG", "this_player() = " +
	     file_name(this_player()) + " (" +
		 this_player()->query_real_name() + ")\n");
#endif
  if(query_real_name()=="guest")
  {
    level=1;
    return write("Sorry, as a guest you cannot advance in level.\n"), 0;
  }
  
  if (sscanf(file_name(previous_object()), "obj/leo%*s") != 1)
    if (lev > 21 || lev < level && level >= 20 || level < 0)
      /* Wing 920502, you shouldn't be able to change level of logon
	 (no ways to snoop logon no I hope) */
      return illegal_patch("set_level");		/* NOPE ! */
  
  /* Paragons might become wizards, but not automatically. */
  if (paragon && level == 19 && lev > 19)
  {
    write("As a paragon, you can't become a wizard without the admins help.\n");
    return;
  }

  hook("changed_level_hook",({level,lev}));
  level = lev;
  if (level == 20) {
    tell_object(this_object(), "Adding wizard commands...\n");
	wiz_setup();
  }
  do_log();
}


static wiz_setup()
{
  string error;
  init_wiz_line();
  add_action("wiz","wiz",1);
  add_action("setmmin", "setmmin");
  add_action("setmmout", "setmmout");
  add_action("setmin", "setmin");
  add_action("setmout", "setmout");
  add_action("review", "review");
  add_action("invis", "invis");
  add_action("vis", "vis");
/* Added to prevent stupid wizwars...*/
  add_action("fix_me","wizfixme");

  if(error = catch(query_wiztool()))
    write(error+"\n");
}

#define CHECK() \
  if(this_player() != this_object() && \
     this_object()->query_level() >= 20 && \
     this_player()->query_level() <= this_object()->query_level()) \
    return 0

int setmin(string m) { CHECK(); return ::setmin(m); }
int setmiout(string m) { CHECK(); return ::setmout(m); }
int setmmin(string m)
{
  CHECK();
  if(m=="dissappears in a puff of smoke" && previous_object())
  {
    log_file("FIX","setmmin from "+file_name(previous_object())+".\n");
  }
  return ::setmmin(m);
}
int setmmout(string m) { CHECK(); return ::setmmout(m); }

set_title(t) 
{
  if(!t)
    return 0;
  if (this_player() != this_object() &&
      level >= 20 &&
      this_player()->query_level_sec() <= level)
    return 0;
  if(this_player()!=this_object() &&
     this_player(1)!=this_object() &&
     this_player()->query_level()>19)
  {
    "/obj/daemon/syslogd"->do_log("TITLE:"+this_player(1)->query_real_name()+" changed "+
	     query_real_name()+" to '"+t+"'\n");
  }
  setenv("TITLE",t);
  return 1;
}

/*
   _New_ shout routines to allow earmuffs via catch_shout()
   
   Possible future extension:
   filter_array() returns an array holding the players that accepted
   the shout(), that is those who heard it.
   These could be listed to the shouter if one wishes.
   */

#define SHOUT_OLD(x) shout(x)
#define SHOUT(x,y) users()->catch_shout(x,y)

static int listen_to_shouts_from_level;

/* this function can be removed now /Profezzorn */
filter_tell(ob,string gTellstring )
{
  log_file("FIX", "called filter_tell: "+file_name(previous_object())+"\n");
  if (ob == this_player())
    return 0;
  return ob->catch_shout(gTellstring);
}

/* This is called for every shouted string to this player.*/
catch_shout(who,str)
{
  if(!str)
  {
    str=who;
    who=0;
  }
  if(previous_object() == this_object()) return;
  if (listen_to_shouts_from_level < 2  ||
      (this_player() &&  this_player()->query_level() >= listen_to_shouts_from_level)) 
  {
    if(who)
      tell_object(this_object(), line_break(str, who + " shouts: "));
    else
      tell_object(this_object(), line_break(str));
    return 1;
  }
  return 0;
}

earmuffs(str) 
{
  int lev;
  if (str && (lev = atoi(str)) && (lev>=0))
    listen_to_shouts_from_level=lev;
  return write("Earmuffs at level " + listen_to_shouts_from_level + ".\n"), 1;
}


quit() 
{
  string errstr;
  int tmp_level;
  object ob, next;
  int temp_level;
  string err;
  last_logout_time_int=time();    
/*
   last_login_time = CURRENT_DATE;
   last_login_time_int = time();
  */
  save_me(0);

  errstr = catch(drop_all(0));	/* catch /pen 920116 */
  if (errstr)
  {
    _syslog("Error in quit():drop_all() for player " + capitalize(name) +
	    ": " + errstr + ".");
    errstr="";
    for(ob=first_inventory(this_object());ob;ob=next_inventory(ob))
      errstr+=" "+file_name(ob)+" ";
    _syslog("Inventory contained:"+errstr+".\n");
  }

  ob = first_inventory(this_object());
  while (ob)
  {
    next = next_inventory(ob);
    if (err = catch(destruct(ob)))
    {
      _syslog("destruct_all(): destruct() failed: " + err + ".");
      temp_level = level;
      level = 99;
      catch(_destruct(ob));
      level = temp_level;
    }
    ob = next;
  }
    
  if (!query_invis() && !is_linkdead) 
    say((ghost ? "The mist" : cap_name) +" left the game.\n");
  
  /*  Notify mudfinger of the logout   /Gwendolyn 930513  */
  if(!query_invis() || level <= 22)
    catch("/obj/daemon/mudwhod"->send_logout(this_object()));
  

  errstr = catch(destruct(this_object()));
  if (errstr)
  {
    _syslog("Error in quit():destruct() for player " + capitalize(name) +
	    ": " + errstr + ".");
    tmp_level = level;
    level = 99;
    if (catch(_destruct(this_object())))
    {
      level = tmp_level;
      _syslog("Error in quit():_destruct() for player " + capitalize(name) +
	      ": " + errstr + ".");
      _disconnect(this_object());
    }
    level = tmp_level;
  }
  return 1;
}

pre_quit()
{
  if(attacker_ob)
  {
    call_out("quit",1); /* This fixes a bug in the fighting system... */
    return 1;
  }else{
    return quit();
  }
}

kill(str) 
{
  object ob;
  string living_ob;
  string weapon_ob;
  
  if (ghost)
    return 
      notify_fail("You can't kill anything in your immaterial state.\n"), 0;
  
  if (!str)
    return notify_fail("Kill what?\n"), 0;
  
  ob = present(lower_case(str), environment(this_player()));
  if (!ob) 
  {
    if (sscanf(str, "%s with %s", living_ob, weapon_ob)) 
    {
      ob = present(lower_case(weapon_ob), this_player());
      if (!ob) 
	return notify_fail("Kill "+ living_ob +" with what?\n"), 0;
      
      if (name_of_weapon != ob && !wield(ob))
	return 1;
      
      str = living_ob;
      ob = present(lower_case(living_ob), environment(this_player()));
    }
  }
  set_heart_beat(1);
  
  str = capitalize(str);
  if (!ob) 
    return notify_fail("No " + str + " here !\n"), 0;
  
  if (!living(ob)) 
  {
    notify_fail(str + " is not a living thing!\n");
    say(cap_name + " foolishly tries to attack " + str + ".\n");
    return 0;
  }

  if (ob == this_object()) 
    return notify_fail("What?! Attack yourself?\n"), 0;

  if(attacker_ob == ob)
  {
    write("Yes, yes.\n");
    return 1;
  }

  if (!attack_allowed(ob))
    return 1;

  hook("start_kill_hook",({attacker_ob,alt_attacker_ob,ob}));
  
  if (alt_attacker_ob == ob || ob->query_attack() == this_object())
  {
    alt_attacker_ob = attacker_ob;
    attacker_ob = ob;
    write("You turn to attack "+str+".\n");
    say(cap_name+" turns to attack "+str+".\n");
    return 1;
  }
  attack_object(ob);
  return 1;
}


/* who wrote this? /Profezzorn. Milamber, Milamber*/
void sendmsg(object ob, string pre, string msg)
{
  tell_object(ob, sprintf(pre+ " %-=*s\n", (ob->query_cols()||80) 
			  - strlen(pre) - 2,
			  msg));
}


communicate(str) 
{
  string verb, a;
  object ob;

  verb = query_verb();
  if (!str)
    str = "";
  if (verb[0] == '\'')
    str = verb[1..1000] +" "+ str;
  if(toggles["say"])
    write(line_break(str, "You say: "));
  else
    write("Ok.\n");

  a = (ghost ? (short() ? capitalize(short()) : "Someone")
       : cap_name) + " says:";
  verb = a + " " + str + "\n";

  for(ob=first_inventory(environment());ob;ob=next_inventory(ob))
  {
    if(living(ob) && ob != this_object())
      if(interactive(ob))
	sendmsg(ob, a, str);
      else
	tell_object(ob, verb);
  }
  tell_object(environment(),verb); /* Profezzorn */

  return 1;
}

/* Busy code by Profezzorn (last change 950111) */
int query_busy()
{
  int i;
  i=busy-time(); /* Was time()-busy; Brom 950112:00.04 */
  if(i<0) i=0;
  if(i==1) return 1;
  return i/2;
}

varargs int check_busy(int b)
{
  if(!interactive(this_object()) || ghost || query_busy()) 
    return 1;
  if(b <= 0) b=1;
  busy=time() + b*2;
  /* Was +b only, BUT b is HB's not seconds!!!
     /Taren and Rohan, Date: Wed Mar 13 16:43:25 1996
   */
  return 0;
}

int remove_busy() { busy = 0; }


/* I made some small changes here, to restart the heart_beat
 * when a player loses it. I can see no bad side effects from
 * it. If you do, please tell me....    /Milamber
 */

static int cardiac;

void restart_dah_heart_beat()
{
  cardiac++;
  set_heart_beat(1);
  tell_object(this_object(),"Heart beat restarted.\n");
#ifdef PARANOIA
  log_file("LOSTHEARTBEAT",cap_name+", "+ctime(time())+" ("+cardiac+")\n");
#endif
  tell_room(environment(),cap_name+" suddenly looks pale and utters: "+
	    "I had a cardiac attack!\n");
}

static int attacks;

attack()
{
  set_heart_beat(1);
  if(attacks)
  {
    attacks--;
    return ::attack();
  }
  return 0;
}

hit_player(int dam,int ignore)
{
  if(!interactive(this_object()))
  {
    write("You cannot fight with statues\n");
    
    stop_fight();
    if(this_player())
      this_player()->stop_fight();

    return 0;
  }

  if (this_player() != this_object() &&
      !ignore &&
      this_player() && 
      !this_player()->query_npc()) 
  {
#ifdef PARANOIA
    /* A player trying to hit another player */
    log_file("PLAYERKILLING", ctime(time()) + ": " +
	     query_real_name() + " was hit (" + dam + ") by " +
	     this_player()->query_real_name() + "\n");
    if (previous_object())
      log_file("PLAYERKILLING", "previous_object() = " +
	       file_name(previous_object()) + " (" +
	       previous_object()->query_real_name() + ")\n");
#endif    
    if (!environment(this_object()) ||
	(!environment(this_object())->query_playerkilling() &&
	 (!query_playerkilling() || !this_player()->query_playerkilling()))) 
    {
      write("You try to hit "+query_name()+
	    " but your conscience stops you.\n");
      tell_object(this_object(), this_player()->query_name() + 
		  " tries to hit you, but " +
		  this_player()->query_possessive() + " conscience stops " +
		  this_player()->query_objective() + ".\n");
#ifdef PARANOIA
      log_file("PLAYERKILLING", "DENIED.\n");
#endif
      stop_fight();
      this_player()->stop_fight();
      return 0;
    }
  }

  return ::hit_player(dam, ignore);
}



#define DUM(X) X(h,ignore,noshow) { ::X(h,ignore); if(!noshow) show_hpinfo(); }
DUM(restore_spell_points)
DUM(reduce_spell_points)
DUM(heal_self)
DUM(reduce_hit_point)
DUM(add_hp)
DUM(add_sp)

#undef DUM



static int visa_sp_eller_inte_typ=1, visa_hp_eller_inte_typ=1;
set_visa_sp_eller_inte_typ(int i) { visa_sp_eller_inte_typ = i; }
set_visa_hp_eller_inte_typ(int i) { visa_hp_eller_inte_typ = i; }

static heart_beat()
{
  if (ghost)
  {
    set_heart_beat(0);
    return;
  }
  attacks = 3;
  call_out("restart_dah_heart_beat",1); /* Milamber */

  show_hpinfo();

  /* Added by Qqqq */
  hook("fast_beat_hook",({attacker_ob}));

  if (attacker_ob) 
    this_object()->attack();


  if (attacker_ob && toggles["wimpy"] && hit_point < 
      (toggles["wimpy"] > 1 ? toggles["wimpy"] : max_hp / 5))
    this_object()->run_away();

  show_hpinfo();

  remove_call_out("restart_dah_heart_beat"); /* Milamber */
}

static show_hpinfo()
{
  string foe_name;

  if(hit_point != old_hp || spell_points != old_sp) 
  {
    update_prompt();
    if(toggles["hpinfo"])
    {
      tell_object(this_object(),
		  (visa_hp_eller_inte_typ ?
		   "** HP: "+ hit_point +"/"+ max_hp : "")+
		  (visa_sp_eller_inte_typ ?
		   "   SP: "+ spell_points +"/"+ max_sp : "") +"\n");
    }


    hook("changed_shape_hook",({hit_point,old_hp,spell_points,old_sp}));

    old_hp = hit_point;
    old_sp = spell_points;
    if(toggles["shape"]&&objectp(attacker_ob) &&
      (environment(attacker_ob) == environment()))
      {
	foe_name = attacker_ob -> query_name();
	if (!stringp(foe_name)) foe_name = "Your opponent";
	tell_object(this_object(),"** "+capitalize(foe_name)+" is in "+show_shape(attacker_ob)+" shape.\n");
      }
  }
}

/*
 * Update our aligment.
 */

add_alignment(a) {
  int old_al;

  
  if (!intp(a)) {
    write("Bad type argument to add_alignment.\n");
    return;
  }
  old_al=alignment;
  alignment = alignment*9/10 + a;
  hook("changed_alignment_hook", ({ alignment, old_al, previous_object() }) );

  /*Added check for paragons, Taren
    Date: Tue Mar 19 18:51:18 1996
   */
  if (level > 20|| query_paragon()) 
    return;
  switch(alignment)
  {
  case KILL_NEUTRAL_ALIGNMENT*100+1..0x7fffffff:
    al_title="saintly";
    return;
    
  case KILL_NEUTRAL_ALIGNMENT*20+1..KILL_NEUTRAL_ALIGNMENT*100:
    al_title="good";
    return;
    
  case KILL_NEUTRAL_ALIGNMENT*4+1..KILL_NEUTRAL_ALIGNMENT*20:
    al_title="nice";
    return;
    
  case -KILL_NEUTRAL_ALIGNMENT*4+1..KILL_NEUTRAL_ALIGNMENT*4:
    al_title="neutral";
      return;
    
  case -KILL_NEUTRAL_ALIGNMENT*20+1..-KILL_NEUTRAL_ALIGNMENT*4:
    al_title="nasty";
    return;
    
  case -KILL_NEUTRAL_ALIGNMENT*100+1..-KILL_NEUTRAL_ALIGNMENT*20:
    al_title="evil";
    return;
    
  default:
    al_title="demonic";
    return;
  }
}

set_al(a) {
    if (intp(a)) alignment = a;
}

set_alignment(al) { al && (al_title = al); }

test_dark() 
{
  if (set_light(0) <= 0) 
    return write("It is too dark.\n"),1;
}

mixed put_obj(object o, object *c)
{
  int i,j;
  object obj;
  string oname;

  if(member_array(environment(o),c) != -1) /* Really.. */
    return 0;
  
  oname =_name(o);
  for(i=0;i<sizeof(c);i++)
  {
    if(!living(c[i]) && !o -> get())
      return 0;

    if(c[i] != this_player() &&
       c[i] != environment(this_player()) &&
       (!environment(c[i]) ||
	(
	 environment(c[i]) != this_player() &&
	 environment(c[i]) != environment(this_player())
	 )
	)
       )
    {
      write("You can't reach there.\n");
      return 0;
    }

    if(c[i]->_signal_please_take_this(o)) continue; /* DON'T THEN! */
    if(!c[i]->can_put_and_get(o)) continue;

    if (!(j=transfer(o, c[i])))
    {
      if (o == name_of_weapon)
      {
	o->unwield();
	stop_wielding();
      }
      if(c[i] && o) c[i]->_signal_gotten(o);
      if(o) o->_signal_given();
      if(o)
	return ({ oname+_name(c[i],1),o });
      else
	return ({ "", 0 });
    } else {
      switch(j)
      {
      case 1:
	if(living(c[i]))
	  write(c[i]->query_name()+" cannot carry the "+oname+".\n");
	else
	  write(_name( c[i] ) + " cannot contain "+oname+".\n");
	break;			/* Try the next container, if any. */

      case 2:
	return 0;		/* drop() returned 1, and should write the message */
	
      case 3:
	write("You cannot remove "+oname+" from "+_name(environment(o))+".\n");
	return 0;		/* This is not all that likely to change the next time */

      case 4:
	if(living(c[i]))
	  write("The "+oname+" will not let itself be given to "+
		c[i]->query_name()+".\n");
	else
	  write("The "+oname+" will not let itself be put into the "+
		_name(c[i])+".\n");
	break;			/* This might change with the object, so lets try the next
				   target... */

      case 5:
	if(living(c[i]))
	  write(c[i]->query_name()+" will not take the " + oname + ".\n");
	else
	  write("The "+_name(c[i])+" will not contain the " + oname + ".\n");
	break;			/* This might change with the object, so lets try the next
				   target... */

      case 6: 
	return 0;		/* This object cannot be moved. */
      }
    }
  }
  write("Couldn't "+query_verb()+" the " + oname + ".\n");
}

mixed drop_obj(object ob)
{
  int t;
  object obj, me;
  string __name,tmp;
  if(!ob->short())
    return 0;

  if (ob == name_of_weapon && ob->query_weapon())
  {
    ob->unwield();
    stop_wielding();
  }
  __name = _name(ob);
  if (tmp=ob->query_property("fragile") &&
      !transfer(ob, environment(this_player())))
    {
      if(tmp&&stringp(tmp))	
	"/std/msg"->msg(tmp);	
      else
	{
	  say(this_player() -> query_name() +
	      "drops a " + __name + ". As it lands, it breaks.\n");
	  write("When the " + __name + " lands, it breaks.\n");
	}
      destruct(ob);
      return 0;
    }
    

  if(!(t=transfer(ob, environment(this_player())))) return ({__name,ob});
  switch(t)
  {
    
   case 5:
   case 4:
   case 1:
    write("You cannot drop "+__name+" here.\n");
    return 0;

   case 2:
    return 0;		/* drop() returned 1, and should write the message */
	
   case 3:
    write("You cannot remove "+__name+" from "+_name(environment(ob))+".\n");
    return 0;

   case 6: 
    return 0;			/* This object cannot be moved. */
  }
  return 0;
}

status put(string str) 
{
  object *cont;
  string a,b;
  if (!str)
    return notify_fail("Put what in what?\n"), 0;
  if (test_dark())
    return 1;
  if (sscanf(str, "%s in %s", a, b) != 2) 
    return notify_fail("Put what in what?\n"), 0;
  if (query_ghost())
  {
    write("It slips right through your immaterial form.\n");
    return 1;
  }
  cont =(object *)get_objects(lower_case(b),({this_player(), environment()}));
  if (!sizeof(cont))
    return notify_fail("No '" + b + "' here!\n"), 0;
  
  if(sizeof(cont = map_array(get_dropable_objects(lower_case(a)) - cont,
			     "put_obj", this_object(), cont) -({0})))
    return describe_objects(cont,"put"), 1;
  else 
    notify_fail("Sorry, but you failed to move anything.\n"), 0;
}

int pick_up(string str) 
{
  mixed o;
  if(!str) 
    return notify_fail("Take what?\n"), 0;
  if (ghost) 
    return notify_fail("You cannot take things in your immaterial state.\n"),0;

  if (test_dark()) return 1;
  if(sizeof(o = map_array(get_getable_objects(str), 
			  "pick_obj", this_object()) - ({0})))
    return describe_objects(o, "take"), 1;
  return write("Nothing to "+query_verb()+".\n"),1;
}

string pick_obj(object ob) 
{
  int i;
  string __name,___name;
  if(environment()!=this_object() && 
     (interactive(environment(ob)) || environment(ob)->query_npc()
      || file_name(environment(ob))[0..9]=="obj/player"))
  {
    write("You can't take things from other people.\n");
    return 0;
  }
  __name = (___name=_name(ob))+_name(environment(ob),1);
  if (this_object() && !(i=transfer(ob, this_object())))
    return __name;

  switch(i)
  {
  case 1:
    write("You can't carry the "+ ___name +".\n");
    break;
  }
  return 0;
}

drop_thing(obj) 
{
  int i, coins;
  mixed *o;
  
  if (!obj) 
    return notify_fail("Drop what?\n"), 0;

  if (sscanf(obj, "%*s in %*s") == 2) 
    return put(obj), 1;
  o=get_dropable_objects(obj, ({ this_object() }));
  if(sizeof(o))
  {
    o=map_array(o, "drop_obj", this_object())-({ 0 });
    if( sizeof( o ) )
    {
      describe_objects(o,"drop");
      /* Fixed by Taren, Tue Jul 11*/
      if(objectp(o)) o->_signal_dropped();
    } 
    return 1;
  } else {
    notify_fail("Drop what?\n");
  }
}

/* This grotesque hack by Brom 951213 (and I am sober).
 * Added the test flag to it, then it will be possible to
 * easily test if we can carry.
 */
add_weight(w, test) 
{
  int max;

  max = query_str() + 10;	/* Mats: was level + 10 */
  if (frog)
    max = max / 2;
  if (test == 1)
    return !((w + local_weight) > max);
  
  if ((w + local_weight > max) && (w > 0)) /* Oros 910422 */
    return 0;
  if(w + local_weight < 0)
    return 0;
  local_weight += w;
  return 1;
}


/* 
 * This function will make the player drop all stuff he cannot carry.
 * It's run every 400 seconds.
 */
count_weight()
{
  int i, max, a;
  object *o;
  string *to_drop;
  to_drop = ({});
  a = sizeof(o = all_inventory(this_object()));
  local_weight = 0;
  max = (query_str() + 10) / (frog ? 2 : 1);
  while(a--)
  {
    local_weight += o[a] -> query_weight();
    if(local_weight > max)
    {
      if(!i)
      {
	write("You carry too much.\n");
	i = 1;
      }
      to_drop += ({drop_obj(o[a])});
    }
  }
  if(sizeof(to_drop))
    describe_objects(to_drop, "drop");
}

shout_to_all(str) 
{
  int i;

#ifdef NO_LVL_1_SHOUT
  if (level == 1)
  {
    write("You cannot use 'shout' at level 1.\n"+
	  "There are several ways of getting information:\n"+
	  "+ Read your newbie booklet.\n"+
	  "+ Use the command 'help'.\n"+
	  "+ Ask fellow players.\n");
    return 1;
  }
#endif

  if (spell_points < 30) 
    return notify_fail("You are low on power.\n"), 0;
  
  if (!str) 
    return notify_fail("Shout what?\n"), 0;
  
  if (ghost) 
    return notify_fail("You fail.\n"), 0;
  if (frog /* || (i = (upper_case(str) == str)) */)
  { 
    
    SHOUT(cap_name + (i ? "" : " the frog")+ " shouts: Hriibit! Hrriiibit!\n",0);
    if(toggles["shout"])  
      write(line_break( str, "You shout: "));
    else
      write("Ok.\n");
  } else {
    SHOUT(cap_name, str ); 
    if(toggles["shout"])
      write(line_break( str, "You shout: "));
    else
      write("Ok.\n");
  } 
  if (level < 20)
    add_sp(-30);
  return 1;
}

status I_all=1;
static object I_magobj;

writei(ob)
{
  string str;
  str=ob->short();
  if(!str)
    return;
  write(capitalize(str));
  if (name_of_weapon && ob == name_of_weapon) 
    if (name_of_weapon->query_property("cursed"))
      write(" (wielded & cursed)");
    else
      write(" (wielded)");
  if (I_magobj && I_magobj->check_bound(ob))
    write(" (bounded)");
  write(".\n");
}

inventory(arg)
{
  object *a;
  int i;
  string  s,q,d;   
  I_magobj=present("magobj",this_object());

  if(arg == "switch" || arg == "toggle")
  {
    I_all=!I_all;
    if(I_all)
      write("'i' will now always show ALL visible objects.\n");
    else
      write("'i' will now show only what you choose.\n");
    return 1;
  }

  if(I_all && !arg)
    arg="*";
  
  a = all_inventory(this_object());
  if (test_dark()) return 1;
  if(!arg)
  {
    write("Inventory, excluding auto-loaders:\n");
    for(i=0;i<sizeof(a);i++)
    {
      if(a[i] && !stringp(function_exists("query_auto_load", a[i])) && a[i]->short())
	writei(a[i]);
    }
    return 1;
  } else {
    switch(arg[0])
    {
     case 'a':
      write("Inventory, only auto-loaders:\n");
      i = sizeof(a);
      while(i--)
	if(a[i] && stringp(function_exists("query_auto_load", a[i])))
	  writei(a[i]);
      return 1;
      
     case 'w':
      write("Inventory, weapons and armours:\n");
      i = sizeof(a);
      while(i--)
	if(a[i]&&(a[i]->weapon_class()||a[i]->armour_class()))
	  writei(a[i]);
      return 1;
      
     case 'W':
      write("Inventory, wielded weapons and worn armours:\n");
      i = sizeof(a);
      while(i--)
	if(a[i] && (a[i] -> weapon_class() || a[i] -> armour_class()) && 
	   (a[i] -> short()) && (a[i] -> query_worn() || a[i] -> 
				 query_wielded() || a[i] == name_of_weapon))
	  writei(a[i]);
      return 1;
      
     case 'o':
     case 't':
      write("Inventory, other objects (no auto-loaders, armours or weapons): \n");
      i = sizeof(a);
      while(i--)	
	if(a[i] && (!(a[i] -> weapon_class() || a[i] -> armour_class()) && 
		    (a[i] -> short()) &&
		    (!stringp(function_exists("query_auto_load",a[i])))))
	  writei(a[i]);
      return 1;
      
     case '*':
      q=arg[1..10000];
      if(strlen(q))
      {
	i = sizeof(a);
	while(i--)
	  if(a[i]&&(s=(string)a[i]->short()))
	    if(sscanf(s,"%s"+q+"%s",d,d)==2)
	      writei(a[i]);
	return 1;
      } else {
	map_array(a,"writei",this_object());
	return 1;
      }
     default:
      write("i w[eapons and armours]\n"+
	    "  W[orn] armours and wielded weapons\n"+
	    "  o[thers], also known as t[reasures]\n"+
	    "  *[str] Show all items matching str, or if no str, all items.\n"+
	    "  a[uto loaders]\n"+
	    "  switch Toggle extended-i on or off.\n");
      
      return 1;
    }
  }
}

examine(str) 
{
  if (!str) 
    return notify_fail("Examine what?\n"), 0;
  return look("at " + str);
}

#define C(x) capitalize(x)
#define F(x) (x && C(replace(x -> short() || "something", "(worn)", "")))
list_equipment()
{
  int i;
  object *a;
  mapping eq;
  if(ghost)
    return notify_fail("You have no physical belongings as a ghost.\n"),0;

  eq = ([]) + worn_armour;
  eq["weapon"] = name_of_weapon;
  a = all_inventory(this_object());
  i = sizeof(a);
  
  write(sprintf("Location          Armour/Weapon\n"+
		"---------------------------------------------------------\n"+
		"Head                   %-40s\n"+
		"Neck                   %-40s\n"+
		"Back                   %-40s\n"+
		"Body                   %-40s\n"+
		"Hands                  %-40s\n"+
		"Finger                 %-40s\n"+
		"Feet                   %-40s\n"+
		"Shield                 %-40s\n\n"+
		"Wielded                %-40s\n"+
		"---------------------------------------------------------\n",
		(F(eq["helmet"]) || "-"), (F(eq["amulet"]) || "-"),
		(F(eq["cloak"]) || "-"), (F(eq["armour"]) || "-"),
		(F(eq["glove"]) || "-"), (F(eq["ring"]) || "-"),
		(F(eq["boot"]) || "-"), (F(eq["shield"]) || "-"),
		(F(eq["weapon"]) || "-")));  
  return coins();
}
#undef C
#undef F

look(str) 
{
  object ob, ob_tmp, magobj;
  object wield_ob;
  string item;
  int max;
    
  if(environment()->dark(str))	/* Mats */
    return 1;
    
  if(test_dark())
    return 1;

  update_prompt();
  if (!str || str == "at room" || str == "around" || str == "at the room")
    return support::display_look(environment(),1), 1; /* Show the env */

  str = lower_case(str);
  magobj = present("magobj",this_object());
  if (sscanf(str, "at %s", item) == 1 ||
      sscanf(str, "in %s", item) == 1)
  {
    int weight;
    string desc;
    
    sscanf(item, "the %s", item);
    sscanf(item, "a %s", item);
    sscanf(item, "an %s", item);
    
    if(sscanf(item, "%s in environment", str) ||
       sscanf(item, "%s in the environment", str))
      item = str; /* Force look in environment only */
    else 
      ob = present(item, this_object());

    if (!ob) 
    {
      if(environment(this_player()) -> id(item))
      {
	display_item(environment(this_player()), item);
	return 1;
      }else {
	ob = present(item, environment(this_player()));
      }
    }
    
    if (!ob) 
    {
      if (desc=environment(this_player()) -> query_mentioned_in_long(item))
      {
	if(stringp(desc))
	{
	  notify_fail(desc);
	}else{
	  notify_fail("You look at the "+ item +
		      ", but you see nothing special.\n");
	}
	return 0;
      }else{
	notify_fail("There "+ (item[-1] == 's' ?"are":"is") +" no "+ 
		    item + " here.\n");
	return 0;
      }
    }
    display_item(ob, item);

    /* Do not show any more if it is an item */

    if((ob->query_item_map() || ([])) [item] ||
       -1!=member_array(item, ob->query_items() || ({})) ||
       stringp(ob->query_item(item)))
      return 1;

    weight = ob->query_weight();

    if (!living(ob) &&
	!ob->query_property("no_weight_message"))
    {
      if (weight >= 5)
	write("It looks very heavy.\n");
      else if (weight >= 3)
	write("It looks heavy.\n");
    }

    if (magobj && magobj->check_bound(ob)) 
      write("It is magically bounded to you.\n");

    if (!ob -> can_put_and_get(item))
    {
      return 1;
    }
    else if(ob->_display_inventory()) /* added by Profezzorn */
    { 
      return 1;
    } else {
      object special;
      special = first_inventory(ob);

      while(objectp(special)) 
      {
	string extra_str;
	if (stringp(extra_str = special-> extra_look()))
	  write(capitalize(extra_str) + ".\n");
	if (objectp(special))
	  special = next_inventory(special);
      }

      ob_tmp = first_inventory(ob);
      while(ob_tmp && ob_tmp -> short() == 0)
	ob_tmp = next_inventory(ob_tmp);
      if (ob_tmp) 
	write("\t" + capitalize(item) + 
	      (living(ob) ? " is carrying:\n" : " contains:\n"));
      
      max = MAX_LIST;
      ob = first_inventory(ob);
     while(ob && max > 0) 
     {
       string sh;
       if (stringp(sh = ob->short()))
       {
	 write(capitalize(sh));
	 if (ob->query_wielded())
	   write(" (wielded)");
	 if (magobj && magobj->check_bound(ob))
	   write(" (bounded)");
	  write(".\n");
	}
	ob = next_inventory(ob);
	max--;
      }
    }
    return 1;
  }
  write("Look AT something, or what?\n");
  return 1;
}

static check_access() {
  string ip;
  string a,b,c,d;

  ip = query_ip_number();

  if(!allow_from_host)
    return 1;
  if(member_array(ip,allow_from_host) != -1)
    return 1;
  sscanf(ip,"%s.%s.%s.%s",a,b,c,d);
  if(member_array(a+"."+b+"."+c+".*",allow_from_host) != -1)
    return 1;
  if(member_array(a+"."+b+".*.*",allow_from_host) != -1)
    return 1;
  if(member_array(a+".*.*.*",allow_from_host) != -1)
    return 1;
  if(member_array("*.*.*.*",allow_from_host) != -1)
    return 1;
  return 0;
}

static add_access(str) {
  int i;

  if(!str) {
    if(allow_from_host) {
      write("Login allowed from:\n");
      for(i = 0; i < sizeof(allow_from_host); i++)
	write(allow_from_host[i]+"\n");
    } else {
      write("All hosts allowed.\n");
    }
    return 1;
  }
  if(allow_from_host) {
    if(member_array(str,allow_from_host) == -1)
      allow_from_host += ({ str });
  } else 
      allow_from_host = ({ str });
  if(member_array(query_ip_number(),allow_from_host) == -1)
    allow_from_host += ({ query_ip_number() });
  write("Ok.\n");
  return 1;
}

static rem_access_mapper(elm,host) {
  if(elm == host)
    return 0;
  else
    return 1;
}

static rem_access(str) {
  int pos;

  if(!str)
    return 0;
  if(str == "all")
    allow_from_host = 0;
  else {
    if(allow_from_host) 
      allow_from_host = filter_array(allow_from_host,"rem_access_mapper",
				     this_object(),str);
    if(sizeof(allow_from_host) == 0)
      allow_from_host = 0;
  }
  write("Ok.\n");
  return 1;
}

static set_use_rlogin() {
  if(use_rlogin) {
    write("Rlogin turned off.\n");
    use_rlogin = 0;
    save_me(1);
  } else {
    write("Rlogin turned on.\n");
    use_rlogin = 1;
    save_me(1);
  }
  return 1;
}

static check_password(p)
{
/*    object magobj;*/
    remove_call_out("time_out");
    write("\n");		/* Pell */
    if (password == 0)
        write("You have no password ! Set it with the 'password' cmd.\n");
    else if (name != "guest" && crypt(p, password) != password) {
	write("Wrong password!\n");
	name = 0;
	cap_name = 0;
	level = -1;
	destruct(this_object());
	return;
    }

    move_player_to_start(0);
#ifdef LOG_ENTER
    log_file("ENTER", name + ", " + ctime(time())[ 4..15]+ 
	     " from "  + (ident_user ? ident_user + "@" : "") +
	       query_ip_name() + "=" + query_ip_number() + ".\n");
#endif
/*
    if(this_object())
    {
      magobj=clone_object("obj/magobj");
      move_object(magobj,this_object());
    }
*/
}
/*
 * Give a new password to a player.
 */
static new_password(p)
{
/*    object magobj;*/
    object adv_guild;
    object booklet;

    write("\n");		/* Pell */
    if (!p || p == "") {
	write("Try again another time then.\n");
	destruct(this_object());
	return;
    }
    if (level <= 22 && strlen(p) < 6) {
	write("The password must have at least 6 characters.\n");
	input_to("new_password", 1);
	write("Password: ");
	return;
    }
    if (password == 0) {
	password = p;
	input_to("new_password", 1);
	write("Password: (again) ");
	return;
    }
    if(query_bad_password(p))
    {
      password=0;
      write("This password is not safe, pick another one.\n");
      input_to("new_password", 1);
      write("Password: ");
      return;
    }
    remove_call_out("time_out");
    if (password != p) {
	write("You changed !\n");
	destruct(this_object());
	return;
    }
    password = crypt(password, 0);
    level = 0;
    "room/adv_guild"-> advance(0);
    level = 1; /* can't use set_level if level < 1... Wing 920503 */
    set_str(1); set_con(1); set_int(1); set_dex(1);
    hit_point = max_hp;
    my_quest_max = "room/quest_room"-> count_all();

    move_player_to_start(0);
#ifdef LOG_NEWPLAYER

    log_file("NEWPLAYER", cap_name + " " + ctime(time()) + 
	     " from " + (ident_user  ? ident_user +"@" : "") +
	     query_ip_name()+ "=" + query_ip_number() + ", " +
	     my_quest_max +  "\n");
#endif
/*    magobj=clone_object("obj/magobj");
    move_object(magobj,this_object());*/


/* This one gives the adv_guilds guildmark to newbies.
   /Taren 950221

   No longer. /Profezzorn 950611
   adv_guild=clone_object("/guilds/adv_guild/adv_guild");
   transfer(adv_guild,this_object());
   adv_guild->join();
*/

/* This one gives the newbie booklet to newbies.
   /Celeborn 950912
*/

   booklet=clone_object("/players/celeborn/guide/booklet");
   transfer(booklet, this_object());


}


static move_player_to_start(where) {
  if (!mailaddr || mailaddr == "")
  {
    write("Please enter your email address (or 'none'): ");
    saved_where = where;
    input_to("getmailaddr");
    return;
  }
  
  move_player_to_start2(where);
}

query_mailaddr() {
  return mailaddr;
}


static getmailaddr(maddr) {
  mailaddr = maddr;

  /* Guest should not have a strange email address. 
   * Brom 960322
   */
  if (stringp(name) && name == "guest")
    mailaddr = "guest@nannymud.lysator.liu.se";

  move_player_to_start2(saved_where);
}

static move_player_to_start2(where) {
  if (gender == -1) {
    write("Are you, male, female or other: ");
    input_to("getgender", where);
    return;
  }
  move_player_to_start3(where);
}

/*  This function is called using input_to, and sets the
 *  gender of this player.
 */
static getgender(gender_string) {

  gender_string = lower_case(gender_string);
  if (gender_string[0] == 'm') {
      write("Welcome, Sir!\n");
      gender = MALE_GENDER;
  }
  else if (gender_string[0] == 'f') {
      write("Welcome, Madam!\n");
      gender = FEMALE_GENDER;
  }
  else if (gender_string[0] == 'o') {
      write("Welcome, Creature!\n");
      gender = NEUTER_GENDER;
  }
  else {
      write("Sorry, but that is too weird for this game!\n");
      write("Are you, male, female or other (type m, f or o): ");
      input_to("getgender");
      return;
  }
#ifdef USE_PK_FLAG  
  write("Be sure to do 'help playerkilling' when you login.\n");
#endif
  move_player_to_start3(saved_where);
}

static int login_ok;
void login_error_check()
{
  if(!login_ok)
  {
    write("An error occured during login.\n");
    catch(destruct(this_object()));
    if(this_object())
    {
      level=99;
      catch(_destruct(this_object()));
      if(this_object())
      {
	name="......."; /* prevent saves the hard way /Profezzorn */
	_disconnect(this_object());
      }
      level=0;
    }
  }
}

static move_player_to_start3(where)
{
  object ob;
  status new_news,new_wiz_news;
  login_ok=0;
  call_out("login_error_check",0);
  /*
   * See if we are already playing.
   * We must blank our own name, or we could find ourselves !
   */
  xx_realname = name;
  name = 0;

  other_copy = find_player(xx_realname); /* Find playing player */

  if (!other_copy)
    other_copy = "/obj/daemon/whod"->find_player_or_statue(xx_realname);

  if (other_copy)
  {
    while (other_copy && other_copy->query_npc())
    {
      /* Got a monster with that name! - a wizard has fucked up,
	 or LPMUD has.. */
      /* As it works now, we shouldn't find any monsters, because
       * find_player_or_statue only returns objects that has once been
       * interactive. However, if it _does_ return a monster - zap it.
       * /Profezzorn  */

      while(first_inventory(other_copy))
	destruct(first_inventory(other_copy));
      destruct(other_copy);
      other_copy = "/obj/daemon/whod"->find_player_or_statue(xx_realname);
    }

    if (other_copy)
    {
      if(query_ip_number(other_copy)) { /*Added by Qqqq*/
	write("You are already playing!\n");
	write("Throw the other copy out? ");
	input_to("try_throw_out");
	name = xx_realname;
	remove_call_out("login_error_check");
	return;
      } else {
	name = xx_realname;
	write("You are already playing, I will remove your statue.\n");
	try_throw_out("yes");
	return;
      } 
    }
  }
  name = xx_realname;
  restore_object("players/" + name);

  /* Needed because of bug somewhere /Profezzorn */
  if(mmsgin == "disappears in a puff of smoke" &&
     mmsgout == "arrives in a puff of smoke")
  {
    /*
       I changed from mmsgin="dis.." to msgout="dis.."
       /Taren
     */
    mmsgout="disappears in a puff of smoke";
    mmsgin="arrives in a puff of smoke";
  }
  if(!toggles)
    toggles = (["wimpy" : whimpy, "brief" : 0, "abrief" : 0 ]);
  new_news = (last_newsread < _file_stat("/etc/NEWS")[3]);
  new_wiz_news = (last_newsread < _file_stat("/etc/NEWS")[3]);
  last_login_time = CURRENT_DATE;
  last_login_time_int = time();

  /* Save chars only if they have XP or money. This is handled in
   * save_me(). */
  /* save_object("players/" + name); */
  
  /* enable_commands(); */
  set_living_name(name);
  
  catch("/obj/daemon/whod"->register_player());

  max_hp = 42 + Con * 8+(paragon?paragon_level*2:0); /*temporary*/
  if(hit_point>max_hp)		/*temporary*/
    hit_point=max_hp;		/*temporary*/

  max_sp = 42 + Int * 8+(paragon?paragon_level*2:0); /*temporary*/
  if(spell_points>max_sp)	/*temporary*/
    spell_points=max_sp;	/*temporary*/

  /* Either this, or the patch in shout_to_all */
#ifdef NO_LVL_1_SHOUT
  if (level == 1)
    spell_points = -10;
#endif

  /*
   * Now we can enter the game. Check tot_value if the game
   * crashed, and the values of the player was saved.
   */
  slow_heart_beat();
  add_standard_commands();

  add_call_back(TITLE, this_object(), "fix_title");

  move_object(clone_object("obj/soul"), this_object());
  if (level >= 20)
    wiz_setup();
  
  if (tot_value) {
    write("You find " + tot_value + " coins of your lost money!\n");
    add_money(tot_value);
    tot_value = 0;
  }
  
  write("\n");
  if (view_news)		/* Do you want to read the news? */
  { 
    last_newsread=time();
    cat("/etc/NEWS");
    write("\n");
  }
  else 
    if (new_news)
      write ("There is NEW general news available.  Use News to read.\n");
    else
      write ("There is general news available.  Use News to read.\n");

  if(level > 22)		/* Milamber */
    if(!getenv("read:arch"))
      setenv("read:arch",({ 0 }));

  if(level > 19)		/* Milamber */
  {
    if(!getenv("read:wizboard") &&!getenv("Wizboards"))
    {
      setenv("Wizboards", ({}));
      setenv("read:RULES",({ 0 }));
      setenv("read:LPC",({ 0 }));
      setenv("read:wizboard",({ 0 }));
      setenv("read:approval",({ 0 }));
      setenv("read:projects",({ 0 }));

    }
  } else {
    setenv("Wizboards", ({}));
  }
  if(!getenv("read:boards") && !getenv("BoarBoards")) /* Milamber */
  {
    setenv("BoarBoards", ({}));
    setenv("read:boards",({ 0  }));
  } else {
    setenv("BoarBoards", ({}));
  }
    
  if(!getenv("read:advguild") &&!getenv("Mortalboards")) /* Milamber */
  {
    setenv("Mortalboards", ({}));
    setenv("read:advguild",({ 0 }));
    setenv("read:gossip",({ 0 }));
    setenv("read:flameboard",({ 0 }));
    setenv("read:MUDboard",({ 0 }));
  } else {
    setenv("Mortalboards", ({}));
  }

  if(name=="guest")		/* Profezzorn */
  {
    prompt="> ";
  }

  query_quest_points();		/* Milamber */
  update_prompt();		/* Milamber */

  if(where)
  {
    move_object(this_object(), where);
  }else {
    if(level > 19)
    {
      where = getenv(START);
      if(where) 
	if(!find_object(where))
	  if(catch(call_other(where, "????")))
	    where = 0;
    }
    if(!where) 
      where = "room/church";
    move_object(this_object(), where);
    load_auto_obj(auto_load);
  }
  login_ok=1;
  if (query_invis() && level < 20)
    vis();
  
  /*  Notify mudfinger of the login   /Gwendolyn 930513  */
  if(!query_invis() || level <= 22)
    catch("/obj/daemon/mudwhod"->send_login(this_object()));
  
  
  /*  To decide the number of quest points *this* player will have to collect.
   *  Done here, so it is set for old players too, if they haven't had it set.
   */
  if (my_quest_max == 0)
    my_quest_max = "room/quest_room"-> count_all();
  
  if (!is_linkdead && !query_invis())
  {
    if (ghost)
      say("Some mist enters the game.\n");
    else
      say(cap_name + " enters the game.\n");
  }
  else
    if (query_invis())
      write("YOU ARE INVISIBLE !\n\n");

  if (level >= 21)
    if (view_news)		/* Do we want to read the news? */
    {
      cat("/etc/WIZNEWS");
      write("\n");
    }
    else 
      if (new_wiz_news)
        write ("There is NEW Wiznews to read.  Use 'News'.\n");
      else
        write ("There is Wiznews to read.  Use 'News'.\n");
  catch("room/post"-> query_mail(0));
  host = 0;
  if (!called_from_host)
    called_from_host = called_from_ip;
  if (query_ip_number(this_object()) != called_from_ip && called_from_ip ||
      _query_ident(this_object()) != called_from_user && called_from_user)
  {
    if (called_from_user)
      write("Your last login was from " + called_from_user + "@" + 
	    called_from_host + "\n");
    else
      write("Your last login was from " + called_from_host + "\n");
  }
  called_from_user = _query_ident(this_object());
  called_from_ip = query_ip_number(this_object());
  called_from_host = query_host();
  
  ob = first_inventory(environment());
  while(ob) {
    if (ob != this_object()) {
      string sh;
      sh = ob-> short();
      if (sh && stringp(sh))
	write(capitalize(sh) + ".\n");
    }
    ob = next_inventory(ob);
  }
  add_call_back(TITLE, this_object(), "fix_title");
  if(!(sh = getenv(TITLE)))
    sh = title;
  setenv(TITLE, sh);
  metaboard::reset();		/* Milamber */
}


status help(string what)
{
  string fid;

  if (!stringp(what))
  {
    this_player() -> simplemore(read_file("doc/help"));
    this_player() -> flush();
    return 1;
  }

  if(what == "wizard" && level >= 20)
  {
    this_player() -> simple_more(read_file("/doc/wiz_help"));
    this_player() -> flush();
    return 1;
  }
  if (sscanf(what, "%*s.%*s") == 2 ||
      sscanf(what, "%*s/%*s") == 2)
  {
    /* 950501 Saw a player using 'help ../../obj/weapon.c'. Even though
     * it truncated, it was no pretty sight. 
     * Brom.
     */
    notify_fail("No help available on'" + what + "'.\n");
    return 0;
  }

  what = replace(what, " ", "_");
  fid = "doc/helpdir/" + what;
  if (file_size(fid) == -1)
  {
    notify_fail("No help available on '" + what + "'.\n");
    return 0;
  }
  this_player() -> simplemore(read_file(fid));
  this_player() -> flush();
  return 1;
} /* help */


tell(string str)
{
  string *told,*statues,*editing,*not_on;
  object ob;
  string who, msg,*mwho,time_str,away;
  int e;

  if(this_player() != this_object())
    return 0;

  if(ghost)
    return notify_fail("You fail.\n"), 0;

  if(spell_points < 5 && query_level() < 20)
    return notify_fail("You are low on power.\n"), 0;

  if(!str || sscanf(str, "%s %s", who, msg) != 2)
    return notify_fail("Tell whom what?\n"), 0;

  mwho=explode(lower_case(who),",") || ({ lower_case(who)});
  time_str = ctime(time())[11..15];
  told=statues=editing=not_on=({});

  for(e=0;e<sizeof(mwho);e++)
  {
    ob=find_player(mwho[e]) || find_living(mwho[e]);
    who=capitalize(mwho[e]);
    if(!ob || 
       (ob->is_hard_invis() && this_player(1)->query_level()<22))
    {
      not_on+=({who});
      continue;
    }
    if(sscanf(file_name(ob), "obj/player#%*d") && !interactive(ob))
    {
      statues+=({who});
      continue;
    }
    if(interactive(ob) && _query_ed(ob) && ob->query_toggles("edblock"))
    {
      editing+=({who});
      if(level<22) continue;
    }

    if(interactive(ob))
    {
      switch(query_idle(ob))
      {
      case 3601..18000:
	write(who+" is very very idle.\n");
	break;
      case 1201..3600:
	write(who+" is very idle.\n");
	break;
      case 601..1200:
	write(who+" has been idle for sometime.\n");
	break;
      case 300..600:
	write(who+" is idle.\n");
	break;
      case -1000..299:
	break;
      default:
	write(who+" is so idle it is very likely "+ob->query_pronoun()+" is dead in RL.\n"); /* Typo fixed. Banshee 951119 */
	break;
      }
    }

    away=(string)ob->query_away(1);

    if(away)
      write(line_break(away + "\n", who + " is away: "));

    sendmsg(ob, ((query_invis() && level < ob->query_level())?
		 capitalize(name):cap_name),
	      "tells you: " + msg + (away ? " <"+time_str+">" : "") );
    told+=({who});
  }
  if(sizeof(not_on))
  {
    write("There "+(sizeof(not_on)==1?"is":"are")+" no living being"+
	  (sizeof(not_on)==1?"":"s")+" called "+
	  implode_nicely(not_on,"or")+".\n");
  }
  if(sizeof(statues))
  {
    write(implode_nicely(statues)+
	  (sizeof(statues)==1?
	   " has lost "+(string)find_living(lower_case(statues[0]))->query_possessive():
	   " have lost their")+
	  " connection with reality.\n");
  }
  if(sizeof(editing))
  {
    write(implode_nicely(editing)+" "+(sizeof(editing)==1?"is":"are")+" editing right now.\n");
    if(level>21)
      write("But your tell got through anyway because you're level "+level+".\n");
  }
  if(level < 20)
    add_sp(sizeof(told)*-5);
  if(sizeof(told))
  {
    if(toggles["tell"])
    {
      write(line_break(implode_nicely(told)+": " +  msg, "You told "));
    }else{
      write("Ok.\n");
    }
  }
  return 1;
}

query_last_tell_from() {
  return last_tell_from;
}

int reply_to_last(string msg)
{
  string time_str, away, who;
  object ob;
  if(this_player() != this_object())
    return 0;
  if(ghost)
    return notify_fail("You fail.\n"), 0;
  if(spell_points < 0 && query_level() < 20)
    return notify_fail("You are low on power.\n"), 0;  
  if(!last_tell_from || !(ob = find_living(last_tell_from)))
    return notify_fail("No one to reply to.\n"), 0;
  
  if(!msg || msg == "")
    return notify_fail("Reply will reply to: "+
		       capitalize(last_tell_from)+".\n"), 0;
  who = capitalize(last_tell_from);
  if(sscanf(file_name(ob), "obj/player#%*d") && !interactive(ob))
    return notify_fail(who +" has lost "+ (string) ob->query_possessive() 
		       +" connection with reality.\n"), 0;
  /*  Don't disturb poor wizards in ed 
      ...23++'s should be able to through....  CEL 200494
      */
  if(interactive(ob) && _query_ed(ob) && ob->query_toggles("edblock"))
  {
    if(level<22)
      return notify_fail("That wizard is editing right now.\n"), 0;
    else
      write("That wizard is editing now, but since you are 22++, "+
	    "your tell gets through.\n");
  }
  if(away = (string)ob -> query_away(1))
    time_str = ctime(time())[11..15];
  
  /* If we are invisible, and talking to a wizard who is
     of higher level than we, then give him our real name */

  if(query_invis() && ((int) ob->query_level() > level))
    sendmsg(ob, capitalize(name) ,"replies to you: " + msg + 
	    (away ? " <"+time_str+">":"")+"\n");
  else
    sendmsg(ob, cap_name , "replies to you: " +msg + 
	    (away ? " <"+time_str+">":"")+"\n");
  
  if(level < 20)		/* Why make it cost for wizards? */
    add_sp(-5);    
  
  if(away)
    tell_object(this_object(), line_break(away +"\n", who +" is away: "));
  if(toggles["tell"])
    return write(line_break(msg +"\n", "You reply to "+ who +": ")), 1;
  return write("Ok.\n"), 1;
}

int away(string s)
{
  if(!stringp(s) || s == "")
    return write(line_break(away_string ? 
			    "Current away string: "+ away_string :
			    "You have no away string.\n")), 1;
  if(s == "-c")
  {
    away_string = 0;
    write("Away string is cleared.\n");
  } else {
    write(line_break("You set your away string to: "+ s));
    away_string = s;
  }
  return 1;
}

string query_away(int i) 
{ 
  if(i)
    last_tell_from = (string)this_player() -> query_real_name();
  return away_string; 
}

whisper(str)
{
  object ob;
  string who;
  string msg;
    
  if(this_player() != this_object())
    return 0;
  
  if(ghost) 
    return notify_fail("You fail.\n"), 0;
  
  if(!str || sscanf(str, "%s %s", who, msg) != 2) 
    return notify_fail("Whisper what?\n"), 0;

  ob=present(lower_case(who),environment());
  if(!ob || (!interactive(ob) && !ob->query_npc()))
    return notify_fail("No player with that name in this room.\n"), 0;
  
  if(ob == this_object())
  {
    say(cap_name +" whispers secretly to "+this_player()->query_objective()+
	"self.\n");
    return write("You whisper to yourself about life, "+
		 "the universe and everything.\n"), 1;
  }
  
  tell_object(ob, cap_name + " whispers to you: " + msg + "\n");
  if(toggles["whisper"])
    write("You whisper '"+ msg +"' to "+ capitalize(who) +"\n");
    else
      write("Ok.\n");
  say(cap_name + " whispers something to " + capitalize(who) + ".\n", ob);
  return 1;
}


query_ld() { return is_linkdead; }
/*
 * This routine is called from other routines to drop one specified object.
 * We return true if success.
 */

drop_all(verbose)
{
  object ob;
  object next_ob;
  if (!this_object() || !living(this_object()))
    return;

  if(verbose)
  {
    object *o;
    if(sizeof(o=map_array(get_dropable_objects("all", ({ this_object() })), 
			"drop_obj", this_object())-({0})))
    describe_objects(o,"drop");
 }
 else
 {
    for(ob = first_inventory(this_object()); ob; ob = next_ob)
    {
      next_ob = next_inventory(ob);
      if(!ob->drop(1) && ob)   /* *sigh* /Gwendolyn */
        move_object(ob,environment(this_object()));
    }
    count_weight(); /* Fix the weight... */
  }
}

/*
 * This one is called when the player wants to change his password.
 */
static change_password(str)
{
    if (password != 0 && !str) {
	write("Give old password as an argument.\n");
	return 1;
    }
    if (password != 0 && password != crypt(str, password)) {
	write("Wrong old password.\n");
	return 1;
    }
    password2 = 0;
    input_to("change_password2", 1);
    write("New password: ");
    return 1;
}

static change_password2(str)
{
    if (!str)
    {
      write("Password not changed.\n");
      return;
    }
    if(level <= 22 && strlen(str)<6)
    {
      write("Password must have at least 6 characters.\n");
      return;
    }

    if(query_bad_password(str))
    {
      password2=0;
      write("This password is not safe, pick another one.\n");
      return 1;
    }
    
    if (password2 == 0) {
      password2 = str;
      input_to("change_password2", 1);
      write("Again: ");
      return;
    }
    if (password2 != str) {
      write("Wrong! Password not changed.\n");
      return;
    }
    password = crypt(password2, 0);
    password2 = 0;
    write("Password changed.\n");
}


static smart_report(string str,object where)
{
  string who,which;
  string current_room;
  string what, rest;

  current_room = file_name(where);

  sscanf(str,"%s %s",what,rest);

  if(str[-1]!='\n') str+="\n";

  log_file(what, current_room+" "+CURRENT_DATE+" "+rest+"\n");
  if(!sscanf(current_room, "players/%s/", who))
  {
    if(sscanf(current_room, "guilds/%s/",which) == 1) /*Added by Taren 941111 */
    {
      write_file("/guilds/log/"+which+".rep",current_room+" "+CURRENT_DATE+" "+str);
      return;
    }
  }
  if(!who) who=creator(where);

  if(!who)
    log_file("ROOM.rep", current_room + " " +CURRENT_DATE+" " + str);
  else
    log_file(who + ".rep", current_room + " "+CURRENT_DATE+" " + str);
}

static typbugpra(str)
{
  if (!str) 
    return notify_fail("Give an argument.\n"), 0;

  smart_report(capitalize(query_verb()) +" "+ query_real_name() + "\n" + str,environment());
  write("Ok.\n");
  return 1;
}

static string report_string;

int report(string what)
{
  object o;
  string in;

  if(!what)
  {
    write("Please use: 'report typo|bug|praise|idea [in object]'\n");
    write("If no object is given, the place you are in will receive the report.\n");
    return 1;
  }

  if(sscanf(what,"%s in %s",what,in))
  {
    o=present(in,this_player()) || present(in,environment());
    if(!o)
    {
      write("There is no such object here.\n");
      return 1;
    }
  }else{
    o=environment();
  }
  what=upper_case(what);
  switch(what)
  {
  default:
    write("You can only report, typos, ideas, bugs or praises.\n");
    return 1;

  case "TYPO":
  case "BUG":
  case "IDEA":
  case "PRAISE":
  }

  report_string=file_name(o)+" "+what+" "+query_real_name()+"\n";
  write("Please insert full description of the report, end with '.' or '**':\n");
  write(". ");
  input_to("receive_report");
  return 1;
}

void receive_report(string s)
{
  string file;
  if(s!="." && s!="**")
  {
    report_string+=s+"\n";
    write(". ");
    input_to("receive_report");
    return;
  }

  sscanf(report_string,"%s %s",file,report_string);
  if(!find_object(file))
  {
    write("Object dissappeard, report failed, sorry.\n");
  }else{
    smart_report(report_string,find_object(file));
  }
  report_string=0;
}

static converse()
{
  write("Give '**' to stop.\n");
  write("]");
  input_to("converse_more");
  return 1;
}

static converse_more(str)
{
  string cmd;
  if (str == "**") 
  {
    write("Ok.\n");
    return;
  }
  say(cap_name + " says: " + str + "\n");
  write("]");
  input_to("converse_more");
}

#define ON(x) (toggles[x] ? "on" : "off")

static write_echo(string str)
{
  write("Echo on "+ str +"s is now "+ ON(str) +".\n");
}

/* grep for and inline */
int write_toggle_status()
{
  write(sprintf(" All toggles:\n"+
		"-----------------------------------------------------------\n"+
		" Tell echo  : %-3s                Shout echo    : %-3s\n"+  
		" Say echo   : %-3s                Whisper echo  : %-3s\n"+
		" Wimpy mode : %-6s             Brief mode    : %-3s\n"+
		" Hpinfo     : %-3s\n"+
		" Obvious exits in short description: %-3s\n"+
		" Autosave hide mode: %-3s\n"+
		" Show shape info: %-3s\n"+
                ((this_player()->query_level() < 20) ? ("") :
		 (" Ed Block Tell : %-3s\n")),
		ON("tell"),
		ON("shout"),
		ON("say"),
		ON("whisper"), 
		(toggles["wimpy"] != 1 ? toggles["wimpy"] : max_hp / 5) +" hp",
		ON("brief"),
		ON("hpinfo"),
		ON("oshort"),
		ON("abrief"),
		ON("shape"),
		ON("edblock")
		));
  return 1;
}

varargs int toggle(string str, int value)
{
  log_file("FIX", "toggle: "+file_name(previous_object())+"\n");
  return this_object()->toggle_all(str,value);
}

varargs int toggle_all(string str, int value)
{
  if(query_verb() != "toggle")
  {
    value = str ? atoi(str) : 0;
    str = query_verb();
  }
  if(!str || str == "")
    return write_toggle_status();
  if(!toggles)
    toggles = (["wimpy" : 1, "brief" : 0, "abrief" : 0 ]);
  if(sscanf(str, "wimpy %d", value))
    str = "wimpy";
  switch(lower_case(str))
  {
   case "on":
   case "all on":
   case "on all":
    toggles["say" ] = 1;
    toggles["shout" ] = 1;
    toggles["whisper" ] = 1;
    toggles["tell" ] = 1;
    write("All echoes turned on.\n");
    break;
    
   case "off":
   case "all off":
   case "off all":
    while(toggles != (toggles = m_delete(toggles, "say")));
    while(toggles != (toggles = m_delete(toggles, "tell")));
    while(toggles != (toggles = m_delete(toggles, "shout")));
    while(toggles != (toggles = m_delete(toggles, "whisper")));
    /* This should fix the bug that was before... */
    write("All echoes turned off.\n");
    break;
    
   case "say":
   case "says":
    toggles["say"] = !toggles["say"];
    write_echo("say");
    break;

   case "whisper":
   case "whispers":
    toggles["whisper"] = !toggles["whisper"];
    write_echo("whisper");
    break;

   case "tell":
   case "tells":
    toggles["tell"] = !toggles["tell"];
    write_echo("tell");
    break;

   case "shout":
   case "shouts":
    toggles["shout"] = !toggles["shout"];
    write_echo("shout");
    break;

   case "hpinfo":
    toggles[str] = !toggles[str];
    write("HP/SP info turned "+ ON(str) +".\n"), 1;
    break;

   case "wimpy":
    toggles[str] = value ? (value > max_hp ? max_hp : value) : !toggles[str];
    write(toggles[str] ? ("Wimpy mode ("+ 
			   (toggles["wimpy"] == 1 ?  
			    max_hp / 5 : toggles["wimpy"]) +" / "+ 
			  max_hp +" hp).\n") : "Brave mode.\n");
    break;

   case "brief":
    toggles[str] = !toggles[str];
    write((toggles[str] ? "Brief" : "Verbose") +" mode.\n");
    break;
    
   case "abrief":
    toggles[str] = !toggles[str];
    write((toggles[str] ? "Autosave hide" : "Autosave verbose") +" mode.\n");
    break;
    
   case "oshort": 
    toggles[str] = !toggles[str];
    write("Obvious exits in short is "+ ON(str) +".\n");
    break;

   case "shape": 
    toggles["shape"] = !toggles["shape"];
    write("Show shape is turned "+ ON(str) +".\n");
    break;

   case "edblock": /* Must be before default: */
    if (this_player()->query_level() > 19) {
      toggles["edblock"] = !toggles["edblock"];
      write("Blocking tells when in ed is turned "+ ON(str) +".\n");
    break;
    }    
   default:
    notify_fail("Toggle what?\n");
    return 0;
  }
  return 1;
}
#undef ON

query_brief() { return toggles["brief"]; }
query_toggles(string str) { return str ? toggles [str] : toggles; }

int query_wimpy()
{
  return toggles["wimpy"];
}

add_money(m)     
{
  hook("changed_money_hook",({money+m,money,m}));
  money += m;
  update_prompt();
#ifdef PARNOIA
  if(this_player(1)!=this_object())
  {
    if(this_player(1)&& level < 20 && this_player(1)->query_level()==21)
      write_file("log/SECURE/EXPERIENCE.money", name + " $"+m+" by "+
	       this_player(1)->query_real_name()+"(WIZ) ["+query_verb()+
	       "] at " + ctime(time()) + "\n");
    else if(m>1000 && this_player(1))
      write_file("log/SECURE/EXPERIENCE.money", name + " $"+m+" by "+
	       this_player(1)->query_real_name()+"("+
	       this_player(1)->query_level()+") ["+query_verb()+"] at "+
	       ctime(time()) + "\n");
    else if(m>1000)
      write_file("log/SECURE/EXPERIENCE.money", name + " $"+m+" by "+
	       filename(previous_object())+" ["+
	       previous_object()->query_name()+"] ["+query_verb()+"] at "+
	       ctime(time()) + "\n");
  }
#endif
  if (level <= 19 && !is_npc)
    add_worth(m);
}

add_exp(e) 
{
#ifdef LOG_EXP
  if (this_player() && ((this_player() != this_object() &&
			 query_ip_number(this_player()) && level < 20 && 
			 (e >= ROOM_EXP_LIMIT))))
    write_file("log/SECURE/EXPERIENCE", ctime(time()) + " " + name + "(" + level + 
	       ") " + e + " exp by " + this_player()->query_real_name() + 
	       "(" + this_player()->query_level() + ")" +"\n"+
	       file_name(previous_object())+"\n");
#endif
  if (e>0 && level < 4 && experience > (level*1000+500))
    tell_object(this_player(),"You must advance your level to gain more experience points.\n");
  else
  {
    hook("changed_exp_hook",({experience+e,experience,e}));
    experience += e;
  }
  if (level <= 19)
    add_worth(e);
  
  update_prompt();
} /* add_exp */

sober() 
{
  intoxicated = soaked = stuffed = 0; 
  update_prompt();
}

add_intoxination(i)
{
  hook("changed_intoxination_hook",({intoxicated+i,intoxicated,i}));
  intoxicated += i;
  if(intoxicated < 0)
    intoxicated = 0;
  update_prompt();
}

query_intoxination() {  return intoxicated; }
query_stuffed() { return stuffed; }
query_soaked() { return soaked; }
 
  
add_stuffed(i)
{
  hook("changed_stuffed_hook",({stuffed+i,stuffed,i}));
  stuffed += i;
  if (stuffed < 0)
    stuffed = 0;
  update_prompt();
}
 
add_soaked(i)
{
  hook("changed_soaked_hook",({soaked+i,soaked,i}));
  soaked += i;
  if (soaked < 0)
    soaked = 0;
  update_prompt();
}
 

second_life() 
{
  string tmp,tmp2;

  if (level >= 20)
    return illegal_patch("second_life");
#ifdef USE_SCAR
  make_scar();
#endif
  ghost = 1;
  if (attacker_ob)
  {		/* Pell */
    tmp = creator(attacker_ob);
    tmp2 = attacker_ob->query_name();
    if (!tmp) {
      if (attacker_ob->query_npc())
	tmp = "npc";
      else {	/* Anvil 910606 Invisible wizards was logged as Someone */
	tmp2 = capitalize(attacker_ob->query_real_name());
	tmp = "player";
      }
    }
  }else {
    tmp = "none";
  }
#ifdef LOG_KILLS
  log_file("KILLED", cap_name + " (" + level + ") " +
	   ", by " + tmp2 + " ("+ tmp +") "+ CURRENT_DATE+ ".\n");
#endif
  if(paragon&&paragon_level>0)
  {
    set_paragon_level(paragon_level--);
  }else{
    if (level > 1) level--;
  }
  if (Str > 1) set_str(Str-1);
  if (Con > 1) set_con(Con-1);
  if (Dex > 1) set_dex(Dex-1);
  if (Int > 1) set_int(Int-1);
  msgin = "drifts around";
  msgout = "blows";
  headache = 0;
  intoxicated = 0;
  stuffed = 0;
  soaked = 0;
  hunter = 0;
  hunted = 0;
  attacker_ob = 0;
  alt_attacker_ob = 0;
  tell_object(this_object(), "\nYou die.\nYou have a strange feeling.\n" +
	      "You can see your own dead body from above.\n\n");
  say(cap_name + " died.\n");	/* Mats 920317 */

  save_me(0);
  return 1;
}

remove_ghost() 
{
  if (!ghost)
    return 0;
  write("You feel a very strong force.\n");
  write("You are sucked away...\n");
  write("You reappear in a more solid form.\n");
  say("Some mist disappears.\n");
  say(cap_name + " appears in a solid form.\n");
  ghost = 0;
  fake_ghost = 0;		/* Special for Ramses guild..sigh..  */
  dead = 0;
  msgin = "arrives";
  msgout = "leaves";
  add_alignment(-alignment/2);
  save_me(1);
  return 1;
}

stop_hunting_mode()
{
  if (!hunted) 
    return notify_fail("You are not hunting anyone.\n"),0;
  hunted-> stop_hunter();
  hunted = 0;
  write("Ok.\n");
  return 1;
}

drink_alcohol(strength, int flag)
{
  if(hook("block_drink_alcohol_hook",({previous_object(),strength,flag})))
    return;
  
  if (flag ? intoxicated > level + 3 : intoxicated + strength > level * 3)
    return write("You fail to reach the drink with your mouth.\n"),0;

  intoxicated += strength / ( flag ? 2 : 1);
  if (intoxicated < 0)
    intoxicated = 0;
  if (intoxicated == 0)
    write("You are completely sober.\n");
  if (intoxicated > 0 && headache) {
    headache = 0;
    tell_object(this_object(), "Your headache disappears.\n");
  }
  if (intoxicated > max_headache)
    max_headache = intoxicated;
  if (max_headache > 8)
    max_headache = 8;
  update_prompt();
# ifdef HLD
  catch(HLD -> logs(this_player(), this_player(1), this_object(),
	      previous_object(), strength, 0));
# endif
  hook("drink_alcohol",({previous_object(),strength,flag}));
  return 1;
}


 
drink_alco(strength) { return drink_alcohol(strength,1); } /* Really.. 8b */

drink_soft(strength)
{
  if(hook("block_drink_soft_hook",({previous_object(),strength})))
    return;
  if (soaked + strength > ((level + Str + Con) * 8) / 3)
  {
    write("You can't possibly drink that much right now!\n" + 
	  "You feel crosslegged enough as it is.\n");
    return 0;
  }
 
  soaked += strength * 2;
 
  if (soaked < 0)
    soaked = 0;
 
  if (soaked == 0)
    write("Your mouth feels a bit dry.\n");
  update_prompt();
# ifdef HLD
  catch(HLD -> logs(this_player(), this_player(1), this_object(),
		    previous_object(), strength, 0));
# endif
  hook("drink_soft_hook",({previous_object(),strength}));
  return 1;
}

eat_food(strength)
{
  if(hook("block_eat_food_hook",({previous_object(),strength})))
    return;
  if (stuffed + strength > level * 8)
  {
    write("This is much too rich for you right now!\n");
    return 0;
  }
  
  stuffed += strength * 2;
  
  if (stuffed < 0)
    stuffed = 0;
  
  if (stuffed == 0)
    write("Your stomach makes a rumbling sound.\n");
 
  if(stuffed > level * 8)
    write("You feel extremely stuffed.\n");
  update_prompt();
# ifdef HLD
  catch(HLD -> logs(this_player(), this_player(1), this_object(),
		    previous_object(), strength, 0));
# endif
  hook("eat_food_hook",({previous_object(),strength}));
  return 1;
}

spell_std(str)
{
  int tmp;
  object ob, e;  
  if (test_dark()) return 1;

  /* no_magic check added by Brom 941124. */
  e = environment(this_player());
  if (e)
    if ((e->query_property("no_magic")) || e->realm()=="no_magic")
      {
	write("Your spell fizzle and fail.\n");
	return 1;
      }
  
  switch(query_verb())
  {
    case "missile": tmp=5; break;
    case "shock":    tmp=10; break;
    case "fireball": tmp=15;
  }
  if(level < tmp) return 0;

  if(check_busy())
  {
    write("You can only do one action at a time.\n");
    return 1;
  }

  if (!str)
    ob = attacker_ob;
  else
    ob = present(lower_case(str), environment(this_player()));

  if (!ob || !living(ob)) 
  {
    write("At whom?\n");
    return 1;
  }
  if (ob == this_object()) 
  {
    write("What?\n");
    return 1;
  }

  if (!attack_allowed(ob))
    return 1;

  set_heart_beat(1);
  call_other(this_object(),replace(query_verb(),"fire","fire_")+"_object",ob);
  return 1;
}

give_object(str)
{
  string item, dest;
  object *cont;
  
  if (!str)
    return notify_fail("Give what to whom?\n"),0;
  if (sscanf(str, "%s to %s", item, dest) != 2)
    return notify_fail("Give what to whom?\n"), 0;

  if(sizeof(cont=get_objects(dest,({environment()}))))
  {
    if(sizeof(cont=map_array(get_objects(lower_case(item),
					 ({this_object()}))-cont,
			     "put_obj",this_object(),cont)-({0})))
      return describe_objects(cont,"give"), 1;
    else 
      notify_fail("Sorry, but no objects were given.\n"), 0;
  }else{
    notify_fail("Give "+item+" to whom?\n"), 0;
  }
    
}

pose() 
{
  if (level >= 15) {
    write("You send a ball of fire into the sky.\n");
    say(cap_name + " makes a magical gesture.\n");
    say("A ball of fire explodes in the sky.\n");
    return 1;
  }
  return 0;
}

/*
 * Recursively compute the values of the inventory.
 */
int compute_values(object ob)
{
  int e, v, tmp;
  object *obs;
  obs = all_inventory(ob);
  for(v = 0, e = 0; e < sizeof(obs); e++)
  {
    tmp = (int) obs[e]->query_value();
    v += (tmp > 1000 ? 1000 : tmp);
    if(obs[e]->can_put_and_get())
      v+=compute_values(obs[e]);
  }
  return v;
}

save_me(value_items)
{
  /* Patch 951121: Characters with neither XP nor money
   * are not saved. Will keep down nr of chars in /players.
   * Of course, players who has a save file already are saved.
   */
  if (money == 0 && experience == 0 &&
      file_size("players/" + name + ".o") < 0 )
  {
    write("** Warning: No XP, no $. Character not saved.\n");
    time_to_save = age + 500;
    return;
  }
  tot_value = 0;
  if (value_items==1)
    tot_value = compute_values(this_object());

  if(value_items<2) compute_auto_str();
  save_object("players/" + name);
  time_to_save = age + 500;
}

illegal_patch(what) 
{
  write("You are struck by a mental bolt from the interior of the game.\n");
    
  log_file("ILLEGAL", ctime(time()) + ":\n" +
	   this_player()-> query_real_name() + " " +
	   what + "\n");
  return 0;
}

load_auto_obj(str) 
{
  string file, argument, rest;
  object ob;
  mixed dum;

  if(str)
    log_file("AUTOLOAD",name+":"+replace(str,"\n","")+"\n");
  while(str && str != "")
  {
    if (sscanf(str, "%s^!%s", file, rest) != 2)
    {
      write("Auto load string corrupt.\n");
      return;
    }
    argument=0;
    sscanf(file,"%s:%s",file,argument);
    file="/obj/daemon/autoaliasd"->auto_load_alias(file);
    str = rest;
    dum=catch(ob = clone_object(file),
	      argument ? ob->init_arg(argument ) : 0,
	      move_object(ob, this_object() ) ); /* Wing 920821 */
    if(dum && level>19)				 /* Profezzorn 921015 */
      write(file+": "+dum);			 /* Milamber */
  }
}

compute_auto_str()
{
#if 1
  auto_load=implode
  (
    map_array
    (
     all_inventory()->query_auto_load(),
     "check_auto_str",
     this_object()
     ),
    "^!"
  )+"^!";
#else
    object ob;
    string str;

    auto_load = "";
    ob = first_inventory(this_object());
    while(ob) {
	str = ob-> query_auto_load();
	str = check_auto_str(str);
	ob = next_inventory(ob);
	if (!str)
	    continue;
	auto_load += str + "^!";
    }
#endif
}

check_auto_str(str)
{
  string tmp;
  if (!stringp(str) || sscanf(str,"%s^!",str) 
      || (name=="guest" && !sscanf(str,"%slocal%s",tmp,tmp)))
    return 0;
  return str;
}


/*  If we have many quests, the #-separated string is too slow,
 *  so we use this temporary array of quests:
 */

static string *quests_array;

fix_quests_array() 
{
  string tmp, rest, rest_tmp;
  int i;

  if (quests_array && sizeof(quests_array))
    /* quests_array has already been set */
    return;
  else if (quests == 0 || quests == "")
    /* No quests solved */
    quests_array = ({ });
  else
    quests_array = explode(quests, "#");
}				/* fix_quests_array */

query_quests(str) 
{
  fix_quests_array();
  return str ? (member_array(str, quests_array) != -1) : quests;
}


/* Temporary function - don't use! */
query_quests_array(str) 
{
  fix_quests_array();
  
  return str ? (member_array(str, quests_array) != -1) : quests_array;
}

query_my_quest_max()  { return my_quest_max; }

static int cached_quest_points=-1;  /* Milamber. */

query_cached_quest_points()
{
  return (cached_quest_points!=-1)?cached_quest_points:query_quest_points();
}

query_quest_points() 
{
  return cached_quest_points="room/quest_room"->count_solved(this_object());
}

void clear_cached_quest_points()
{
  cached_quest_points=-1;
}

set_quest(q) 
{
  fix_quests_array();

  if (!q)
    return;
  if (query_quests(q))
    return 0;

  if (quests == 0)
    quests = q;
  else
    quests += "#" + q;
  
  quests_array +=({ q });
  save_me(1);

  cached_quest_points=-1;
#ifdef LOG_SET_QUEST
  if (previous_object()) {
    write_file("log/SECURE/QUESTS", ctime(time()) + ": "+name + ": " 
	       + q + " from " +
	       file_name(previous_object()) + "\n");
    if (this_player() && this_player() != this_object() &&
	query_ip_number(this_player()))
      write_file("log/SECURE/QUESTS", "Done by " +
		 this_player()->query_real_name() + "\n");
  }
#endif				/* LOG_SET_QUEST */
  catch("obj/quest_mailer"-> solved(name, q),
	"/obj/daemon/questd"-> solved(name, q),
	do_log());
  
  return 1;
}

remove_quest(str)
{
  if (!stringp(str)) return;
  if (this_player(1) != this_object() &&
      (this_player(1) -> query_level() < 23) &&
      !query_testchar())
  {
    write_file("log/SECURE/CHEAT",
	       this_player(1) -> query_real_name() +
	       " tried to remove quest \""+ str + "\" from " +
	       query_real_name() + " at " + time() + "\n");
    return;
  }
  if (!query_quests(str)) /* query_quests() also sets quests_array */
  {
    write("No such quest set.\n");
  }
  quests_array -= ({ str }); /* Remove it */
  quests = implode(quests_array, "#");
  save_me(1);
} /* remove_quest */

query_puzzles(str) 
{
  string tmp, rest, rest_tmp;
  int i;
  
  if (str == 0)
    return puzzles;

  if(sscanf("#"+puzzles+"#","%*s#"+str+"#%*s"))
    return 1;
  return 0;
}

set_puzzle(q) 
{
  if (!q)
    return;
  if (query_puzzles(q))
    return 0;
  /* No logging of puzzles. */
  if (puzzles == 0)
    puzzles = q;
  else
    puzzles = puzzles + "#" + q;
  return 1;
}

reset_all_puzzles() 
{
  log_file("PUZZLES", ctime(time()) + ": all puzzles reset in " + name + "\n");
  if (previous_object())
    log_file("PUZZLES", "previous_object() = " +
	     file_name(previous_object()) + " (" +
	     previous_object()->query_real_name() + ")\n");
  if (this_player())
    log_file("PUZZLES", "this_player() = " +
	     file_name(this_player()) + " (" +
	     this_player()->query_real_name() + ")\n");
  puzzles = 0;
}

time_out() 
{
  name = 0;
  cap_name = 0;
  level = -1;
  write("Login timed out\n");
  destruct(this_object());
}

/* A player is never normally picked up by other players, but wizards
 * can do anything. Thus, if a player is dropped, the local weight
 * would be scrambled.
drop() {  return 1; }
 */

int people_sort(object ob1, object ob2)
{
  return (string)ob1->query_real_name() < (string)ob2->query_real_name();
} /* people_sort */

static int pars;

int foo_scale_fun(object o,int paragon)
{
  string a;
  a=o->query_real_name()+"\n\n\n\n";

  if(!paragon)
    return a[0]*0x200000+a[1]*0x4000+a[2]*0x80+a[3];
  if(!o->query_paragon())
  {
    return a[0]*0x200000+a[1]*0x4000+a[2]*0x80+a[3]+0xFF00000;
  }
  else
  {
    pars++;
    return a[0]*0x200000+a[1]*0x4000+a[2]*0x80+a[3];
  }
}


int who(int *flags)
{
  mixed people;
  int wiz, mor,par;
  mixed wizz, mort,para;
  string *shorts;
  
  people = users() + (object *) "obj/daemon/whod"->query();     
  wizz=filter_array(people,"filter_users_fun",this_object(),20)-({0});
  mort=people-wizz-({0});
  wizz=m_values(mkmapping(map_array(wizz, "foo_scale_fun",this_object()),
			  wizz));
  wizz = wizz -> short();
  wizz = wizz - ({ 0 });
  wiz=sizeof(wizz);
  pars=0;
  mort=m_values(mkmapping(map_array(mort,"foo_scale_fun",this_object(),1),
			  mort));
  shorts = (string *) mort->short();
  par=sizeof(para=shorts[0..pars-1]-({0}));
  mor=sizeof(mort=shorts[pars..10000]-({0}));
  
  simplemore(sprintf("%|'='79s\n"," "+version()+" ")+ 	     
	     (wiz?implode(wizz,"\n"):"")+
	     (par?sprintf("\n%|79'='s\n"," Paragons ")+implode(para,"\n"):"")+
	     (mort?sprintf("\n%79'='s\n"," ")+implode(mort,"\n"):"")+
	     sprintf("\n%'='|79s\n",(" "+(wiz+mor)+" user(s) online, "+wiz+
		     " wizard(s) "+mor+" player(s) "))
		     
	     );	     
  flush();
  return 1;
} /* who */


reset_host() { host = 0; }

#define DNSD "/obj/daemon/dnsd"

query_host() 
{
  if (!host)
  {
    host = DNSD->query_hostname(query_ip_number(this_object()));
    if(host==query_ip_number())
    {
      host=0;
      return query_ip_number();
    }
  }
  return host;
}

#ifdef USE_SCARS
#define MAX_SCAR	10
int scar;

static make_scar() 
{
  if (level < 10)
    return;
  scar |= 1 << random(MAX_SCAR);
}

show_scar() 
{
  int i, j, first, old_value;
  string scar_desc;
  
  scar_desc = ({ "left leg", "right leg", "nose", "left arm", "right arm",
		 "left hand", "right hand", "forehead", "left cheek",
		 "right cheek" });
  j = 1;
  first = 1;
  old_value = scar;
  while(i < MAX_SCAR) {
    if (scar & j) {
      old_value &= ~j;
      if (first) {
	write(cap_name + " has a scar on " + query_possessive() +
	      " " + scar_desc[i]);
	first = 0;
      } else if (old_value) {
	write(", " + query_possessive() + " " + scar_desc[i]);
      } else {
	write(" and " + query_possessive() + " " + scar_desc[i]);
      }
    }
    j *= 2;
    i += 1;
  }
  if (!first)
    write(".\n");
}
#endif

static set_email(str) 
{
  if (!str) 
  {
    write("Your official electronic mail address is: " + mailaddr + "\n");
    return 1;
  }
  /* Guest should not have a strange email address. 
   * Brom 960322
   */
  if (stringp(name) && name == "guest")
    mailaddr = "guest@nannymud.lysator.liu.se";
  else
    mailaddr = str;
  write("Changed your email address.\n");
  return 1;
}

static set_www(string str)
{
  if (!str) 
    {
      write("Your WWW address is: " + wwwaddr + "\n");
      return 1;
    }
  if (stringp(name) && name == "guest")
  {
    write("The NannyMUD character 'guest' can't change the WWW address.\n");
    wwwaddr = "http://www.lysator.liu.se/nanny/";
    return 1;
  }
  wwwaddr = str;
  write("Changed your WWW address.\n");
  return 1;
} 

query_www(string str)
{
  return wwwaddr;
}

static int read_news(string arg)
{
  if (!arg)
  {
    simplemore(read_file("/etc/NEWS"));
    if(query_level()>19)
      simplemore(read_file("/etc/WIZNEWS"));
    flush();
    write ("\n");
    last_newsread=time();
    return 1;
  }

  if (arg=="on")
    if (view_news)
      return write ("It's already on.\n"), 1;
    else {
      view_news=1;
      return write ("You will now read news.\n"), 1;
    }

  if (arg=="off")
    if (!view_news)
      return write ("It's already off.\n"), 1;
    else {
      view_news=0;
      return write ("You will now not read news.\n"), 1;
    }
  notify_fail("Usage: News [on | off]\n");
}

status query_view_news() { return view_news; }

add_standard_commands() 
{
  add_action("shape","shape");
  add_action("display_boards","boards"); /* Milamber 940701 */
  add_action("earmuffs", "earmuffs");    /* Mats 930304 */
  add_action("do_wield", "wield");       /* Lpd, 920124 */
  add_action("new_plan","Plan");         /* Added by Qqqq */
  add_action("finger","Finger");         /* Added by Qqqq */
  if(name != "guest") 
  {
    add_action("add_access","addhost");	/* Anvil, 921026 */
    add_action("rem_access","subhost");
  }
  add_action("set_email", "email");
  add_action("set_www", "WWW");
  add_action("give_object", "give");
  add_action("score", "score");
  add_action("coins", "coins");
  add_action("save_character", "save");
  add_action("pre_quit", "quit");
  add_action("kill", "kill");
  add_action("communicate", "say");
  add_action("communicate", "'", 1);
  add_action("shout_to_all", "shout");
  add_action("put", "put");
  add_action("pick_up", "get");
  add_action("pick_up", "take");
  add_action("drop_thing", "drop");
  add_action("inventory", "i");
  add_action("inventory", "inventory");
  add_action("look", "look");
  add_action("look", "l");
  add_action("examine", "exa", 1);
  add_action("help", "help");
  add_action("reply_to_last", "reply"); /*Added by Q4 921022*/
  add_action("tell", "tell");
  add_action("whisper", "whisper");
  add_action("change_password", "password");
  add_action("typbugpra", "idea");
  add_action("typbugpra", "typo");
  add_action("typbugpra", "bug");
  add_action("typbugpra", "praise");
  add_action("report","report");
  add_action("converse", "converse");
  add_action("stop_hunting_mode", "stop");

/* Removed by Taren 950221, this spells are in the new
   adv_guild.
  add_action("spell_std", "missile");
  add_action("spell_std", "shock");
  add_action("spell_std", "fireball");
*/
  add_action("pose", "pose");
  add_action("who", "who");
  add_action("stty", "stty");
  add_action("read_news","News");
  add_action("set_prompt","set_prompt");
  add_action("toggle_all","hpinfo");
  add_action("toggle_all", "brief");
  add_action("toggle_all", "wimpy");
  add_action("toggle_all", "toggle");  /* By Slater 940709 */
  add_action("list_equipment", "eq");  /* By Slater 940705 */
  add_action("away", "away");	       /* By Slater 940705 */
}


/*Added by Qqqq 920427*/

nomask query_valid_write(str) { return this_object()->valid_write(str); }
nomask query_valid_read(str)  { return this_object()->valid_read(str); }

/* XXXXXX FIXME!!! XXXXXX */
nomask valid_write(str, logread)
{
  if(str[0]=='/')
    return str[1..100000];
  return str;
}

nomask valid_read(str, lvl)
{
  if(str[0]=='/')
    return str[1..100000];
  return str;
}

/*A small finger function added by Qqqq*/

#define FINGERD "/obj/daemon/fingerd"

finger(who) 
{
  string str;
  if (!who) 
    return write("You use it: Finger <playername>\n"), 1;
  who=lower_case(who);
  if(!(str = FINGERD->finger(who)))
  {
    write("There is no player with that name.\n");
    if(query_level()>20)
      if(file_size("/banish/"+who+".o")>0)
	write("That name is banished.\n");
    return 1;
  } 
  simplemore(str);
  flush();
  return 1;
}

void add_to_plan(string str)
{
  if(str == "~q")
    return write("Aborted!\n");
  else if(str != "**" && str != ".")
  {
    saved_where += str + "\n";
    input_to("add_to_plan");
    write("]");
  } else {
    plan_desc = saved_where;
    write("Your plan reads:\n" + plan_desc);
    save_me(1);
  }
}

int new_plan(string str) 
{
  if(!str)
  {
    if(plan_desc)
    {
      write("Your plan reads:\n" + plan_desc);
      if(plan_desc[-1] != '\n')
	write("\n");
    } else
      write("You have no plan.\n");
  } else {
    saved_where = "";
    input_to("add_to_plan");
    write("Write your plan. Finish with '**' or '~q' to cancel.\n]");
  }
  return 1;
}

int plan(string str) 
{
  write("This function is outdated.\n");
  write("Now the usual 'Plan' command handles multiple lines.\n");
  return 1;
}


#ifdef USE_FOLLOW

follow(str)
{
  object ob, ob2;
  if (!str)
  {
    if (!follow_ob)
      return write("You are not following anyone.\n"), 1;
    follow_ob->remove_follow();
    write("You are not following " +
	  capitalize(follow_ob->query_real_name()) +
	  " anymore.\n");
    follow_ob = 0;
    return 1;
  }
  
  ob = present(str, environment());
  if (!ob)
    return write("Follow what?\n"), 1;
  
  /* May not automatically follow wizards */
  if (!ob->query_npc() && ob->query_level() >= 20)
    /* Changed is_npc to query_npc, by Padrone May 11, 1992 */
  {
    write("You may not follow " +
	  capitalize(ob->query_real_name()) + "!\n");
    return 1;
  }
  
  /* Check for follow loops */
  ob2 = ob;
  while ((ob2 = ob2->is_following()) && ob2 != this_object())
    ;
  if (ob2)
  {
    write("You cannot follow "+ob->query_objective()+".\n");
    return 1;
  }
  
  if (ob->start_follow())
  {
    write("You are now following " +
	  capitalize(ob->query_real_name()) + ".\n");
    follow_ob = ob;
  }
  else
    write("You cannot follow that!\n");
  
  return 1;
}


is_following()
{
  return follow_ob;
}

#endif

do_wield(str)
{
  object ob;
  
  if (!str)
    return notify_fail("Wield what?\n"), 0;
  
  ob = present(str, this_object());
  
  if (!ob)
    return notify_fail("You don't have that!\n"), 0;
  
  if (!ob->query_weapon())
    if (ob->weapon_class())
      return 0;			/* Old weapon, be compatible... */
    else
      /* Not a new or old weapon */
      return notify_fail("You cannot wield that!\n"), 0;
  
  if (silent_wield(ob))
    return write("Ok.\n"), 1;
  return notify_fail("You cannot wield that!\n"), 0;
}


remove_statue(arg)
{
  object *oblist;
  int i;
  
  arg++;
  
  switch (arg)
  {
   case 1:
    statue_type = "a dusty";
    break;
    
   case 2:
    statue_type = "an old";
    break;
    
   case 3:
    statue_type = "a slightly cracked";
    break;
  }
  
  if (arg >= MAX_STATUE)
  {
    object ob;
    
    if(!query_invis())
    {
      say("A nearby statue starts to crumble, and as time passes "+
	  "it withers to dust.\n");
    }
    
    is_linkdead = 0;
    statue_type = "a nice";
    
    ob = clone_object("/obj/dust");
    move_object(ob, environment());
    
    move_object(this_object(), "/room/void");
    quit();
  }
  else
    call_out("remove_statue", TICK_STATUE, arg);
}


/*----------- Most of the playerkilling code here: ------------*/

#ifdef USE_PK_FLAG
query_playerkilling() { return playerkilling; }

set_playerkilling(s) 
{
#ifdef PARANOIA
  log_file("PLAYERKILLING", ctime(time()) + ": Changed status of " +
	   query_real_name() + " from " + playerkilling + " to " + s + "\n");
  if (previous_object())
    log_file("PLAYERKILLING", "previous_object() = " +
	     file_name(previous_object()) + " (" +
	     previous_object()->query_real_name() + ")\n");
  if (this_player() != this_object())
    log_file("PLAYERKILLING", "this_player() = " +
	     file_name(this_player()) + " (" +
	     this_player()->query_real_name() + ")\n");
#endif  
  playerkilling = s;
}				/* set_playerkilling */

/* Check if we are allowed to attack, with respect to statues and playerkilling */
attack_allowed(victim)
{
  object the_room;
  string message;
  the_room = environment(this_object());
  if (the_room == 0)
    return 0;
  if (the_room->query_property("no_fight")) 
  {
    if(!stringp(message = the_room -> query_property("no_fight")))
    {
      message = "Such actions are forbidden here.\n";
    }else{
      if(message[strlen(message)-1] != '\n')
	message += "\n";
    }
    return write(message), 0;
  }
  
  if (!(interactive(victim) || victim->query_npc()))
  {
    write("You can't fight with stone statues!\n");
    say(cap_name + " tries foolishly to attack a statue of " +
	victim->query_name() + ".\n");
    return 0;
  }
  
  if (!victim->query_npc()) 
  {
    /* We are trying to attack another player. */
#ifdef PARANOIA    
    log_file("PLAYERKILLING", ctime(time()) + ": " +
	     query_real_name() + " tried to attack " + victim->query_real_name() + "\n");
    if (previous_object())
      log_file("PLAYERKILLING", "previous_object() = " +
	       file_name(previous_object()) + " (" +
	       previous_object()->query_real_name() + ")\n");
    if (this_player() != this_object())
      log_file("PLAYERKILLING", "this_player() = " +
	       file_name(this_player()) + " (" +
	       this_player()->query_real_name() + ")\n");
    
#endif
    if (the_room->query_playerkilling()) 
    {
#ifdef PARANOIA
      log_file("PLAYERKILLING", "ALLOWED - in " + file_name(the_room) + ".\n");
#endif
      return 1;
    }	    
    
    if (query_playerkilling() && victim->query_playerkilling()) 
    {
#ifdef PARANOIA
      log_file("PLAYERKILLING", "ALLOWED - both players are playerkillers.\n");
#endif
      return 1;
    }
    
    write("Your conscience doesn't allow you to do that.\n");
#ifdef PARANOIA
    log_file("PLAYERKILLING", "DENIED.\n");
#endif
    return 0;
  }  
  return 1;
}				/* attack_allowed */
#else
query_playerkilling() { return 1; }


/* Check if we are allowed to attack, with respect to statues and playerkilling */
attack_allowed(victim)
{
  object the_room;
  string message;
  the_room = environment(this_object());
  if (!the_room) return 0;
  if (the_room -> query_property("no_fight")) 
  {
    if(!stringp(message = the_room -> query_property("no_fight")))
    {
      message = "Such actions are forbidden here.\n";
    }else{
      if(message[strlen(message)-1] != '\n')
	message += "\n";
    }
    return write(message), 0;
  }
  
  if (!(interactive(victim) || victim->query_npc()))
  {
    write("You can't fight with stone statues!\n");
    say(cap_name + " tries foolishly to attack a statue of " +
	victim->query_name() + ".\n");
    return 0;
  }
  
  return 1;
}				/* attack_allowed */
#endif

/* No more bouncing /wing 920512 */

nomask query_level() { return level; }

nomask query_level_sec(string func) 
{
  object pob;
  string s1,f;
  
  
  if (func == "query_ident")
  {
    object ob;
    
    if (previous_object() == this_object())
      return 23;
#ifdef PARANOIA
    else
    {
      ob = previous_object();
      if (ob)
	log_file("QUERYSEC", "Previous_object ("+
		 file_name(ob) + ") != this_object().\n");
      else
	log_file("QUERYSEC", "No Previous object ()!\n");
    }
#endif
  }
  
  if(level <= 21 )
    return level;
  
  pob=previous_object();
  if(!pob) return level;
  f=file_name(pob);
  if(level>22 &&  /* Level 22 look like 21 to query_level_sec() /Profezzorn */
     (sscanf(f,"obj/%s",s1)==1 ||
      sscanf(f,"room/%s",s1)==1 ||
      sscanf(f,"secure/%s",s1)==1 ||
      sscanf(f,"local/%s",s1)==1 ||
      sscanf(f,"players/"+name+"/%s",s1)==1))
    return level;
  return 21;
}

get_nickname(str) { return str; }

restore_me() { return restore_object("players/" + name); }

set_ghost()			/* Special for Ramses guild..sigh..  */
{
  ghost = fake_ghost = 1;
  drop_all(1);
}

query_real_ghost() { return (ghost && !fake_ghost); }


int query_rows() { return term_rows; }
int query_cols() { return term_cols; }
string query_term(){ return term_type; }


/* Added 93036 pen@lysator.liu.se */
static stty(str) 
{
  string cmd;
  string val;
  
  if (!str || str == "" || str == "all" || str == "everything") 
  {
    write("" + term_rows + " rows, " + term_cols + " columns\n");
    if (str == "everything" || str == "all")
      write("Terminal type is " + term_type + "\n");
    return 1;
  } else {
    if (sscanf(str, "%s %s", cmd, val) != 2) 
    {
      write("stty: illegal options '" + str + "'\n");
      return 1;
    }
    if (cmd == "rows") 
    {
      if (sscanf(val, "%d", term_rows) != 1 || term_rows < 3) 
      {
	write("stty: illegal rows argument '"+val+"'\n");
	return 1;
      }
    } else if (cmd == "cols" || cmd == "columns") {
      if (sscanf(val, "%d", term_cols) != 1) 
      {
	write("stty: illegal columns argument '"+val+"'\n");
	return 1;
      }
      update_wiz_line();
    } else if (cmd == "term") {
      term_type = val;
    } else {
      write("stty: illegal option '" + cmd + "'\n");
      return 1;
    }
  }
  write("Ok.\n");
  return 1;
}

object query_hunter() { return hunter; }

void set_testchar(string whose)
{
   if ((this_player(1) && (int) this_player(1)->query_level_sec() > 22) ||
       (file_name(previous_object()) == "room/registration_office"))
     testchar = whose;
}

void set_secondchar(string whose)
{
   if ((this_player(1) && (int) this_player(1)->query_level_sec() > 22) ||
       (file_name(previous_object()) == "room/registration_office"))
     secondchar = whose;
}

query_testchar()
{
  if(this_player(1))
  {
    switch(this_player(1)->query_level())
    {
    case 20..21:
      if(testchar) return "yes";
      else return 0;
    case 22..50:
      return testchar;
    default:
      return 0;
    }
  }
  else
    return 0;
}

query_secondchar()
{
  if(this_player(1))
  {
    switch(this_player(1)->query_level())
    {
    case 20..21:
      if(secondchar) return "yes";
      else return 0;
    case 22..50:
      return secondchar;
    default:
      return 0;
    }
  }
  else
    return 0;  
}

set_gender(g)
{
  /* profezzorn loggar {ckligt l{bbiga lynglar */
  "/obj/daemon/syslogd"->do_log("GENDER CHANGE: "+
				query_real_name()+":"+
				(this_player(1)?this_player(1)->query_real_name():"")+":"+
				file_name(previous_object())+":"+
				g+"\n");

  if (intp(g) && g>-1 && g<3)
    gender = g;
}

set_neuter() { set_gender(0); }
set_male()   { set_gender(1); }
set_female() { set_gender(2); }

query_wiztool()
{
  int num;
  if(!wiztool)
  {
    wiztool=present("wizsoul",this_object());
    if(wiztool && !sscanf(file_name(wiztool),"obj/wiztool#%d",num))
      wiztool=0;

    if(!wiztool)
    {
      wiztool=clone_object("/obj/wiztool");
      move_object(wiztool,this_object());
    }
  }
  return wiztool;
}


query_path()
{
  object o;
  o=query_wiztool();
  if (o)
    return o->query_path(); 
  else
    return 0;
}

string query_ident()
{
   if(stringp(ident_user))
      return ident_user;
   if(!ident_user)
      "/obj/daemon/identd"->query_ident(this_object());
   return (string) 0;
}

public status hard_invis;

nomask toggle_hard_invis(status s)
{
  hard_invis = ! hard_invis;
  if(hard_invis)
  {
    invis();
    tell_object(this_object(),"You are now _very_ INVISIBLE.\n");
    destruct(get_wiz_player());
  }else{
    vis();
    tell_object(this_object(),"You are now quite VISIBLE.\n");
    get_wiz_player();
  }
}

nomask is_hard_invis() { return hard_invis; }

set_ident(mixed arg)
{
   if(previous_object() == load_object("/obj/daemon/identd"))
      if(!stringp(ident_user) || stringp(arg) || !arg)
	 ident_user = arg;
}

#define get_tox() sprintf("%d%%",(intoxicated*100)/(level + 3))
#define get_soak() sprintf("%d%%",(soaked*37)/(level+Str+Con))
#define get_full() sprintf("%d%%",(stuffed*12)/level)

update_prompt()
{
/* 
   $qp   -- Cached Qp
   $hp   -- Hp
   $mhp  -- Max HP
   $tox  -- Intox level
   $full -- 'Full' ness
   $soak -- Soaked level
   $sp   -- Sp
   $msp  -- Max SP
   $exp  -- Exp
   $gold -- Pengar
   $path -- Wiz> Path
   $env  -- Wiz> Room
*/
  string tmp_prompt, foo;
  if(prompt=="> ") return prompt;
  if (!prompt) return "No prompt set.";
  if(sscanf(prompt,"%*s$"))
  {
    tmp_prompt=replace(replace(replace(replace(replace(prompt,"$hp",
						       hit_point+""),
					       "$sp",spell_points+""),
				       "$exp",""+experience),
			       "$gold",money+""),
		       "$qp",cached_quest_points+"");
    tmp_prompt=replace(replace(replace(tmp_prompt, "$tox",get_tox()),
			       "$full",get_full()),"$soak",get_soak());
    tmp_prompt=replace(replace(tmp_prompt, "$mhp", ""+max_hp),
		       "$msp", ""+max_sp);
    if(level > 20)
    {
      if(foo = query_path())
      {
	if(foo[0] != '/')
	  foo = "/" + foo;
	if(foo == "/players/" + name)
	  foo = "~";
	else if(sscanf(foo, "/players/" + name + "/%s", foo) == 1)
	  foo = "~/" + foo;
	else if(sscanf(foo, "/players/%s", foo) == 1)
	  foo = "~" + foo;
      }else{
	foo="~/";
      }
      tmp_prompt = replace(replace(tmp_prompt, "$path", foo),
			   "$env", environment() ?
			   file_name(environment()) : "The VOID");
    }
  } else {
    tmp_prompt=prompt;
  }
  if(tmp_prompt && this_player(1) == this_object() &&
     this_player() == this_object())
    _set_prompt(tmp_prompt);
  return tmp_prompt;
}

int set_prompt(string to)
{
  if(!to || to == "")
    return write("Set prompt to what?\n"), 1;
  prompt=replace(replace(replace(to,"$name",cap_name),"$mud","NannyMUD"),
		 "\\n","\n")+" ";

  if(this_player() == this_object())
    _set_prompt(prompt);

  write("Ok.\n");
  update_prompt();
  return 1;
}

frog_curse(arg) 
{
  if (arg) 
  {
    object frogger;
    string rname;

    if (frog)
      return 1;
    tell_object(this_object(), "You turn into a frog!\n");

    /* And now: some logging... */
    if (objectp(this_player(1) || this_player()))
    {
      frogger = this_player(1);
      rname = this_player(1) -> query_real_name();
    }
    else
    {
      frogger = previous_object();
      rname = file_name(frogger);
    }
    if (frogger != this_object())
    {
      "/obj/daemon/syslogd"->do_log("FROG: " + query_real_name() +
				    " frogged by "+ rname + "\n");
    }
    frog = 1;
    this_object()->count_weight();
    return 1;
  }
  tell_object(this_object(), "You turn HUMAN again.\n");
  frog = 0;
  return 0;
}

/*
   Paragon stuff added by Taren, namechanges by Leowon 951110
 */

paragon_log(ob, type)
{
  string fname;
  /* Don't log under some circumstances. */
  if (   previous_object() != this_object()
      || !objectp(ob)
      || !stringp(type)) return;

  sscanf(file_name(ob), "%s#%*s", fname);
  switch(fname)
  {
  case "obj/player" :
  case "std/special/paragon/paragon_obj" :
    break;
  default:
    write_file("/log/PARAGONS",
	       type + ": " + fname + "\n");
  }
}

query_paragon()
{
  paragon_log(previous_object(), "query_paragon");
  return paragon;
}

query_paragon_level()
{
  paragon_log(previous_object(), "query_paragon_level");
  return paragon_level;
}

set_paragon()
{
  if(file_name(environment(this_object()))!="room/paragon_entry_room")
    return 0;
  paragon=1;
  write_file("/log/PARAGONS","set paragon: "+capitalize(query_real_name())+"\n");
  return 1;
}

set_paragon_level(int i)
{
  if(file_name(environment(this_object()))!="room/paragon_room") return 0;
  paragon_level=i;
  max_hp = 42 + query_con() * 8 + i*2;
  max_sp = 42 + query_int() * 8 + i*2;
   /*
     What follows is a small sanity check, if any one is near breaking this
     barrier we can raise it later.
   */
  if(max_hp>300) max_hp=300;
  if(max_sp>300) max_sp=300;
  write_file("/log/PARAGONS","paragon level: "+capitalize(query_real_name())+" to "+i+"\n");
  return paragon_level;
}


/* Shape stuff...*/
show_shape(object ob)
{
  int max, cur;
  
  max=ob->query_max_hp();
  if (max==0) max=1; /* Brom 950918, failsafe. Been known to happen*/
  cur=ob->query_hp();
  switch((cur*100)/max)
    {
    case 90..1000:  return "undamaged";
    case 80..89: return "superior";
    case 70..79: return "very good";
    case 60..69: return "good";
    case 50..59: return "fair";
    case 40..49: return "fairly poor";
    case 30..39: return "poor";
    case 20..29: return "weak";
    case 10..19: return "very weak";
    default:
      return "deplorable";
    }
}

shape(string str)
{
  object ob;

  notify_fail("That is not here.\n");
  if(str) 
    ob=present(str,environment(this_object()));
  if(!ob)
    ob=attacker_ob;
  if(!ob)
    return 0;
  write(capitalize(ob->query_name())+" is in "+show_shape(ob)+" shape.\n");
  return 1;
}


static fix_me()
{
  _lock(0);
  if(!present("wizsoul",this_object()))
    move_object(clone_object("/obj/wiztool"),this_object());
  write("You now have now been fixed.\n");
  return 1;
}

query_prevent_virtual() { return 1; }

