#include <log.h>

#define MAX_PLAYERS    50
#define MAX_ARCHWIZ     5
#define PLAYEROBJECT   "/secure/player"

#pragma strict_types
#pragma save_types

string name, password, allow_from_host;
int level, is_invis;

static object other_copy, playerobj;
static string ident_user;
static int nologin, newplayer;

mixed query_prevent_shadow() { return 4711; }
nomask int query_level() { return 99; }

void reset(status flag)
{
   if(flag)
      return;
   if(creator(this_object()))
   {
      destruct(this_object());
      return;
   }
   level = -1;
   gender = -1;
}

static status filter_users_fun(obj, level)
{
   if(level > 0)
      return (obj && (int) obj->query_level() >= level);
   else
      return (obj && (int) obj->query_level() <= -level);
}

object *filter_users(level)
{
   return filter_objects(users(), "filter_users_fun", this_object(), level);
}

status valid_name(string str)
{
   int i, length;
   if(str == "logon")
   {
      write("Invalid name");
      return 0;
   }
   length = strlen(str);
   if(length > 11)
   {
      write("Too long name.\n");
      log_file("BAD_NAME", str + "\n");
      return 0;
   }
   for(i = 0; i < length; i++)
   {
      if(str[i] < 'a' || str[i] > 'z')
      {
	 write("Invalid characters in name:" + str + "\n");
	 write("Character number was " + (i+1) + ".\n");
	 return 0;
      }
   }
   return 1;
}

static status check_access()
{
   string ip;
   string a, b, c, d;
   ip = query_ip_number();
   if(!allow_from_host)
      return 1;
   if(member_array(ip, allow_from_host) != -1)
      return 1;
   sscanf(ip, "%s.%s.%s.%s", a, b, c, d);
   if(member_array(a + "." + b + "." + c + ".*", allow_from_host) != -1)
      return 1;
   if(member_array(a + "." + b + ".*.*", allow_from_host) != -1)
      return 1;
   if(member_array(a + ".*.*.*", allow_from_host) != -1)
      return 1;
   if(member_array("*.*.*.*", allow_from_host) != -1)
      return 1;
   return 0;
}

void fiddle_obj(object obj)
{
   int weight;
   weight = (int) obj->query_weight();
   if(!obj->id("soul") && !obj->id("magobj") && playerobj->add_weight())
   {
      obj->drop(1);
      if(obj)
	 move_object(obj, playerobj);
   }
}

static void move_player_to_start3()
{
   if(newplayer)
      save_object("players/" + name);
   if(!is_invis || level <= 21)
      catch("/obj/daemon/mudwhod"->send_login(this_object()));
   playerobj->logon(name, password);
}

nomask static void try_throw_out(string str)
{
   object ob, next_ob;
   if(str == "" || (str[0] != 'y' && str[0] != 'Y'))
   {
      write("Welcome another time then !\n");
      _destruct(this_object());
   }
   for(ob = first_inventory(other_copy); ob; ob = next_ob)
   {
      string err;
      next_ob = next_inventory(ob);
      if(err = catch(fiddle_obj(ob)))
      {
	 _syslog("try_throw_out(): buggy object: " + err + ".");
	 _destruct(ob);
      }
   }
   ob = environment(other_copy);
   other_copy->save_me(2);
   if(other_copy)
      _destruct(other_copy);
   restore_object("players/" + name);
   other_copy = 0;
   move_player_to_start3();
}

static void move_player_to_start2()
{
   string err;
   if((err = catch(playerobj = clone(PLAYEROBJECT))) || !playerobj)
   {
      write("*** Internal error!\n");
      _destruct(this_object());
   }
   if((other_copy = find_player(name)) || (other_copy = find_living(name)))
   {
      if(query_ip_number(other_copy))
      {
	 write("You are already playing!\n");
	 write("Throw the other copy out ? ");
	 input_to("try_throw_out");
      }
      else
      {
	 write("You are already playing, I will remove your statue.\n");
	 try_throw_out("yes");
      }
      return;
   }
   move_player_to_start3();
}

static void getgender(string gender_string)
{
   gender_string = lower_case(gender_string);
   if(gender_string[0] == 'm')
   {
      write("Welcome, Sir!\n");
      gender = MALE_GENDER;
   }
   else if(gender_string[0] == 'f')
   {
      write("Welcome, Madam!\n");
      gender = FEMALE_GENDER;
   }
   else if(gender_string[0] == 'o')
   {
      write("Welcome, Creature!\n");
      gender = NEUTER_GENDER;
   }
   else
   {
      write("Sorry, but that is too weird for this game!\n");
      write("Are you, male, female or other (type m, f or o): ");
      input_to("getgender");
      return;
   }  
   move_player_to_start2();
}

static void move_player_to_start()
{
   if(gender == -1)
   {
      write("Are you, male, female or other: ");
      input_to("getgender", where);
      return;
   }
   move_player_to_start2();
}

static void new_password(string p)
{
   write("\n");
   if(!p || p == "")
   {
      write("Try again another time then.\n");
      _destruct(this_object());
   }
   if(strlen(p) < 6)
   {
      write("The password must have at least 6 characters.\n");
      input_to("new_password", 1);
      write("Password: ");
      return;
   }
   if(!password)
   {
      password = crypt(p, 0);
      if(level != -1)
	 input_to("check_password", 1);
      else
	 input_to("new_password", 1);
      write("Password: (again) ");
      return;
   }
   remove_call_out("time_out");
   if(crypt(p, password) != password)
   {
      write("You changed!\n");
      _destruct(this_object());
   }
   level = 0;
   "/room/adv_guild->advance(0);
   level = 1;
   move_player_to_start();
#ifdef LOG_NEWPLAYER
   log_file("NEWPLAYER", cap_name + " " + ctime(time()) + 
	    " from " + ( ident_user  ? ident_user +"@" : "") +
	    query_ip_name()+".\n");
#endif
}

static void check_password(string p)
{
   remove_call_out("time_out");
   write("\n");
   if(name != "guest" && crypt(p, password) != password)
   {
      write("Wrong password!\n");
      _destruct(this_object());
   }
   move_player_to_start();
#ifdef LOG_ENTER
   log_file("ENTER", name + ", " + extract(ctime(time()), 4, 15)+ 
	    " from "  + ( ident_user ? ident_user + "@" : "") +
	    query_ip_name() + ".\n");
#endif
}

nomask private void logon2(string str)
{
   if(!str || str == "")
   {
      write("Another time then.\n");
      _destruct(this_object());
   }
   if(name)
      return;
   name = lower_case(str);
   if(!valid_name(name))
   {
      input_to("logon2");
      write("Not a valid name. Give a new name: ");
      return;
   }
   if(restore_object("banish/" + name))
   {
      write("That name is reserved.\n");
      _destruct(this_object());
   }
   if(!restore_object("players/" + name))
   {
      write("New character.\n");
      newplayer = 1;
   }
   ident_user = _query_ident();
   if(nologin && level <= 21)
   {
      write("Sorry, login has been limited to Arch Wizards currently.\n");
      write("Please come back later.\n");
      _destruct(this_object());
   }
   if(!check_access())
   {
      write("You may not login from this site!\n");
      log_file("CRACKER", name + ", " + extract(ctime(time()), 4, 15) +
	       " from " + ( ident_user ? ident_user +"@" : "") +
		 query_ip_name()+".\n");
      _destruct(this_object());
   }
   if(level == -1 || name == "guest")
   {
      if(!"/obj/daemon/newbied"->allowed_newbie(query_ip_number(this_object()),
					       name))
      {
	 if(name == "guest")
	    write("Guest login denied from your host.\n");
	 else
	 {
	    write("New players are not allowed to login from your site.\n");
	    write("Please mail nannymud@lysator.liu.se if you wish to start playing.\n");
	    write("State the name of the character you wish to play in the letter.\n");
	 }
	 _destruct(this_object());
      }
   }
   if(name == "guest")
   {
      check_password("");
      return;
   }
   else if(level != -1 && password)
      input_to("check_password", 1);
   else
   {
      if(level != -1)
	 write("You have no password! Set one now.\n");
      input_to("new_password", 1);
   }
   write("Password: ");
}

/* logon() is called when the players log on. */
nomask static void logon()
{
   int num_users;
   write("Welcome to the NannyMUD multiuser adventure running on NannyMOS " +
	 version() + "\n");
   if(sizeof(users()) >= MAX_PLAYERS + MAX_ARCHWIZ)
   {
      write("*** NannyMUD is full. Please come back later.\n");
      _destruct(this_object());
   }
   num_users = sizeof(users()) - sizeof(filter_users(22));
   if(nologin = cat("/etc/NOLOGIN"))
      write("*** Login limited to Arch Wizards ***\n");
   else if(nologin = (num_users >= MAX_PLAYERS))
      write("*** NannyMUD is full. Only Arch Wizards are allowed to login.\n");
   else
      cat("/etc/WELCOME");
   ident_user = _query_ident();
   if(name == "logon")
   {
      write("What is your name: ");
      input_to("logon2");
      call_out("time_out", 120);
   }
}

/* Make it possible to 'become' someone else */
nomask void do_su(string newname)
{
   if(this_player(1) && (int) this_player(1)->query_level() > 21)
      logon2(newname);
}
