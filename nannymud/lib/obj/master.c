#pragma strict_types

/* MOVE THIS FILE TO /obj/master.c !
 *
 * This is the LPmud master object, used from version 3.0.
 * It is the second object loaded after void.c.
 * (But these days, 950813, it is the first object loaded at all... )
 * Everything written with 'write()' at startup will be printed on
 * stdout.
 * 1.  reset() will be called first.
 * 2. flag() will be called once for every argument to the flag -f
 * 	supplied to 'parse'.
 * 3. epilog() will be called.
 * 4. The game will enter multiuser mode, and enable log in.
 *
 * This version of master.c is specifically designed to be used with mudlib
 * 2.4.6 or earlier. The master.c for mudlib 3.0 will be supplied with
 * that source.
 *
 * One thing is different in the file access permission system in game
 * driver 3.0. The master object can read and write any file.
 * That means that all occurences of such
 * manipulations below must be carefully garded against calls from
 * elsewhere. Always make a comment at the start of a function that is doing
 * such things, to make it easy for other to quickly assert security.
 * The master object can of course not access files above the mudlib
 * directory.
 */
#ifdef AMY
void log_file(string s, string t)
{
   return;
}

string creator_file(string o)
{
  return "idi";
}
#endif

/* To test a new function xx in object yy, do
 * parse "-fcall yy xx arg" "-fshutdown"
 */

/* This is ugly, and may be hazardous to your health. */

nomask int is_hard_invis(mixed o)
{
  return o &&
    (int)o->is_hard_invis() &&
      (o=(int)o->query_level())>22 && 
	(!this_player(1) ||
	 (int)this_player(1)->query_level()<o);
}

void flag(string str)
{
   string file, arg;
   int num_arg;
   if(sscanf(str, "for %d", num_arg) == 1)
      write("Got : " + catch(this_object()->xx(num_arg)) + "\n");
   else if(str == "shutdown")
      shutdown();
   else if(sscanf(str, "dhrystone %d", arg) == 1)
      call_other("players/lars/dry", "main", arg);
   else if(sscanf(str, "echo %s", arg) == 1)
      write(arg + "\n");
   else if(sscanf(str, "call %s %s", file, arg) == 2)
   {
      arg = (string)call_other(file, arg);
      write("Got " + arg + " back.\n");
   }
   else
      write("master: Unknown flag " + str + "\n");
}

string get_bb_uid() { return "backbone"; }

/* This function is called every time a player connects.
 * input_to() can't be called from here.
 */
object connect()
{
   object ob;
   string ret;
#ifdef FAST
   return clone_object(file_name(this_object()));
#else
   if(ret = (string) catch(ob = clone_object("obj/player")))
   {
      write("** Internal Error: " + ret + "\n");
      return 0;
   }
#endif
   return ob;
}

#ifdef FAST
void logon()
{
   enable_commands();
   set_living_name("idi");
   add_action("do_exec", "", 1);
}

int do_exec(string str)
{
   mixed ret;
   object ob;
   rm("/syslog/tmp_file.c");
   write_file("/syslog/tmp_file.c", "run() {" + query_verb() + " " + 
	      (str ? str : "") + "}");
   if(ob = find_object("/syslog/tmp_file"))
      destruct(ob);
   ret = (mixed) call_other("/syslog/tmp_file", "run");
   write("\nReturn: ");
   write(ret);
   write("\n");
   return 1;
}
#endif

int xx(int arg)
{
   return 1/arg;
}

/* This function is called for a wizard that has dropped a castle.
 * The argument is the file name of the object that called create_wizard().
 * Verify that this object is allowed to do this call.
 */
int verify_create_wizard(object ob)
{
   int dummy;
   if(sscanf(file_name(ob), "obj/leo#%d", dummy) == 1)
      return 1;
   return 0;
}

/* Get the owner of a file. This is called from the game driver, so as
 * to be able to know which wizard should have the error.
 */
string get_wiz_name(string file)
{
   string name, rest;
   if(sscanf(file, "players/%s/%s", name, rest) == 2)
     return name;
   if(sscanf(file, "guilds/%s/%s", name, rest) == 2)
     return name;
   return 0;
}

/* Write an error message into a log file. The error occured in the object
 * 'file', giving the error message 'message'.
 */
void log_error(string file, string message)
{
   string name;
   if(!(name = get_wiz_name(file)))
      name = "log";
   write_file("/log/" + name, message);
}

/* save_ed_setup and restore_ed_setup are called by the ed to maintain
   individual options settings. These functions are located in the master
   object so that the local gods can decide what strategy they want to use.
   suggestions:
	A setup file for every wizard.
		advantages:	transparent to the user
				independent of wizard count
		disadvantage:	extra file access at ed invocation
	An array in the master object, wizards are searched by member_array
		advantage:	easy to implement
		disadvantage:	performance degradation with high wizard counts
	An AVL-tree to access wizards by name
		advantage:	can fit any need
		disadvantage:	hard to implement, will need more overhead on
				small and medium muds than it can ever make
				good by lg(wizcount) complexity
	Dedicated flags in every wizard object, inherited from /obj/living
		advantages:	easy to implement ( as shown here)
				independent of wizard count
				Will also work for nonm-wizards.
		disadvantage:	care has to be taken to avoid collision with
				other uses of the /obj/living flags.          */

#if 0 /* Dedicated flags not used. A save file method is used below */

#define FIRST_ED_FLAG 0 /* adjust this value so that no flags are clobbered */
#define NUM_ED_FLAGS 10
int save_ed_setup(object wiz, int setup)
{
   int mask,i;
   for(mask=1,i=FIRST_ED_FLAG;i<FIRST_ED_FLAG+NUM_ED_FLAGS;mask<<=1,i++)
   {
      if(setup & mask)
	 wiz->set_flag(i);
      else
	 wiz->clear_flag(i);
   }
   return 1; /* function is defined, success */
}

int retrieve_ed_setup(object wiz)
{
   int i,setup;
   for(i = FIRST_ED_FLAG + NUM_ED_FLAGS; i > FIRST_ED_FLAG;)
      setup += setup + wiz->test_flag(--i);
   return setup;
}
#else

/* The wizard object 'who' wants to save his ed setup. It is saved in the
 * file /players/wiz_name/.edrc . A test should be added to make sure it is
 * a call from a wizard.
 *
 * Don't care to prevent unauthorized access of this file. Only make sure
 * that a number is given as argument.
 */
int save_ed_setup(object who, int code)
{
   string file;
   if(!intp(code))
      return 0;
   file = "/players/" + lower_case((string)who->query_name()) + "/.edrc";
   rm(file);
   return write_file(file, code + "");
}

/* Retrieve the ed setup. No meaning to defend this file read from
 * unauthorized access.
 */
int retrieve_ed_setup(object who)
{
   string file;
   int code;
   file = "/players/" + lower_case((string)who->query_name()) + "/.edrc";
   if(file_size(file) <= 0)
      return 0;
   sscanf(read_file(file), "%d", code);
   return code;
}
#endif

/* Create a home directory and a castle for a new wizard. It is called
 * automatically from create_wizard(). We don't use the 'domain' info.
 * The create_wizard() efun is not really needed any longer, as a call
 * could be done to this function directly.
 * The wizard gets a default workroom too.
 *
 * This function can create directories and files in /players. It is
 * guarded from calls from the wrong places.
 */
string master_create_wizard(string owner, string domain, object caller)
{
  string def_castle, def_workroom;
  string dest, castle, workroom, wizard;
  object player;
  if(!(player = find_player(owner)))
    return 0;
  if(!verify_create_wizard(caller))
  {
    tell_object(player, "That is an illegal attempt!\n");
    return 0;
  }
  if(caller != previous_object())
  {
    tell_object(player, "Faked call!\n");
    return 0;
  }
  wizard = "/players/" + owner;
  castle = "/players/" + owner + "/castle.c";
  workroom = "/players/" + owner + "/workroom.c";
  if(file_size(wizard) == -1)
  {
    tell_object(player, "You now have a home directory: " +
		wizard + "\n");
    mkdir(wizard);
      
  }
  dest = "room/castle_room";	/* Profezzorn */
  def_castle = "#define NAME \"" + owner + "\"\n#define DEST \"" +
    dest + "\"\n" + read_file("/room/def_castle.c");
  if(file_size(castle) > 0)
    tell_object(player, "You already had a castle !\n");
  else
  {
    /* The master object can do this ! */
    if(write_file(castle, def_castle))
    {
      tell_object(player, "You now have a castle: " + castle + "\n");
      if(!write_file("/room/init_file", "#"+ extract(castle, 1) + "\n"))
	tell_object(player, "It will not be loaded automatically until a 23++ enables it.\n");
    }
    else
      tell_object(player, "Failed to make a castle for you!\n");
  }
  /* Give the newbie wiz a workroom too! Padrone, July 31, 1992 */
  def_workroom = "#define NAME \"" + owner + "\"\n" + read_file("/room/def_workroom.c");
  if(file_size(workroom) > 0)
    tell_object(player, "You already had a workroom !\n");
  else
  {
    /* The master object can do this ! */
    if(write_file(workroom, def_workroom))
      tell_object(player, "You now have a workroom: " + workroom + "\n");
    else
      tell_object(player, "Failed to make a workroom for you!\n");
  }


  if(!write_file(wizard+"/.acl",capitalize(owner)+": ALL\n"+
		wizard+"/: ALL\n"+
		"$REST: NONE\n"))
  {
    tell_object(player, "Failed to set directory protection!\n");
  }
  return castle;
}

/* When an object is destructed, this function is called with every
 * item in that room. We get the chance to save players !
 */
void destruct_environment_of(object ob)
{
  if(!interactive(ob))
    return;
  tell_object(ob, "Everything you see is disolved. Luckily, you are transported somewhere...\n");
  ob->move_player("is transfered#room/void");
}

/* Define where the '#include' statement is supposed to search for files.
 * "." will automatically be searched first, followed in order as given
 * below. The path should contain a '%s', which will be replaced by the file
 * searched for.
 */
string *define_include_dirs() { return ({ "/include/%s" }); }

/* The master object is asked if it is ok to shadow object ob. Use
 * previous_object() to find out who is asking.
 *
 * In this example, we allow shadowing as long as the victim object
 * hasn't denied it with a query_prevent_shadow() returning 1.
 *
 * It is not a good idea to allow shadows on vcompile since this
 * changes filename on objects. This patch here until it's fixed
 * in the driver or so. Brom 960219 
 *
 */
int query_allow_shadow(object ob)
{
  if (sscanf(file_name(previous_object()), "%*svcompile%*s") == 2)
    return 0;
  return ((previous_object() &&
	   extract(file_name(previous_object()), 0, 6) == "secure/") ||
	  !ob->query_prevent_shadow(previous_object()));
}

int query_prevent_shadow() { return 1; }   /* Angmar */

/* This moves the responsibility for the security check to the
   object in /local/. Do NOT put objects in /local/ that uses
   exec() without rigorous security checks. It is not enough
   to check the identity of the caller, ie this_player(). You
   must check the arguments to exec().
    /Angmar   920219
*/
int valid_exec(string fnam)
{
   return (sscanf(fnam, "local/%*s") == 1 ||
	   sscanf(fnam, "secure/%*s") == 1 ||
	   sscanf(fnam, "obj/player#%*s") == 1 ||
	   (this_player(1) &&
	   this_player(1)->query_level()>22));   /*  Gwendolyn  */
}

int valid_disconnect()
{
  string fnam;
  if(!previous_object() || previous_object()==this_object())
    return 0;
  fnam=file_name(previous_object());

/*  log_file("DISCONNECT",fnam+"\n"); */

  return (sscanf(fnam, "local/%*s") == 1 ||
	  sscanf(fnam, "secure/%*s") == 1 ||
	  sscanf(fnam, "obj/player#%*s") == 1 ||
	  (this_player(1) &&
	   this_player(1)->query_level()>22));

}

/* Give a file name for the currently edited edit file in. */
/* This function may under no circumstances have errors in it,
 * as that can crash the mud. /Profezzorn
 */
string get_ed_buffer_save_file_name(string file)
{
   string *file_ar;
   file_ar=explode(file,"/");
   file=file_ar && sizeof(file_ar) ? file_ar[sizeof(file_ar)-1] : "unknown";
   if(this_player())
     return "/players/"+this_player()->query_real_name()+"/.dead_ed_files/"+file;
   else
     return "/open/dead_ed_files/"+file;
}

/* Give a path to a simul_efun file. Observe that it is a string returned,
 * not an object. But the object has to be loaded here. Return 0 if this
 * feature isn't wanted.
 */
string get_simul_efun()
{
   string fname;
   string error;
   fname = "/obj/simul_efun";
   if(error = catch(call_other(fname, "??")))
   {
     write("Failed to load " + fname + "\nError: " + error + "\n");
     shutdown();
     return 0;
   }
   rm("/etc/NOLOGIN");
   return fname;
}

int query_player_level()
{
   int tmp;
   if(this_player() && intp(tmp = (int) this_player()->query_level()) && tmp > 19)
      return 1;
   return 0;
}

/* Return 1 if that is a valid command() target */
int valid_command(object caller, object target)
{
   if(target == caller)
      return 1;
   if(target && interactive(target) && target->query_level_sec() >= 20 &&
      (!caller || !interactive(caller) ||
       caller->query_level_sec() <= target->query_level_sec()))
   {
      return 0;
   }
   return 1;
}

int rmlogg() { return rm("/players/angmar/otime"); }
int logg(string s) { return write_file("/players/angmar/otime",s); }

int nologin_write(string msg)
{
   if(!previous_object() || file_name(previous_object()) == "obj/shut")
   {
      rm("/etc/NOLOGIN");
      return write_file("/etc/NOLOGIN", msg);
   }
   else
      return 0;
}

/* vem i &&%%&% hade kommenterat bort nologin_rm() ? /Profezzorn */

int nologin_rm()
{
   if(!previous_object() || file_name(previous_object()) == "obj/shut")
      return rm("/etc/NOLOGIN");
   else
      return 0;
}

int deamon_file_size(string file) 
{
   if(previous_object() && file_name(previous_object())[0..10] == "obj/daemon/")
      return file_size(file);
   return -1;
}

int daemon_rm(string file)
{
   if(previous_object() && file_name(previous_object())[0..10] == "obj/daemon/")
      return rm(file);
   else
      return 0;
}

int daemon_write(string file, string str)
{
   if(previous_object() && file_name(previous_object())[0..10] == "obj/daemon/")
      return write_file(file, str);
   else
      return 0;
}

string daemon_read(string file)
{
   if(previous_object() && file_name(previous_object())[0..10] == "obj/daemon/")
      return read_file(file);
   else
      return 0;
}

int daemon_mkdir(string file)
{
   if(previous_object() && file_name(previous_object())[0..10] == "obj/daemon/")
      return mkdir(file);
   else
      return 0;
}

string query_mudwho_name(object ob)
{
   string str;
   str =(string)"/obj/daemon/countryd"->query_country_name(ob->query_host());
   if(query_idle(ob) > 10*60)
      str += ", I";
   return str;
}

string query_daemon_read(string file, int write_flag)
{
   string from;
   if(!previous_object())
      return 0;
   if(file_name(previous_object())[0..3] == "obj/")
      return file;
}

status valid_socket(object ob, string func, mixed *info) 
{
   return (previous_object() &&
	   (extract(file_name(previous_object()), 0, 10) == "obj/daemon/" ||
	    extract(file_name(previous_object()), 0, 6) == "secure/"));
}


string *daemon_get_dir(string dir)
{
  string from;
  if(!previous_object())
    return 0;
  if(file_name(previous_object())[0..6] == "obj/dae")
    return get_dir(dir);
}

status valid_unix_exec(object ob) 
{
   return (previous_object() &&
	   (extract(file_name(previous_object()), 0, 10) == "obj/daemon/" ||
	    extract(file_name(previous_object()), 0, 6) == "secure/"));
}

mixed *to_call=({});

status add_reboot_callback(object o, string fun) 
{
  to_call+= ({ o, fun }); 
  return 1;
}

void shut_down_game()
{
  int i;
  if(i=sizeof(to_call))
    while(i--)
      catch(call_other(to_call[i][0],to_call[i][1]));
}

void epilog()
{
  /* For uptime statistics */
  log_file("STARTS",time()+"\n");
}

void _exit()
{
  string a;
  lower_case(a); /* Error, do not destruct the master /Profezzorn */
}

/* Virtual objects:
 * This function will get called when trying to load an object
 * from a nonexistant file. The argument given is the filename
 * with the leading '/' and any trailing '.c' stripped. If you
 * return an object it will be returned as having the given
 * name (and will also get that name). Do _not_ return the
 * same object for more than one name (clones of the same
 * object is alright) since it will be fiddled with...
 * Do _not_ make any changes here!
 * See /doc/LPC/virtual for a usage description
 */
object get_vobject(string fname)
{
   object obj;
   switch(fname)
   {
      /* Test... Do _not_ add anything more here! */
      case "foo/bar":
	 if(catch(obj = clone_object("/obj/bag")))
	    obj = 0;
	 break;
      default:
	 if(catch(obj = "/secure/virtual"->get_vobject("/" + fname)))
	    obj = 0;
	 break;
   }
   if(!objectp(obj) || environment(obj) || obj->query_prevent_virtual(fname))
      return 0;
   else
      return obj;
}

int do_chmod(string fname)
{
   if(!stringp(fname) || !previous_object() ||
      (extract(file_name(previous_object()), 0, 6) != "secure/" &&
       extract(file_name(previous_object()), 0, 10) != "obj/daemon/"))
      return 0;
   if(!_chmod(fname, 6 * 8 * 8 + 6 * 8 + 4))
      return 0;
   else
      return 1;
}
