/* Standard guild by Profezzorn
 * based on the old /room/adv_guild.c
 */

#pragma strict_types
#pragma save_types

#include <tune.h>
inherit "/std/room";

static string guild_name;
static string guild_short;
static string guild_mark;

/* Call this function from reset with the following arguments:
 * name: The name of the guild, like "prophets"
 * short: a one line desc of the guild, like "the prophets guild"
 * guild_mark: object to clone when someone joins
 */
void set_guild(string name, string short, string mark)
{
  if(guild_name) return;
  guild_name=name;
  guild_short=short;
  guild_mark=expand_file_name(mark);
}

/* Call att reset to add a 'standard' join command.
 * be sure to call _after_ calling set_guild
 */
void add_join()
{
  add_command("join "+guild_name,"@join()");
  add_command("join "+guild_short,"@join()");
  add_command("join","@join()");
  add_command("join guild","@join()");
}

/* Don't forget to call ::reset */
void reset(int arg) 
{
  ::reset(arg);
  if (!arg)
  {
    add_property("no_fight");
    add_property("no_magic");
    
    add_command("cost","@cost_cmd()");
    add_command("list %s","@list_quests_cmd()");
    
    add_command("advance %s","@advance_stat_cmd()");
    add_command("advance","@advance_level_cmd()");
    add_command("advance level","@advance_level_cmd()");
    add_command("wiz","@wiz_cmd()");
  }

  if(!present("party object",this_object()))
    move_object(clone_object("/obj/party"),this_object());
} /* reset */

int ismember()
{
  object ob;
  string a;

  if(ob=present("guild_mark",this_player())) 
  {
    a="/"+file_name(ob);
    sscanf(a,"%s#",a);
    if(a==guild_mark) return 1;
  }
  return 0;
}

/* Feel free to redefine */
int join()
{
  object ob;
  string a;

  if(ob=present("guild_mark",this_player())) 
  {
    if(ismember())
    {
      write("You are already a member.\n");
    }else{
      write("You can not join "+guild_short+" because you are already\nin another guild.\n");
    }
    return 1;
  }
      
  ob=clone_object(guild_mark);
  if(transfer(ob,this_player()))
  {
    write("You are too weak to receive the guild token.\n");
    destruct(ob);
    return 1;
  }
  ob->init_guild_ob();
  write("You have joined the "+guild_short+".\n");
  say(this_player()->query_name()+" has joined the "+guild_short+".\n");
  return 1;
}

/* You _must_ redefine this function.
 * It should return the approperiate title for a player in this guild
 * with level 'level' and gender 'gender'.
 */
string get_title_for_level(int lvl, int gender)
{
  return "the meddling tester";
}

/* Please do NOT redefine */
int xp_for_level(int lvl) 
{
  switch(lvl)
  {
    case 20: return 1000000;
    case 19: return  666666; 
    case 18: return  444444;
    case 17: return  296296;
    case 16: return  197530;
    case 15: return  131687;
    case 14: return   97791;
    case 13: return   77791;
    case 12: return   58527;
    case 11: return   39018;
    case 10: return   26012;
    case 9: return   17341;
    case 8: return   11561;
    case 7: return    7707;
    case 6: return    5138;
    case 5: return    3425;
    case 4: return    2283;
    case 3: return    1522;
    case 2: return    1014;
    case 1: return     676;
  }
}

/* Returns cost to advance to next level. 
 * arguments are current level and xp
 */
int query_cost_for_level(int level, int exp) 
{
  int next_exp;
  next_exp=xp_for_level(level+1);
  if (next_exp <= exp) return 0;
  return (next_exp - exp) * 1000 / EXP_COST;
}


/* 
 * Compute cost for raising a stat one level. 'base' is the level that
 * you have now, but never less than 1.
 */
int raise_cost(int base)
{
  int cost, saldo;
  
  if (base >= 20) return 0;
  cost = (xp_for_level(base+1) - xp_for_level(base)) / STAT_COST;
  if(cost < 0) cost=0;
  return cost;
}


/* Internal function */
static void cost_for_stat(string stat)
{
  int cost;
  write(capitalize(stat)+": ");
  if(cost=raise_cost((int)call_other(this_player(),"query_"+stat)))
  {
    write(cost+" experience point"+(cost==1?"":"s")+".\n");
  }else{
    write("Not possible.\n");
  }
}


/*** Commands ***/
int cost_cmd()
{
  int current_max, cost;

  if(!ismember())
  {
    write("Please check this in your own guild.\n");
    return 1;
  }

  cost_for_stat("str");
  cost_for_stat("con");
  cost_for_stat("dex");
  cost_for_stat("int");

  if (this_player()->query_level() >= 20)
  {
    write("You will have to seek other ways to advance your level.\n");
    return 1;
  }
  
  /* If a player has solved ALL the quests, she, he (or it) can 
   * become a wizard! 
   * This works only if the quest point max is at least 1000. 
   */
  current_max = "room/quest_room"->count_all();
  if (this_player()->query_quest_points() >= current_max &&
      current_max >= 1000)
  {
    write("Since you have solved all the quests, you can now advance to level 20\nby typing 'wiz'.\n");
    return 1;
  }
  
  cost=query_cost_for_level(this_player()->query_level(), this_player()->query_exp());
  if (!cost)
  {
    write("It will cost you nothing to be promoted.\n");
    return 1;
  }
  write("It will cost you "+cost+" gold coins to advance to level "+
	(this_player()->query_level()+1)+
	".\n");
  return 1;
}


/* internal function */
static void advance(int to, int cost)
{
  string title;

  if (cost)
  {
    if (this_player()->query_money() < cost)
    {
      write("You don't have enough gold coins.\n");
      return;
    }
    this_player()->add_money(-cost);

    if(xp_for_level(to) > this_player()->query_exp())
      this_player()->add_exp(this_player()->query_exp() - xp_for_level(to));
  }

  this_player()->set_level(to);
  title=get_title_for_level(to, this_player()->query_gender());
  this_player()->set_title(title);
  say(this_player()->query_name()+ " is now level " + to + ".\n");
  if (to==0) return;
  
  if (to < 7) {
    write("You are now " + this_player()->query_name() + " " + title +
	  " (level " + to + ").\n");
  }
  else if (to < 14) {
    write("Well done, " + this_player()->query_name() + " " + title +
	  " (level " + to + ").\n");
  }
  else if (to < 20)  {
    write("Welcome to your new class, mighty one.\n" +
	  "You are now " + title + " (level " + to + ").\n");
  }
  else if (to == 20)
  {
    write("A new Wizard has been born.\n");
    shout("A new Wizard has been born.\n");
  }
}


int wiz_cmd()
{
  int current_max;

  if(!ismember())
  {
    write("You can only do this in your own guild.\n");
    return 1;
  }


  /* If a player has solved ALL the quests, he (she, it) can become 
   * a wizard! 
   * This works only if the quest point max is at least 1000. 
   */
  current_max = "room/quest_room"->count_all();
  if ((query_verb() == "wiz") 
    && (this_player()->query_quest_points() >= current_max && current_max >= 1000))
  {
    write("Since you have solved all the quests, you are ready to become a wizard!\n");
    advance(20,0);
  }
  return 1;
}

int advance_level_cmd()
{
  int level, qp_left, exp, cost;
  object guild_ob;
  
  if(this_player()->query_level() > 19)
  {
    write("You can no longer advance here.\n");
    return 1;
  }

  if(!ismember())
  {
    write("Please advance in your own guild.\n");
    return 1;
  }

  if (qp_left="room/quest_room"->count(1))
  {
    write("You have not solved enough quests to advance.\n");
    write("You need "+qp_left+" more questpoint"+(qp_left==1?"":"s")+" to advance to next level.\n");
    return 1;
  }

  level = this_player()->query_level();
  if (level < 0) level = 0;

  exp = this_player()->query_exp();
  cost=query_cost_for_level(level, exp);

  advance(level+1, cost);
  return 1;
}

/* internal function */
static string gnd_prn()
{
  switch(this_player()->query_gender())
  {
  case 0: return "best creature";
  case 1: return "sir";
  case 2: return "madam";
  }
}

int advance_stat_cmd(int dummy, string which_stat)
{
  int stat, cost, exp, diff;
  string alas;

  if(!ismember())
  {
    write("You can only do this in your own guild.\n");
    return 1;
  }

  if(!which_stat) which_stat="";
  which_stat=lower_case(which_stat);

  which_stat=which_stat[0..2];
  switch(which_stat)
  {
  case "con": alas="tough and edurable"; break;
  case "dex": alas="skilled and vigorous"; break;
  case "str": alas="strong and powerful"; break;
  case "int": alas="knowledgeable and wise"; break;
    break;

  default:
    write("What do you want to advance?\n"+
	  "Strength, Constitution, Dexterity or Intelligence?\n");
    return 1;
  }

  if ((this_player()->query_con() +
       this_player()->query_str() +
       this_player()->query_int() +
       this_player()->query_dex()) / 4 >= this_player()->query_level() + 3)
  {
    write("Sorry " + gnd_prn() +
	  ", but you are not of high enough level.\n");
    return 1;
  }
  
  stat=(int)call_other(this_player(),"query_"+which_stat);
  if(stat >= 20)
  {
    write("Sorry, "+gnd_prn()+", but you are already as "+alas+"\n"+
	  "as any "+(this_player()->query_gender()?"one":"thing")+
	  " could possibly hope to get.\n");
    return 1;
  }

  cost=raise_cost(stat);
  exp=this_player()->query_exp();
  diff=exp - xp_for_level(this_player()->query_level()) - cost;
  if(diff<0)
  {
    write("You need "+(-diff)+" more experience points to do that.\n");
  }else{
    this_player()->add_exp(-cost);
    call_other(this_player(),"set_"+which_stat,stat+1);
    write("Ok.\n");
  }
  return 1;
}

int list_quests_cmd(int dummy, string str)
{
  int qnumber;
  
  if(!str)
    str="";
  else
     str = lower_case(str);

  switch(str)
  {
  case "":
  case "all":
  case "all quests":
    /* List all solved quests */
    "room/quest_room"->more_list_solved();

    /* fall through */

  case "quests":
  case "unsolved":
  case "unsolved quests":
  case "all unsolved":
  case "all unsolved quests":
    /* List all unsolved quests */
    call_other("room/quest_room", "more_count", 0);
    break;

  case "solved":
  case "solved quests":
  case "all solved":
  case "all solved quests":
    /* List all solved quests */
    "room/quest_room"->more_list_solved();
    break;

  default:
    if (sscanf(str, "%d", qnumber) ||
	sscanf(str, "quest %d", qnumber) ||
	sscanf(str, "number %d", qnumber) ||
	sscanf(str, "quest number %d", qnumber) ||
	sscanf(str, "unsolved %d", qnumber) ||
	sscanf(str, "unsolved quest %d", qnumber) ||
	sscanf(str, "unsolved quest number %d", qnumber))
    {
      /* Show info about one of the solved quests */
      "room/quest_room"->more_list(qnumber);
    }
    else if (sscanf(str, "solved %d", qnumber) ||
	     sscanf(str, "solved number %d", qnumber) ||
	     sscanf(str, "solved quest %d", qnumber) ||
	     sscanf(str, "solved quest number %d", qnumber))
    {
      /* Show info about one of the solved quests */
      "room/quest_room"->more_list_solved(qnumber);
    }
    else
    {
      notify_fail("Which quest or quests do you want to list?\n" +
		  "Use for example 'list', 'list unsolved' or 'list unsolved 2'.\n");
      return 0;
    }
  }
  this_player()->flush();
  return 1;
}
