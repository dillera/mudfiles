
#include <var.h>

inherit "/obj/env_var";

/* It is ESSENTIAL that all nonstatic global
 * variables in this object are initalized
 * to respectable values below
 */
string  name = "";
string  title = "";
string  al_title = "";
string  plan_desc = "";

int     money = 0;
int     level = 0;
int     max_hp = 0;
int     max_sp = 0;
int     age = 0;
int     Str = 0;
int     Int = 0;
int     Con = 0;
int     Dex = 0;
int     my_quest_max = 0;

int     gender = 0;
string  last_login_time = 0;
int     last_login_time_int = 0;
int     last_logout_time_int = 0;
int     npc = 0;
string  called_from_ip = 0;
string  called_from_host = 0;

string  mailaddr = "";
string  wwwaddr = "";
int     playerkilling = 0;
int     experience = 0;
string  testchar = "";
string  secondchar = "";
string  quests = 0;

int paragon =0;

void __INIT();

void zero_var () 
{
  __INIT();
}


int restore (string who) 
{
  int e;
  if(name == who) return 1;
  zero_var();
  if(who)
  {
    npc=0;
    if(!(e=restore_object("players/" + who)))
    {
      npc=1;
      e=restore_object("etc/fake_finger_save/" + who);
    }
    return e;
  }
  return 0;
}


#define PRONOUN(gender) (gender ? (gender == 1 ? "he" : "she") : "it")

string get_title(string cap_name)
{
    string newtitle;
    string tmp1, tmp2;

    if(newtitle = getenv(TITLE))
    {
      newtitle = replace(newtitle, "$N", cap_name);
      if (!newtitle)
	return cap_name + title;
      if(sscanf(newtitle, "%s" + cap_name + "%s", tmp1, tmp2) != 2)
	return cap_name + " " + newtitle;
      return newtitle;
    }
    return cap_name + " " + title;
}

age(int i)
{
  int t;
  string s;
  s="";

  if (t=i/86400) {
    s+=(t + " day"+(t==1?" ":"s "));
    i %= 86400;
  }
  if (t=i/3600) {
    s+=(t + " hour"+(t==1?" ":"s "));
    i %= 3600;
  }
  if (t=i/60) {
    s+=(t + " minute"+(t==1?" ":"s "));
    i %= 60;
  }
  s+=(i + " seconds");

  return s;
}


finger(string str) 
{
  object ob;
  int tmp;
  
  str = lower_case(str);

  if(!restore(str))  return 0;
  name = capitalize(name);
  str = sprintf("%s (%s)\n\n", get_title(name), stringp(al_title)?al_title:"IDI");
  ob = (object)"/obj/daemon/whod"->find_player(lower_case(name));

  str += capitalize(PRONOUN(gender)) + " ";

  if(this_player(1) && this_player(1)->query_level()>19 && npc)
  {
    str+="is really an NPC, but appears on finger anyway.\n";
  }else{
    switch(level)
    {
    case -10000000..19:
      if(paragon)
	str += "is a mighty Paragon.\n";
      else
	str += "is a puny mortal.\n";
	
      break;

    case 20:
      str += "is a new immortal wizard.\n";
      break;

     case 21: 
      str += "is a powerful immortal wizard with creating talents.\n";
      break;

     case 22:
      str += "is a skilled, powerful immortal wizard with creating talents.\n";
      break;

    case 23:
      str += "is a powerful and very helpful high wizard.\n";
      break;

    case 24:
      str += "is a special immortal wizard.\n";
      break;

     case 25:
      if(name == "Oros")
	str += "is a skilled arch sourceror.\n";
      else 
	str+="is one of the divine archwizards (beholder of the law).\n";
      break;
      
    case 30:
      str += "is an old elder wizard with unbelievable powers.\n";
      break;

    case 40:
      str+= "is god's appointed ruler of the world.\n";
      break;

    case 50:
      str += "is the almighty god (beware of his wrath).\n";

    }
  }
  if(this_player(1) && this_player(1)->query_level()>21)
  {
    if (secondchar) 
      str += (capitalize(name) + " is a secondchar (" + secondchar + ").\n");
    if (testchar) 
      str += (capitalize(name) + " is a testchar (" + testchar + ").\n");
  }
  else
    if(this_player(1) && this_player(1)->query_level()>19)
    {
      if (secondchar) 
	str += (capitalize(name) + " is a secondchar.\n");
      if (testchar) 
	str += (capitalize(name) + " is a testchar.\n");
    }
      
  if (ob)
  {
    if(npc ||
       (interactive(ob) &&
	!("obj/master"->is_hard_invis(ob)) &&
	(ob->short() || this_player(1)->query_level() > 19)))
    {
      str += capitalize(PRONOUN(gender)) + " is logged on right now and has been so for "+age(time()-_object_stat(ob)[0])+".\n";
    }else{
      str += capitalize(PRONOUN(gender)) + " is at the moment linkdead.\n";
    }
  }else{
    if(last_login_time_int)
      {
	if(last_logout_time_int)
	  {
	    str+="Last seen "+ctime(last_logout_time_int)+" on for "+age(last_logout_time_int-last_login_time_int)+".\n";
	    tmp=time()-last_logout_time_int;
	    if(tmp<0)
	      str+=capitalize(PRONOUN(gender))+" is proberly linkdead.\n";
	    else
	      str+="This was "+age(tmp)+" ago.\n";
	  }
	else	  
	  str+="The last time "+PRONOUN(gender)+" logged on was "+ctime(last_login_time_int)+".\n";
      }
    else
      {
	if(last_login_time)
	  str += "The last time " + PRONOUN(gender) + " logged on was " +
	    last_login_time + ".\n";
      }
  }
  if (!plan_desc)
  {
    str += name + " has no plan.\n";
  }else{
    str += name + "'s plan is:\n" + plan_desc;

    if(plan_desc[-1]!='\n')  str += "\n";
  }
  return str;
}

mixed get_player_stats(string name)
{
  mixed _secondchar;
  mixed _testchar;  
  if (restore(name))
  {
    if(this_player(1))
      {
	switch(this_player(1)->query_level()) 
	{
	case 20..21:
	  _secondchar=!!secondchar;
	  _testchar=!!testchar;
	  break;
	case 22..50:
	  _secondchar=secondchar;
	  _testchar=testchar;
	  break;
	default:
	  _secondchar=0;
	  _testchar=0;
	  break;
	}
      }
    else
    {      
      _secondchar=0;
      _testchar=0;
    }
    return
      ({
	name,
	title,
	al_title,
	plan_desc,
	money,
	level,
	max_hp,
	max_sp,
	age,
	Str,
	Int,
	Con,
	Dex,
	my_quest_max,
	gender,
	last_login_time,
	mailaddr,
	playerkilling,
	called_from_ip,
	called_from_host,
	experience,
	_testchar,
	_secondchar,
        last_login_time_int,
	last_logout_time_int,
	wwwaddr,
	quests,
      });
  }
  return 0;
}

int wizardp(string s)
{
  return restore(s) && level > 19;
}

int playerp(string s) { return file_size("players/"+s+".o") > 0; }

string *all_player_files()
{
  return get_dir("/players/*.o");
}

int query_prevent_shadow() { return 1; }
