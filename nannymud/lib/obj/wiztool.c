/*
** wiztool.c   The NannyMUD wizards tool addendum, a must-have for all wizards!
**
** Extracted from player.c and modified by Peter Eriksson <pen@lysator.liu.se>
*/

#include <acl.h>

#define MAX_IDLE    30*60


string change_dir, change_file, current_path;
status file_exp;

void reset(status arg)
{
    if (arg)
	return;

    current_path = (string) this_player()->getenv("HOME");
    if(!current_path)
	current_path = "players/" + (string) this_player()->query_real_name();
}

int get() { return 1; }
int drop() { return 1; }

/* if you change this, don't forget to change query_wiztool() in player.c */
int id(string str)
{
  return str == "wizsoul" || str == "wizard's soul";
}

static void log(string s)
{
  "/obj/daemon/syslogd"->do_log(s);
}

mixed *get_flags(string str, string b)
{
  string flags;
  int e;
  mixed *q,*p;

  p=allocate(256);
  q=allocate(strlen(b)+1);
  if(!str) str="";

  for(e=0;e<strlen(b);e++) p[b[e]]=1;

  q[0]=str;
  while(sscanf(str,"-%s",flags))
  {
    if(!sscanf(flags,"%s %s",flags,str))
      str="";

    for(e=0;e<strlen(flags);e++)
    {
      if(!p[flags[e]]) break;
      p[flags[e]]++;
    }
    if(e<strlen(flags)) break;
    q[0]=str;
  }

  for(e=0;e<strlen(b);e++) q[e+1]=p[b[e]]-1;
  return q;
}

status send_to(string s)
{
  string who,where,what;
  object daemon;
  daemon=load_object("/obj/daemon/interwiz");
  if(!daemon)
  {
    write("I cannot find the daemon!\n");
    return 1;
  }
  if(!s)
    return 0;
  sscanf(s,"%s@%s %s",who,where,what);
  daemon->user_write("send	"+where+"	to	"+who+"	"+
		     capitalize((string)this_player()->query_real_name())+
		     "@NannyMUD: "+what+"\n");
  write("Ok. Sending to "+who+" on the mud "+where+"...\n");
  return 1;
}

status iwiz(string s)
{
  object daemon;
  daemon=load_object("/obj/daemon/interwiz");
  if(!daemon)
  {
    write("I cannot find the daemon!\n");
    return 1;
  }
  daemon->user_write("bcast	"+capitalize((string)
					     this_player()->query_real_name())+
		     "@NannyMUD: "+s+"\n");
  daemon->wiz(capitalize((string)this_player()->query_real_name())+
	      "@NannyMUD: "+s,"iwiz");
  write("Ok.\n");
  return 1;
}

/*
   If you add a new command remeber to update any help files,
   esp. /doc/w/wizsoul
   */

void init()
{
  /*
     object o;
     while(o=present("wizsoul 2",environment()))
     destruct(o);
     */
  
  if (this_player()->query_level() < 20 || !interactive(this_player()))
    return;
  
  if(this_player()->query_level()>20) /* Cel 94 */
  {
    add_action("makedir", "mkdir");
    add_action("removedir", "rmdir");
    add_action("echo_all", "echoall");
    add_action("edit", "ed");
    add_action("force_player", "force");
    add_action("cp", "cp");
    add_action("cp", "mv");
    add_action("list_access", "list_access");
    add_action("list_access", "lac");
    add_action("set_access", "set_access");
    add_action("set_access", "sac");
    add_action("edit_access", "edit_access");
    add_action("edit_access", "edac");
    add_action("lockfile", "lockfile");
    /*
       Normal wizards don't want this.
       /Slater
    */
    if(this_player()->query_level()>21) 
      add_action("shut_down_game", "shutdown");
  }
  /*
     add_action("send_to","send_to");
     add_action("iwiz","iwiz");
    */
  add_action("local_commands", "localcmd");
  add_action("wiz_score_list", "wizlist");
  add_action("spell_zap", "zap");
  add_action("stat", "stat");
  add_action("heal", "heal");
  add_action("tail_file", "tail");
  add_action("cd", "cd");
  add_action("changed_dir","updir");
  add_action("changed_dir","loaddir");
  add_action("refresh","refresh");
  add_action("update_object", "update");
  add_action("set_title", "title"); /* Diff for 20 and 21 ,Cel */
  add_action("teleport", "goto");
  add_action("in_room", "in");
  add_action("emote", "emote");
  add_action("list_peoples", "people");
  add_action("trans", "trans");
  add_action("snoop_on", "snoop");
  add_action("toggle_file_exp","fileexp");
  add_action("set_env","set");
  add_action("set_env","setenv",1);
  add_action("unset_env","unset");
  add_action("unset_env","unsetenv",1);
    
  add_action("pwd", "pwd");
  add_action("more", "more");
  add_action("echo_to", "echoto");
  add_action("echo", "echo");
  add_action("home", "home");
  add_action("remove_file", "rm");
  add_action("list_files", "ls");
  add_action("cat_file", "cat");
  add_action("clone", "clone");
  add_action("destruct_local_object", "destruct");
  add_action("load", "load");
  add_action("vis_2","VIS");
  add_action("man","man");
  add_action("man","Syntax");
  add_action("cmd_owner", "owner");
  add_action("change_light", "light");
  add_action("cmd_uptime", "uptime");
  add_action("cmd_lookupp","lookupp");
  add_action("isareaclosed","areaclosed");
  if (this_player()->query_level() > 22) {
    add_action("areaclose","closearea");
    add_action("areaopen","openarea");
  }
}
#define AREA_D "/obj/daemon/area_d"

sort_closed(path1,path2) {
  return path1 > path2;
}
  

isareaclosed(str) {
  mixed res;
  if (!str) {
    mixed array;
    int max;
    max = sizeof(array = m_indices(AREA_D->query_closed_areas() || ([ ])));
    if (!max) {
      write("There are no closed areas.\n");
      return 1;
    }
    write("The following paths are considered closed:\n");
    array = sort_array(array,"sort_closed",this_object());
    write(sprintf("%#-80s\n",implode(array,"\n")));
    return 1;
  }
  str = mk_path(str);
  if (res = AREA_D->_query_closed(str)) {
    write(str+": is closed.\n");
    if (!AREA_D->query_closed_areas(str))
      write("Due to the fact that: "+res+" is closed.\n");
    return 1;
  }
  write(str+": is open.\n");
  return 1;
}

/* Qqqq, 960115 */
static areaclose(str) {
  string res;
  if (!str)
    return isareaclosed();
  str = mk_path(str);
  res = AREA_D->add_closed_area(str);
  if (stringp(res)) {
    write(str+": is closed, but it was already closed because:\n"+res+" was already closed.\n");
    return 1;
  }
  switch(res) {
  case 0:
    write("Sorry you were not able to close the area:\n"+str+"\n");
    return 1;
  case -1:
    write(str+": is already closed.\n");
    return 1;
  case 1:
    write(str+": is now closed.\n");
    return 1;
  default:
  write("BUG!\n");
  return 1;

  }
  write("BUG!\n");
  return 1;
}

static areaopen(str) {
  string res;
  if (!str)
    return isareaclosed();
  str = mk_path(str);
  res = AREA_D->remove_closed_area(str);
  if (stringp(res)) {
    write(str+": is still closed, because of:\n"+res+".\n");
    return 1;
  }
  switch(res) {
  case 0:
    write("Sorry you were not able to open the area:\n"+str+"\n");
    return 1;
  case -1:
    write(str+": is already open.\n");
    return 1;
  case 1:
    write (str+": is now open.\n");
    return 1;
  default:
  write("BUG!\n");
  return 1;
  }
  write("BUG!\n");
  return 1;
}


/*
   Added by Taren, Date: Thu Dec 28 22:37:53 1995
 */
cmd_lookupp(str)
{
  object ob;
  ob=load_object("/room/prop_room");
  if(!ob)
    return write("Couldn't find the propery room.\n"),1;
  if(!objectp(ob))
    return write("The room was not an object!\n"),1;
  return ob->_lookupp(str);
}


vis_2()
{
  if(this_player()->query_level()<23)
  {
    write("It won't work for you. No matter HOW hard you try\n"+
	  "That's for sure. /The arches.\n");
    return 1;
  }
  this_player()->toggle_hard_invis();
  /*    if (!(this_player()->short())) write("And quite 'normal-invis'\n"); */
  return 1;
}


/* **************************** Begin ACL commands *************************** */

static acl_wren(mixed *aclen)
{
    write("\t" + capitalize(aclen[0]) + ":\t");
    if (strlen(aclen[0]) < 7)
	write("\t");
    if (strlen(aclen[0]) < 15)
	write("\t");
    write("" + _acl_num2str(aclen[1]) + "\n");
}

static acl_print(mixed *ael)
{
    int i;

    for (i = 0; i < sizeof(ael); i++)
	acl_wren(ael[i]);
}


static mixed str2ace(string str)
{
    string obj;
    string rights;
    int num;
    mixed ace;
    
    
    if (!str || !sscanf(str, "%s:%s", obj, rights))
	return 0;

    num = _acl_str2num(rights);
    if (num < 0)
	return 0;
    
    return ({ obj, num });
}

static mixed mk_acl(string acl_str)
{
    mixed acl;
    string ace_str;
    string rest;
    mixed ace;
    

    if (!acl_str)
	return 0;
    
    acl = ({ 0, ACL_NONE, ({}) });

    while (sscanf(acl_str, "%s %s", ace_str, rest))
    {
	ace = str2ace(ace_str);
	if (ace == 0)
	    return 0;
	
	acl[2] += ({ ace });
	acl_str = rest;
    }

    if (stringp(acl_str))
    {
	ace = str2ace(acl_str);
	if (ace == 0)
	    return 0;
	
	acl[2] += ({ ace });
    }

    return acl;
}


static set_access(args)
{
    string path;
    string path2;
    mixed acl;
    

    if (!args || args == "" || args == "-help")
    {
	write(
"Usage: sac <target-pathname> <object>:[<rights>] [..<object>:[<rights>]]\n"+
"                           | -default\n"+
"                           | -like <source-pathname>\n"	      
	      );
	return 1;
    }
    
    if (sscanf(args, "(%s) %s", path, args))
    {
	int i;
	
	path = explode(path, " ");
	for (i = 0; i < sizeof(path); i++)
	    set_access(path[i] + " " + args);
	return 1;
    }

    if (!sscanf(args, "%s %s", path, args))
    {
	write("sac: missing argument(s)\n");
	return 1;
    }
    

    path = mk_path(path);
    if (path == 0)
    {
	write("sac: Illegal target pathname.\n");
	return 1;
    }
	

    if (args == "-default")
    {
	/* Set default ACL protection */

	if (!_acl_put(0, path))
	{
	    write("sac: Failed to assign default ACL protection.\n");
	    return 1;
	}
	write("Ok.\n");
	return 1;
    }
    else if (sscanf(args, "-like %s", path2) == 1)
    {
	path2 = mk_path(path2);
	if (path2 == 0)
	{
	    write("sac: Illegal source pathname.\n");
	    return 1;
	}
	
	acl = _acl_get(path2);
	if (acl == 0)
	{
	    write("sac: Failed to get ACL protection for \"" + path2 + "\".\n");
	    return 1;
	}
	
	if (!_acl_put(acl, path))
	{
	    write("sac: Failed to assign ACL protection on \"" + path + "\".\n");
	    return 1;
	}
	
	write("Ok.\n");
	return 1;
    }
    else
    {
	acl = mk_acl(args);
	if (acl == 0)
	{
	    write("sac: Illegal ACL list.\n");
	    return 1;
	}
	
	if (!_acl_put(acl, path))
	{
	    write("sac: Failed to assign ACL protection on \"" + path + "\".\n");
	    return 1;
	}

	write("Ok.\n");
	return 1;
    }
    

    return 0;
}

static list_access(path)
{
    mixed *acl;
    int show_rest;


    if (path == 0 || path == "")
	path = ".";
    
    path = mk_path(path);
    if (path == 0 || path == "")
	path = "/";

    show_rest = 0;
    acl = _acl_get(path);

    if (acl != 0)
    {
	if (stringp(acl[0]))
	    write("\"" + path +
		  "\" protected by default ACL (from \"" +
		  acl[0] + "\"):\n");
	else
	    write("ACL protecting \"" + path + "\":\n");

	acl_print(acl[2]);
	write("\t$REST:\t\t\t" + _acl_num2str(acl[1]) + "\n");
    }
    else
	show_rest = 1;
    
    acl = _acl_get("");
    if (acl != 0)
    {
	write("\nPriority ACL in effect for \"" + path + "\":\n");
	acl_print(acl[2]);
	if (show_rest)
	    write("\t$REST:\t\t\t" + _acl_num2str(acl[1]) + "\n");
    }

    return 1;
}


static modify_acl(acl, arg)
{
    int i;
    string objname;
    string rights;
    mixed *av;
    int nrights;
    

    if (!arg)
	return 0;

    if (!sscanf(arg, "%s:%s", objname, rights))
	if (!sscanf(arg, "%s:", objname))
	    return 0;
    
    objname = lower_case(objname);
    if (rights && rights != "")
	nrights = _acl_str2num(rights);
    else
	nrights = ACL_NONE;

    if (objname == "$rest")
	acl[1] = nrights;
    else
    {
	av = acl[2];
	for (i = 0; i < sizeof(av); i++)
	{
	    if (objname == lower_case(av[i][0]))
	    {
		if (rights && rights != "")
		{
		    acl[2][i][1] = nrights;
		}
		else
		{
		    acl[2] -= ({ acl[2][i] });
		}
		
		return acl;
	    }
	}
	
	acl[2] += ({ ({ objname, nrights }) });
    }

    return acl;
}


static edit_access(args)
{
    int i;
    mixed *acl;
    string path;


    if (!args || args == "" || args == "-help")
    {
	write(
"Usage: edac <target-pathname> <object>:[<rights>] [..<object>:[<rights>]]\n"
	      );
	return 1;
    }
	
    if (sscanf(args, "(%s) %s", path, args))
    {
	path = explode(path, " ");
	for (i = 0; i < sizeof(path); i++)
	    edit_access(path[i] + " " + args);
	return 1;
    }
    
    if (!sscanf(args, "%s %s", path, args))
    {
	write("edac: Missing target pathname or ACL list!\n");
	return 1;
    }

    path = mk_path(path);

    if (path == 0 || path == "")
	path = "/";

    acl = _acl_get(path);
    if (acl == 0)
    {
	write("edac: Failed to get ACL protecting \"" + path + "\".\n");
	return 1;
    }
	
    args = explode(args, " ");
    for (i = 0; i < sizeof(args); i++)
	acl = modify_acl(acl, args[i]);
    
#if 0
    if (stringp(acl[0]))
    {
	write("\"" + path + "\" is default-protected. Creat specific ACL? ");
	input_to("edac_2", 0);
    }
#else
    if (stringp(acl[0]))
	write("\""+path+"\" is default-protected. Creating specific ACL.\n");
#endif

    if (!_acl_put(acl, path))
    {
	write("edac: Failed to assign ACL rights.\n");
	return 1;
    }

    write("Ok.\n");
    return 1;
}

/* ************************** End of ACL Code ***************************** */



echo_all(str) {
    if (!str) {
       write("Echoall what?\n");
       return 1;
    }
    shout(str + "\n");
    write("You echo: " + str + "\n");
    if(this_player()->query_level() < 25)
    {
      log_file("ECHO", ctime(time()) + " " +
	       (string) this_player()->query_real_name() +
	       " : echoall " + str+"\n");
      log("ECHO: "+ (string) this_player()->query_real_name() +
	  " : echoall " + str+"\n");
    }
    return 1;
}


echo(str) {
    if (!str) {
       write ("Echo what?\n");
       return 1;
    }
    say (str + "\n");
    write ("You echo: " + str + "\n");
    return 1;
}

echo_to(str)
{
    object ob;
    string who;
    string msg;
    if (!str || sscanf(str, "%s %s", who, msg) != 2) {
	write("Echoto what ?\n");
	return 1;
    }
    ob = find_living(lower_case(who));
    if (!ob) {
	write("No player with that name.\n");
	return 1;
    }
    tell_object(ob, msg + "\n");
    write("You echo: " + msg + "\n");
    log_file("ECHO", ctime(time()) + " " +
	     (string) this_player()->query_real_name() + " : echoto " +
	     who + " " + msg + "\n");
    log("ECHO:"+(string) this_player()->query_real_name() + " : echoto " +
	     who + " " + msg + "\n");
    return 1;
}

teleport(dest) {
  object ob;
  status is_invis;
  if (!dest) {
    write("Goto where ?\n");
    return 1;
  }
  is_invis = this_player()->query_invis();
  ob = find_living(dest);
  if (ob) {
    ob = environment(ob);
    if(!is_invis)
	say(capitalize((string) this_player()->query_real_name())+ " " + (string) this_player()->query_mmsgout() + ".\n");
    move_object(this_player(), ob);
    if(!is_invis)
      say(capitalize((string) this_player()->query_real_name()) + " " + (string) this_player()->query_mmsgin() + ".\n");
    
    this_player()->display_look(ob, 0, (status) this_player()->query_brief());
    return 1;
  }
  
  dest = mk_path(dest); /* Changed by Milamber (Was valid_read(...); */
  if(dest[0]=='/')
    dest=dest[1..10000];
  if (!dest || file_size( "/" + dest + ".c") <= 0) {
    write("Invalid monster name or file name.\n");
    return 1;
  }
  this_player()->move_player("X#" + dest);
  return 1;
}





emote(str) {
    if (!str) {
	write("Emote what?\n");
	return 1;
    }
    str=capitalize((string) this_player()->query_real_name()) + " " + str + "\n";
    say(str);
    if(this_player()->query_toggles("say"))
      write("You emote: "+str);
    else
      write("Ok.\n");
    return 1;
}


tail_file(path)
{
    if (!path)
	return 0;
    if (!tail(mk_path(path)))
	return 0;
    return 1;
}

/*Added by Qqqq, used to update or load a dir*/
static check_dir(path) {
  string dir;
#if 0
  if (!valid_write(path+"/foo")) {
    write("Access denied.\n");
    return 0;
  }
#endif
  if(path != "/") 
    if(file_size(path+"/") != -2) {
    write("It isn't any directory.\n");
    return 0;
  }
  
  dir = get_dir(path+"/");
  if (!sizeof(dir)) {
    write("The directory is empty.\n");
    return 0;
  }
  return dir;
}

static changed_dir(str) {
  string dir,text;
  int i,cflag;
  object obj;

  cflag = 1;
  if (query_verb() == "updir") 
    text = "updated";
  else text = "loaded";
  if (str != "-c") { change_dir = mk_path(str);
		     cflag = 0;}
  if (!(dir = check_dir(change_dir))) return 1;
  if(!(dir = filter_files2(dir,change_dir+"/","",1))) return 1;
  if (cflag && change_file && (cflag = member_array(change_file,dir)+1)) 
    dir = dir[cflag..10000000];
  if (query_verb() == "updir") {
    for(i=0;i<sizeof(dir);i++) 
      if (obj = find_object(change_dir+"/"+dir[i])) 
	destruct(obj);
  }
  else
    for(i=0;i<sizeof(dir);i++) 
      if (!find_object( change_dir+ "/"+dir[i]))
	/*     if (error = catch(call_other(change_dir+"/"+dir[i],"AAABBBCCC"))) 
	       write("An error has occured in "+change_dir+"/"+dir[i]+"\n"+error);
	       */
	call_other(change_dir+"/"+dir[i],change_file=dir[i]);
  write("All files in the directory "+change_dir+" has been "+text+".\n");
  return 1;
}

refresh(str) {
    object ob,inv;
    if (!str) {
      ob = environment(this_player());
      str = file_name(environment(this_player()));
    }
    else
      if (!(str = mk_path(str))) {
	write("Invalid file name.\n");
	return 1;
      }
      else
	if (!(ob = find_object(str))) {
	  write("No such object.\n");
	  return 1;
	}
    if (_isclone(ob))
      return write("I can't refresh a cloned object.\n");
    if (inv = all_inventory(ob))
      inv = filter_array(inv,"refresh_under",this_object());
    destruct(ob);
    call_other(str,"????");
    if (inv && inv != ({ }))
      filter_array(inv,"move_back",this_object(),str);
     write(str + " has been refreshed.\n");
     return 1;
   }

refresh_under(obj) {
if (interactive(obj)) {
     move_object(obj,"/room/void");
     return 1;
   }
return 0;
}

move_back(obj,str) {
       move_object(obj,str);
     }


static update_object(str) {
    object ob;
    if (!str) {
	write("Update what object ?\n");
	return 1;
    }
    str = mk_path(str);
    if (!str) {
	write("Invalid file name.\n");
	return 1;
    }
    ob = find_object(str);
    if (!ob) {
	write("No such object.\n");
	return 1;
    }
    destruct(ob);
    write(str + " will be reloaded at next reference.\n");
    return 1;
}

static string locked;
void unlock()
{
  if(locked)
    "/obj/daemon/lockd"->release_lock(locked);
}

void _signal_lost_connection()
{
  unlock();
}

static edit(file)
{
    string tmp;
    if (!file) {

      ed();
      return 1;
    }
    file = mk_path(file);            
    sscanf(file, "/%s", file);

    if(tmp="/obj/daemon/lockd"->lock(file,1,0,"editing"))
    {
      write(tmp);
      return 1;
    }

    locked = file;
    ed(file,"unlock");
    return 1;
}

heal(name)
{
    object ob;
    string it;

    if (!name)
	return 0;
    it = lower_case(name);
    ob = find_living(it);
    if (!ob) {
	write("No such person is playing now.\n");
	return 1;
    }
    if(this_player()->query_level()==20 && find_player(it))
	{   
	   if(ob!=this_player()) {
		write("You can't heal players as level 20.\n");
		return 1; }
	}
    call_other(ob, "heal_self", 100000);
    tell_object(ob, "You are healed by " + capitalize(this_player()->query_name()) + ".\n");
    write("Ok.\n");

    if(ob!=this_player()) 
    {
        log("HEAL:"+ capitalize((string) this_player()->query_real_name()) + " healed " +
	     capitalize(name) + " who was in "+file_name(environment(ob))+".\n");
    }

    return 1;
}

stat(livname)
{
  object ob;
  string it;
  if (!livname)
    return 0;
  it = lower_case(livname);
  ob = present(livname, environment(this_player()));
  if (!ob || !living(ob))
    ob = find_living(it);
  if (!ob) {
    write("No such person is playing now.\n");
    return 1;
  }
  ob->show_stats();
  return 1;
}

static shut_down_game(str)
{
    if (!str) {
	write("You must give a shutdown reason as argument.\n");
	return 1;
    }
    shout("Game is shut down by " + capitalize((string) this_player()->query_real_name()) + ".\n");
#ifdef LOG_SHUTDOWN
    log_file("GAME_LOG", ctime(time()) + " Game shutdown by " + (string) this_player()->query_real_name() +
	     "(" + str + ")\n");
#endif
    log("Game shutdown by " + (string) this_player()->query_real_name()+
	"(" + str + ")\n");
    shutdown();
    return 1;
}

spell_zap(str)
{
  object ob;


  if (!str)
    ob = (object) this_player()->query_attack();
  else
    ob = present(lower_case(str), environment(this_player()));
  if (!ob || !(interactive(ob) || ob->query_npc()))
  {
    write("At whom ?\n");
    return 1;
  }

  if (!this_player()->attack_allowed(ob))
    return 1;

  this_player()->zap_object(ob);
  return 1;
}




force_player(str)
{
    string who, what;
    object ob;
    
    if (!str)
	return 0;
    
    if (sscanf(str, "%s to %s", who, what) == 2 ||
	sscanf(str, "%s %s", who, what) == 2) {
	ob = find_living(who);
	if (!ob) {
	    write("No such player.\n");
	    return 1;
	}
	if (interactive(ob) &&
	    ob->query_level_sec() >= this_player()->query_level_sec())
	{
	  write("You fail.\n");
	  return 1;
	}
	
	tell_object(ob, capitalize((string) this_player()->query_real_name()) + " forces you to: " + what + "\n");
	command(what, ob);
	write("Ok.\n");
 
        log("FORCE: "+
	     capitalize((string) this_player()->query_real_name()) + " forced " +
	     capitalize(who) + " to '" + what + "' in "+
		     (environment(ob)?file_name(environment(ob)):"no env")+".\n");

	return 1;
    }
    return 0;
}



clone(string str) 
{
  object ob;
  string mess;
  string ob_name;
  
  if (!str) 
  {
    write("Clone what object ?\n");
    return 1;
  }
  if(sscanf(str,"a %s",str) || sscanf(str,"an %s",str))
  {
    ob=(object)"/std/lib"->make(str);
  }else{
    str = mk_path(str);
    if (extract(str,-2) == ".c")  /* Added by Guildmaster 941110 */ 
      str = extract(str,0,-3);  /* Handles .c extension */
  
    ob = clone_object(str);
  }
  if(!ob)
  {
    write("Clone failed.\n");
    return 1;
  }
  
  ob_name = ob->short();
  if(!ob_name)
    ob_name = ob->query_name();
  if(!ob_name)
    ob_name = "something";
  mess = (string) this_player()->getenv("MCLONE");
  if(!mess)
    mess = capitalize((string) this_player()->query_name()) + " fetches something from another dimension.";
  else
  {
    mess = replace(mess, "$N", capitalize((string) this_player()->query_name()));
    mess = replace(mess, "$O", ob_name);
  }
  say(capitalize(mess) + "\n");
  if (call_other(ob, "get")) 
    transfer(ob, this_player());
  if(ob && !environment(ob))
    move_object(ob, environment(this_player()));
  write("Ok.\n");
  return 1;
}



destruct_local_object(string str)
{
  object ob;
  string mess;
  string ob_name;



  if (!str) 
  {
    notify_fail("Destruct what ?\n");
    return 0;
  }
  str = lower_case(str);
  ob = present(str, this_player());
  if (!ob)
    ob = present(str, environment(this_player()));
  if (!ob)
  {
    write("No " + str + " here.\n");
    return 1;
  }
  ob_name = ob->short();
  if(!ob_name)
    ob_name = str;
  mess = (string) this_player()->getenv("MDEST");
  if(!mess)
    mess = ob_name + " is disintegrated by " + capitalize((string) this_player()->query_name()) + ".";
  else
  {
    mess = replace(mess, "$N", capitalize((string) this_player()->query_name()));
    mess = replace(mess, "$O", ob_name);
  }
  destruct(ob);
  say(capitalize(mess) + "\n");
  write("Ok.\n");
  return 1;
}

load(str)
{
    object env;
    string mess;
    
    if (!str)
    {
      write("Load what ?\n");
      return 1;
    }
    str = mk_path(str);
    if (!str)
    {
      write("Invalid file name.\n");
      return 1;
    }
    env = environment(this_player());
    /* We just call a non existing function in the object, and that way
     * force it to be loaded.
     */

    /* Putting a catch here is to deprive wizards of their rightful error
     * messages /Profezzorn
     */
    call_other(str, "???");

    write("Ok.\n");
    return 1;
}

static snoop_on(str)
{
    object ob;
    int ob_level;

    if (!str) 
      {
	snoop();
	return 1;
      }

    ob = find_player(lower_case(str));
    if (!ob) 
      {
	write("No such player.\n");
	return 1;
      }
    if(snoop(ob))
      {
	if(this_player()->query_level() < 23)
	  log("SNOOP: "+
	      this_player()->query_real_name()+" snooping "+
	      ob->query_real_name()+" who was in: "+
	      file_name(environment(ob))+"\n");
      }
    else
      write("You fail.\n");
    
    return 1;
}





static home() {
  string where;
  
  where = (string) this_player()->getenv("WROOM");
  if(!where)
    where = "players/" + (string) this_player()->query_real_name() + "/workroom";
  this_player()->move_player("X#" + where);
  return 1;
}


wiz_score_list(arg)
{
    if (arg)
	wizlist(arg);
    else
	wizlist();
    return 1;
}

static remove_file(string str) {
  mixed *files;
  int i;
  string tmp;
  
  if(!str) {
    notify_fail("Usage: rm <file>\n");
    return 0;
  }
  
  str = mk_path(str);

  if(file_size(str) == -2) {
    write("rm: " + str + " is a directory\n");
    return 1;
  }

  if(file_size(str) > -1)
  {
    if(tmp="/obj/daemon/lockd"->has_access(str))
    {
      write(str);
      return 1;
    }
    if(rm(str))
      write("rm: File removed: " + str + "\n");
    else
      write("rm: rm failed: " + str + "\n");
    return 1;
  }

  files = get_dir(str);

  if(!files || files == ({})) {
    write("rm: File not found: " + str + "\n");
    return 1;
  }

  for(i=strlen(str); i && str[i] != '/'; i--)
    str = str;
  str = extract(str,0,i);

  i = 0;
  while(i < sizeof(files))
  {
    if(tmp="/obj/daemon/lockd"->has_access(str+files[i]))
    {
      write(str);
      continue;
    }

    if(rm(str + files[i]))
      write("rm: File removed: " + str + files[i] + "\n");
    else
      write("rm: rm failed: " + str + files[i] + "\n");
    i++;
  }
	
  return 1;
}

local_commands()
{
  string *actions;
  int i, j;
  

  actions = _query_action(this_player());
  write("Current local commands:\n");
  j = 0;
  for (i = 0; i < sizeof(actions); i++)
  {
    if (j + strlen(actions[i]) > 70)
    {
      j = 0;
      write("\n");
    }

    j+= strlen(actions[i]);
    write(actions[i]);
    
    if (i+1 < sizeof(actions))
    {
      j++;
      write(" ");
    }
  }
  
  write("\n");
  
  return 1;
}



status list_peoples(string str)
{ 
  object *list;
  string host, pathname, username, restpath, age;
  int show_where, j, a, len, level, minlevel;
  mixed *flags, *showgen, i;

  if(!str) str="";
  flags=get_flags(str,"orsiwmlaOMFet");

#define PEOPLE_OLD     (flags[1]&1)
#define PEOPLE_REVERSE (flags[2]&1)
#define PEOPLE_SHORT   (flags[3]&1)
#define PEOPLE_IDENT   (flags[4]&1)
#define PEOPLE_WIZARDS (flags[5]&1)
#define PEOPLE_MORTALS (flags[6]&1)
#define PEOPLE_LONG    flags[7]
#define PEOPLE_ALPHA   flags[8]
#define PEOPLE_NEUTER  (flags[9]&1)
#define PEOPLE_MALE    (flags[10]&1)
#define PEOPLE_FEMALE  (flags[11]&1)
#define PEOPLE_LOGON   (flags[12]&1)
#define PEOPLE_TIME    (flags[13]&1)

  if(flags[0]=="")
  {
    list=users();
  }
  else if(minlevel=atoi(flags[0]))
  {
    list=users();
    for(i = a = 0; i < sizeof(list); i++)
      if(list[i]->query_level()>=minlevel)
	list[a++]=list[i];

    list=list[0..a-1];
  }
  else if(flags[0][0]=='/')
  {
    list=users();
    str=flags[0][1..1000000];

    for(i = a = 0; i < sizeof(list); i++)
      if(environment(list[i]) &&
	 file_name(environment(list[i]))[0..strlen(str)-1]==str)
	list[a++]=list[i];

    list=list[0..a-1];
  }
  else
  {
    i=find_player(flags[0]);
    if(!i)
    {
      write("No such player on.\n");
      return 1;
    }

    list=({i});
  }

  list = m_values(mkmapping(map_array(list, "people_scale_fun"+PEOPLE_ALPHA, this_object()), list));
  len=sizeof(list);

  PEOPLE_LONG*=5;

  if(PEOPLE_MALE || PEOPLE_NEUTER || PEOPLE_FEMALE)
  {
    showgen=({ 0, PEOPLE_NEUTER, PEOPLE_MALE, PEOPLE_FEMALE });
  }else{
    showgen=({ ! PEOPLE_LOGON, 1, 1, 1});
  }

  for(i = a = 0; i < sizeof(list); i++)
    if(query_idle(list[i]) >= MAX_IDLE/2)
      a++;

  if(len==sizeof(users()))
    write("There are now ");
  else
    write("Listing ");

  write(len + " players (" + (len - a) +
	" active). " + query_load_average() + "\n");

  for(i=0; i<sizeof(list); i++)
  {
    object o;
    string name, idle;

    o=list[ PEOPLE_REVERSE ? ~i : i ];
    level=(int) o->query_level();

    if(PEOPLE_WIZARDS && level < 20) continue;
    if(PEOPLE_MORTALS && level > 19) continue;

    j = (int) o->query_gender()+1;
    if(!showgen[j]) continue;

    name = capitalize((string) o->query_real_name() || "logon");

    if(!o->short())  name = "(" + name + ")";

    idle = "-NMF"[j..j]+
      (query_idle(o) >= MAX_IDLE/2 ? "I" : (_query_ed(o) ? "E" : (o->query_away() ? "A" : " ")))+
	(o->query_testchar() ? "T" : (o->is_hard_invis() ? "H" : " "));

    if(PEOPLE_TIME)
    {
      switch(a = (int) o->query_age() * 2)
      {
      case 0..59:
	age=a + " s";
	break;
      case 60..60 * 60 - 1:
	age=(a / 60) + " m";
	break;
      case 60 * 60..60 * 60 * 24 - 1:
	age=(a / 3600) + " h";
	break;
      case 60 * 60 * 24..60 * 60 * 24 * 30 - 1:
	age=(a / 86400) + " D";
	break;
      case 60 * 60 * 24 * 30..60 * 60 * 24 * 365 - 1:
	age=(a / (60*60*24*30)) + " M";
	break;
      default:
	age=(a / (60 * 60 * 24 * 365)) + " Y";
      }
      age=sprintf("%4s",age);
    }else{
      switch(a = (int) o->query_age() * 2)
      {
      case 0..59:
	age=a + " s";
	break;
      case 60..60 * 60 - 1:
	age=(a / 60) + " m";
	break;
      case 60 * 60..60 * 60 * 24 - 1:
	age=(a / 3600) + " h";
	break;
      case 60 * 60 * 24..60 * 60 * 24 * 365 - 1:
	age=(a / 86400) + " D";
	break;
      default:
	age=(a / (60 * 60 * 24 * 365)) + " Y";
      }
      age=sprintf("%5s",age);
    }

    if(!PEOPLE_SHORT)
    {
      if(!environment(o))
	pathname="";
      else
      {
	pathname = file_name(environment(o));
	if(!PEOPLE_OLD)
	{
	  if(sscanf(pathname, "players/%s/%s", username, restpath))
	  {
	    if(username == (string) this_player()->query_real_name())
	      pathname = restpath;
	    else
	      pathname ="~" + username + "/" + restpath;
	  }
	  else if (sscanf(pathname, "guilds/%s/%s", username, restpath))
	  {
	    pathname ="~" + capitalize(username) + "/" + restpath;
	  }
	  else
	  {
	    pathname = "/" + pathname;
	  }
	}
      }
    }

    if(PEOPLE_OLD)
    {
      host=query_ip_number(o);
    }else{
      if(PEOPLE_IDENT && (host=_query_ident(o)))
	host+="@";
      else
	host="";
      host+=query_ip_name(o);
    }

    if(!PEOPLE_SHORT)
    {
      j=25+PEOPLE_LONG;
      while(j < strlen(host) && sscanf(age," %s",age)) j++;

      write(sprintf("%-13s%2d %s %-*s %s %s\n",
		    name,
		    level,
		    idle,
		    25+PEOPLE_LONG,host,
		    age,
		    pathname));
    }
    else
    {
      j=40+PEOPLE_LONG;
      while(j < strlen(host) && sscanf(age," %s",age)) j++;

      write(sprintf("%-13s %2d %s %-*s %s\n",
		    name,
		    level,
		    idle,
		    40+PEOPLE_LONG,host,
		    age));
    }
  }
  return 1;
}

people_scale_fun0(o)
{
  string a;
  a=o->query_name();
  return -o->query_level()*0x1000000+a[0]*0x10000+a[1]*0x100+a[2];
}

people_scale_fun1(o)
{
  string a;
  a=o->query_name();
  return a[0]*0x1000000+a[1]*0x10000+a[2]*0x100+a[3];
}

people_scale_fun2(o)
{
  return -o->query_age();
}

people_scale_fun3(o)
{
  return -query_idle(o);
}

#if 0
people_sort_fun(ob1,ob2) {
  int lev1,lev2;
  string name1,name2;
  
  
  lev1 =  ob1->query_level();
  lev2 =  ob2->query_level();
  name1 = ob1->query_real_name();
  name2 = ob2->query_real_name();
  if(lev1 == lev2)
    return name1 > name2;
  return lev1 < lev2;
  
}
#endif

pwd() {
  write("/" + current_path + "\n");
  return 1;
}

makedir(str) {
  if (!str)
    return 0;
  if (mkdir(mk_path(str)))
    write("Ok.\n");
  else
    write("Fail.\n");
  return 1;
}

static removedir(string str)
{
  string path,*files;
  string *acl_files;
  int e;

  if (!str) return notify_fail("Usage: rmdir <directory>\n"),0;

  path=mk_path(str);

  if(file_size(path)!=-2)
  {
    write("Rmdir failed: No such directory or permission denied.\n");
    return 1;
  }

  if(path[-1]!='/') path+="/";

  files=get_dir(path);
  acl_files=regexp(files,"^\\.acl");

  if(sizeof(files)>sizeof(acl_files))
  {
    write("Rmdir failed: Directory not empty.\n");
    return 1;
  }

  for(e=0;e<sizeof(acl_files);e++)
  {
    if(!rm(path+acl_files[e]))
    {
      write("Rmdir: failed to remove acl file ("+path+acl_files[e]+").\n");
      return 1;
    }
  }

  if (rmdir(path))
    write("Ok.\n");
  else
    write("Rmdir failed.\n");
  return 1;
}

query_path() {
  if(!this_player(1)||!this_player())
    return "/";
  /* Paranoia checks added by Brom 951218.
   */

  if(!objectp(this_player(1)) ||
     !objectp(this_object()) ||
     !objectp(environment(this_object())) ||
     this_player(1) -> query_level() < environment(this_object())->query_level())
    return "/";
  return current_path;
}

toggle_file_exp()
{
  file_exp = !file_exp;
  write("Filename expansion ");
  if(file_exp)
    write("on.\n");
  else
    write("off.\n");
  return 1;
}

cd(path) {
  string tmp;
  
  if(!path) {
    current_path = (string) this_player()->getenv("HOME");
    if(!current_path)
      current_path = "players/" + (string) this_player()->query_real_name();
  }
  else {
    path = mk_path(path);		/* Evaluate the path    */
    if(path != "/") {
      if(file_size(path+"/") != -2) {
	write("Can't find directory or access denied.\n");
	return 1;
      }
      sscanf(path,"/%s",current_path);
    }
    else
      current_path = "";
  }
  write("/" + current_path + "\n");
  this_player()->update_prompt();
  return 1;
}


static static cat_file(file) {
  int from,to;
  
  if(!file) {
    notify_fail("Usage: cat <file> [from length]\n");
    return 0;
  }
  sscanf(file,"%s %d %d",file,from,to);
  if(from && !to)
    to = 20;
  file = mk_path(file);            
  file = find_file(file);
  if(!file) {
    write("Couldn't find.\n");
    return 1;
  }
  if(!cat("/"+file,from,to)) {
    write("Couldn't find " + file + "\n");
    return 1;
  }
  write("Ok.\n");
  return 1;
}

get_prev_dir(path) {
  
  path = explode(path,"/");
  if(!path || path==({}) || sizeof(path)==1)
    return "/";
  return "/" + implode(path[0..sizeof(path)-2],"/");
}

list_files(path)
{
  string tmp, files;
  int i, size, long_flag, all_flag, s_flag;
    

  while (path && path[0] == '-')
  {
    if(sscanf(path, "-%s %s", tmp, path) != 2)
    {
      tmp = path[1..strlen(path)];
      path = ".";
    }
    i = strlen(tmp);
    while(i--)
      switch (tmp[i])
      {
       case 'l':
	long_flag = 1;
	break;
       case 'a':
	all_flag = 1;
	break;
       case 's':
	s_flag = 1;
	break;
	
       default:
	write("ls: Unknown command line flag: -" + tmp[i..i] + "\n");
	return 1;
      }
  }
  
  path = mk_path(path);

  files = get_dir(path);
  
  if(!files || files == ({}))
  {
    if(sscanf(path,"%s*",tmp) || sscanf(path,"%s?",tmp))
      write("No match.\n");
    else
      write("Can't find directory or access denied.\n");
    return 1;
  } 
  while(path[-1] == '/')
    path = path[0..strlen(path)-2];
  return print_files(files, path + "/", all_flag, long_flag, s_flag), 1;
}


mk_path(path) {
  int i;
  string rest;
  
  if(!path)
    return "/" + current_path;
  
  if(path == "~")
    return "/players/" + (string) this_player()->query_real_name();
  if(sscanf(path,"~/%s",path))
    path = "/players/" + (string) this_player()->query_real_name() + "/" + path;
  else if(sscanf(path,"~%s",path))
    path = "/players/" + path;
  
  i = strlen(path)-1;
  rest = "";
  if(path[i] == '/')
    rest = "/";
  if( i && path[i] == '.' && path[i-1] == '/')
    rest = "/.";
  if(path[0] != '/')
    path = current_path + "/" + path;

  path = explode(path,"/");
  if(!path || path == ({}))
    return "/";
  path = filter_array(path,"path_fun",this_object());
  
  for(i = member_array("..",path); i != -1; i = member_array("..",path)) {
    if( i > 1)
      if(i == sizeof(path)-1)
	path = path[0..i-2];
      else
	path = path[0..i-2] + path[i+1..sizeof(path)-1];
    else
      path = path[i+1..sizeof(path)-1];
  }
  
  if(!path || path == ({}))
    return "/";
  path = implode(path,"/");
  return "/" + path + rest;;
  
}

path_fun(str) { return str != "" && str != "."; }

#define CHUNK 17
#define SEARCH_LINES 500

static string file;
static string f_name;
static int tot_lines;
static int lines;
static int current_line;
static int no_show;

set_lines(arg) { lines = arg; }

more(str) 
{
  string tmp;
  
  current_line = 1;
  if(!lines)
    lines = CHUNK;
  if(!str) {
    if(sscanf(str = "/" + file_name(environment(this_player())), "%s#%*s", tmp)) 
      str = tmp +".c";
    else 
      str += ".c";
#if 0
    if(!valid_read(str)) {
      write("Usage: more <filename>\n");
      return 1;
    }
#endif
  }
  file = mk_path(str);
  file = find_file(file);
  if(file)
    file = "/" + file;
  if(file){
    tot_lines = count_lines(file);
    tmp = explode(file,"/");
    f_name = tmp[sizeof(tmp)-1];
    show_text();
    more_prompt();
  }
  else 
    if(!file_exp)
      write("Couldn't find " + str + ".\n");
  return 1;
}


syntax(s) {
  string filecon,text,tmp;
  int i;

  filecon = read_file(s);
  if (sscanf(filecon,"%*sSYNOPSIS%s",tmp)) {
      if (sscanf(tmp,"%sDESCRIPTION%*s",text)) {
	write(s+":\n"+implode(explode(text,"\n"),"\n"));
	return 1;
      }
      filecon = tmp;
    }
  filecon = explode(filecon,"\n");
  if (pointerp(filecon)) 
    for(i=0;i<sizeof(filecon);i++)
      if (replace(replace(filecon[i]," ",""),"\t","") == "")
	continue;
      else {
	write(s+":\n "+filecon[i]+"\n");
	return 1;
      }
  write("Empty file.\n");
  return 1;
}

#define MAN_CHECK(X,s) if(0<file_size((X)+s)) if (query_verb() == "Syntax") return syntax((X)+s); else return more((X)+s)
man(string s)
{
  string mpath,*p;
  int e;

  if(!s)
  {
    notify_fail("Usage: "+query_verb()+" <topic>.\n");
    return 0;
  }
  if(stringp(mpath=(string)this_player()->getenv("MANPATH")))
  {
    if(p=explode(mpath,":"))
    {
      for(e=0;e<sizeof(p);e++)
      {
	mpath=p[e];
	if(mpath[-1]!='/') mpath+="/";
	MAN_CHECK(mpath,s);
      }
    }
  }
  MAN_CHECK("/doc/efun/",s);
  MAN_CHECK("/doc/sfun/",s);
  MAN_CHECK("/doc/lfun/",s);
  if (query_verb() != "Syntax") {
    if(s[-1]=='c' && s[-2]=='.')
      s=s[0..strlen(s)-3];

    MAN_CHECK("/doc/",s);
    MAN_CHECK("/doc/build/",s);
    MAN_CHECK("/doc/LPC/",s);
    MAN_CHECK("/doc/w/",s);
    MAN_CHECK("/doc/helpdir/",s);
    MAN_CHECK("/doc/basic/",s);
    MAN_CHECK("/doc/std/",s);
    MAN_CHECK("/doc/std/",s+".doc");
    MAN_CHECK("/doc/hooks/",s);
  }
  if (query_verb() == "Syntax")
    write("Could not find any function matching "+s+".\n");
  else
    write("Could not find any file matching "+s+".\n");
  return 1;
}

more_input(str) {
  int jump;
  int tmp_line;
  int rem_line;
  int ret;
  
  if(str[0] == '/') {
    sscanf(str,"/%s",str);
    search_string(str);
  }
  else
    if(sscanf(str,"-%d",tmp_line) == 1) {
      current_line -= tmp_line;
      if (current_line < 1)
	current_line = 1;
    }
    else
      if(sscanf(str,"+%d",tmp_line) == 1) {
	current_line += tmp_line;
	jump = 1;
      }
      else
	if(sscanf(str,"%d",tmp_line) == 1) {
	  rem_line = current_line;
	  current_line = tmp_line;
	}
	else
	  if(sscanf(str,"l%d",lines) == 1) {
	    if(lines < 1)
	      lines = CHUNK;
	    if(lines > 40)
	      lines = 40;
	    write("\nPage length set to " + lines + "\n\n");
	    no_show = 1;
	  }
	  else
	    switch(str) {
	      
	     case "u":
	     case "p":
	      current_line -= lines;
	      if(current_line < 1)
		current_line = 1;
	      break;
	      
	     case "b":
	      current_line -= 4;
	      if(current_line < 1)
		current_line = 1;
	      break;
	      
	     case "":
	     case "n":
	      current_line += lines;
	      break;
	      
	     case "d":
	      current_line += 4;
	      break;
	      
	     case "q":
	      end_more();
	      return;
	      
	     case "h":
	      help_more();
	      break;
	      
	     case "?":
	      help_more();
	      
	     default:
	      no_show = 1;
	    }
  
  if(!no_show) {
    ret = show_text();
    
    if(!ret) 
      if(!jump) {
	write("EOF\n");
	return;
      }
      else {
	write("To long jump.\n");
	current_line = rem_line;
      }
  }
  more_prompt();
}

show_text() {
  
  return cat(file,current_line,lines);
}

more_prompt() {
  int perc;
  
  no_show = 0;
  perc = (current_line + lines - 1)*100/(tot_lines ? tot_lines : 1);
  if(perc > 100)
    perc = 100;
  write(f_name + " : " + current_line + "-" + (current_line+lines-1));
  write(" (" + tot_lines + ") " + (tot_lines ? perc : "--") + "%");
  write(" -- [<cr>,d,u,b,#,/,q,?] ");
  input_to("more_input");
}

end_more() {
  write("Ok.\n");
}

help_more() {
  write("\n--------- More Help ---------\n\n");
  write("\tu,p\tOne page up.\n");
  write("\t<cr>,n\tOne page down.\n");
  write("\tb,d\tUp/down 4 lines.\n");
  write("\t#\tGoto line #.\n");
  write("\t+/-#\tJump # lines up/down.\n");
  write("\t/str\tGoto line with first occurance of str.\n");
  write("\tl#\tSet page length.\n\n");
}

#define COUNT_LINES 1000

count_lines(path) {
  int i,j;
  string tmp, slask;
  
  if(file_size(path) < 0)
    return 0;
  while(tmp = read_file(path,i,COUNT_LINES)) {
    j += sizeof(explode(tmp,"\n"));
    i += COUNT_LINES;
  }
  return j;
}

search_string(str) {
  int tmp_line;
  int s_lines;
  string tmp_str;
  string tmp;
  
  if(!str) return;
  tmp_line = current_line + lines -2;
  s_lines = SEARCH_LINES;
  
  tmp_str = read_file(file, tmp_line, s_lines);
  
  while( tmp_str && s_lines ) {
    
    tmp_str = read_file(file, tmp_line, s_lines);
    
    
    while(tmp_str && !sizeof(regexp( ({ tmp_str }), str))) {
      tmp_line += s_lines;
      tmp_str   = read_file(file, tmp_line, s_lines);
    }
    
    if(tmp_str)			/* Got a matching string */
      s_lines = ( s_lines > 10 ) ? s_lines/10 : s_lines-1;
    else
      break;			/* No match and eof  *sulk*  */
  }
  
  if(tmp_str) {
    current_line = tmp_line - 2;
    return 1;
  }
  
  write("No match.\n");
  no_show = 1;
}

find_file(str) {
  string dir;
  string files;
  int stat;
  int i;
  
  stat = file_size(str);
  if(stat > -1)
    return str;
  if(!file_exp)
    return 0;
  if(stat == -2) {
    write(str + " is a directory containing:\n");
    if(str != "/")
      str += "/";
    print_files(get_dir(str),str, 0, 0);
    return 0;
  }
  dir = get_prev_dir(str) + "/";
  files = get_dir(dir);
  if(!files) {
    write("Couldn't read directory: " + dir + "\n");
    return 0;
  }
  files = filter_files(files,dir,str,1);
  if(files)
    if(sizeof(files) > 1) {
      write("Several files matches: " + str + "\n");
      print_files(files,dir, 0, 0);
      return 0;
    }
    else {
      write("Treating: " + dir + files[0] + "\n");
      return dir + files[0];
    }
  write("No file matches " + str + "\n");
}


filter_files(files,dir,str,no_dir) {
  int i;
  string tmp;
  string tmp_files;
  int c;
  for(i = 0; i < sizeof(files); i++) 
    if(sscanf(dir + files[i],str+"%s",tmp) &&
       (!no_dir || file_size(dir+files[i]) !=-2) &&
       (c = files[i][strlen(files[i])-1]) && 
       c != '~' && c != '#')
      if(!tmp_files)
	tmp_files = ({ files[i] });
      else
	tmp_files += ({ files[i] });
  return tmp_files;
}

filter_files2(files,dir,str,no_dir) {
  int i;
  string tmp;
  string tmp_files;
  int c;
  for(i = 0; i < sizeof(files); i++) 
    if(sscanf(dir + files[i],str+"%s.c",tmp) &&
       (!no_dir || file_size(dir+files[i]) !=-2) &&
       (c = files[i][strlen(files[i])-1]) && 
       c != '~' && c != '#')
      if(!tmp_files)
	tmp_files = ({ files[i] });
      else
	tmp_files += ({ files[i] });
  return tmp_files;
}

print_files(files, path, all_flag, long_flag, s_flag)
{
  int i;
  int len;
  string tmp;
  string *file;
  int fsize;
    
  if(!all_flag)
    files = regexp(files, "^[^\.]");
  i = sizeof(files);
  while(i--)
    if((fsize = file_size(path + files[i])) == -2)
      if(s_flag)
	files[i] = sprintf("     %s/",files[i]);
      else
	files[i] += "/";
    else if(s_flag)
      files[i] = sprintf("%4d %s", fsize/1024 || 1, files[i]);
  
  if(!long_flag)
    return write(sprintf("%#-80s\n",implode(files,"\n"))),0;

  file = allocate(i = sizeof(files));
  while(i--)
  {
    
  }
  write("\n");
}

/* cp -- Oros Jan 1991 */

static cp(string str) {
  string from, to;
  string verb, tmp;
  string *files;
  int    i, force, dir;
  
  verb = query_verb();
  
  if(!str || (!(force = (sscanf(str, "-f %s %s", from, to) == 2)) &&
	      sscanf(str,"%s %s",from,to) != 2)) {
    notify_fail("Usage: " + verb + " [ -f ] <from> <to>\n");
    return 0;
  }
  
  
  /* Get the full path name */
  
  from = mk_path(from);
  to   = mk_path(to);

  
  /* If we have wildcards in the string or if str is a dir   */
  /* then the target must be a directory                     */
  
  if ((sscanf(from,"%s*",tmp) || sscanf(from,"%s?",tmp) ||
       (dir = (file_size(from + "/") == -2))) &&
      file_size(to+"/") != -2) {
    write(verb + ": Target must be a directory when using wildcards.\n"); 
    return 1;
  }
  
  if(dir)
    from += "/";
  
  files = get_dir(from);
  
  if (!files || files == ({})) {
    write(verb + ": Couldn't find any files.\n");
    return 1;
  }
  
  from = get_prev_dir(from + "/");
  
  for(i = 0; i < sizeof(files); i++)
    do_copy(from + "/", files[i], to, force, verb);
  
  return 1;
}

static do_copy(from, file, to, force, verb) {
  int i;
  string tmp;
  
  if(i = file_size(from + file + "/") == -2) {
    write(verb + ": " + from + file + " is a directory.\n");
    return 1;
  }
  
  if(file_size(to + "/") == -2) 
    to += "/"+file;
  
  
  if (to == (from+file))
    return write(verb + ": "+to+" not effected, because source and target file  are the same.\n");

  if(tmp="/obj/daemon/lockd"->has_access(to))
    return write(tmp);

  from += file;

  if(verb[0] == 'm')
    if(tmp="/obj/daemon/lockd"->has_access(from))
      return write(tmp);
  
  /* Check if the 'to' file exists. Remove it if we have the force flag */
  
  if(file_size(to)>0) {
    if(!force) {
      write(verb + ": file already exists: " + to + "\n");
      return 1;
    }
    else
      rm(to);
  }
  
  /* Copy the file */
  
  for(i = 1; tmp = read_file(from,i,500); i += 500) 
    write_file(to,tmp);
  
  /* Let's see if the copy was succesful */
  
  if(file_size(to) != file_size(from)) {
    rm(to);
    write(verb + " failed.\n");
    return 1;
  }
  
  if(verb[0] == 'c')
    verb = "Copied";
  else {
    verb = "Moved";
    rm(from);
  }
  
  write(verb + ": " + from + " to " + to + "\n");
  return 1;
  
}


int set_env(string str)
{
  string var, value, verb;
  
  if(!str) {
    if(!this_player()->printenv())
      write("No variables set.\n");
    return 1;
  }
  
  verb = query_verb();
  
  if(sscanf(str, "%s %s", var, value) != 2) {
    notify_fail("Usage: " + verb + " [<variable> <value>]\n");
    return 0;
  }
  
  this_player()->setenv(var, value);
  write("Ok.\n");
  return 1;
}

int unset_env(string var) {
  
  if(!var) {
    notify_fail("Usage: " + query_verb() + " <variable>\n");
    return 0;
  }
  if(!this_player()->unsetenv(var))
    write("Couldn't find variable " + var + ".\n");
  else
    write("Ok.\n");
  return 1;
}

set_title(str)
{
    return (int) this_player()->set_title(str);
}

/*
 * Temporary move the player to another destination and execute
 * a command.
 */

static in_room(str)
{
    object room;
    object old_room;
    string cmd;
    
    if (!str)
	return 0;
    
    if (sscanf(str, "%s %s", room, cmd) != 2) {
	write("Usage: in ROOM CMD\n");
	return 1;
    }
    room = mk_path(room);
    if (!room) {
	write("Invalid file name.\n");
	return 1;
    }
    old_room = environment(this_player());
    move_object(this_player(), room);
    command(cmd, this_player());
    if (old_room)
	move_object(this_player(), old_room);
    else
	write("Could not go back again.\n");
    return 1;
}



static trans(str)
{
    object ob;
    string out,old_dest;

    if (!environment(this_player()))
        return 0;

    if (!str)
	return 0;
    ob = find_living(str);
    if (!ob) {
	write("No such living thing.\n");
	return 1;
    }
    old_dest=file_name(environment(ob));
    if(this_player()->query_level() < 25 && ob->query_level()<21)
      log("TRANS: "+
	  (string) this_player()->query_real_name() +
	  " : " + str + " from " + old_dest + " to " +
	  file_name(environment(this_player()))+"\n");

/*    it = str; */
#if 1
    tell_object(ob, "You are magically transfered somewhere.\n");
    out = call_other(ob, "query_mmsgin", 0);
    if (!out)
	out = capitalize(ob->query_name()) +
	    " arrives in a puff of smoke.\n";
    else
	out = capitalize(ob->query_name()) + " " + out + ".\n";
    say(out);
    write(out);
    environment(ob) ?
      tell_room(environment(ob),ob->query_name()+" disappears in a puff of smoke.\n",({ob})):0; /* Cel */

    move_object(ob, environment(this_player()));
#else
    ob->move_player("X", environment(this_player()));
#endif
    return 1;
}

cmd_owner(str) {

  string *all, head, ws, items, pn, ofn, fn, ts, thing, junk;
  mapping lvls;
  object *us, u, it;
  int i, j, s, is, nr, minlvl, maxlvl, tmp;
  status dd, ds, sf, sv, sw, sn;

  if (!str) {
    notify_fail("Try: owner -h\n");
    return 0;
  }
  ws = "-------------------------------------------------------------------------\n";
  head = "";
  lvls = ([]);
  dd = ds = sf = sn = sv = sw = nr = 0;
  minlvl = 1;
  maxlvl = 50;
  /* Parse the flags etc; */
  if (sscanf(str, "%*s-h%*s")==2 || sscanf(str, "%*s-?%*s")==2) {
    this_player() -> simplemore(cat("doc/w/owner"));
    this_player() -> flush();
    return 1;
  }

  all = explode(str, " ");
  s = sizeof(all);
  thing = all[s-1];

  for (i=0; i<s-1; i++) {
    ts = all[i];
    if (sscanf(ts, "-%*sd%*s")==2) dd = 1;
    if (sscanf(ts, "-%*sf%*s")==2) sf = 1;
    if (sscanf(ts, "-%*sn%*s")==2) sn = 1;
    if (sscanf(ts, "-%*ss%*s")==2) ds = 1;
    if (sscanf(ts, "-%*sv%*s")==2) sv = 1;
    if (sscanf(ts, "-%*sw%*s")==2) sw = 1;
    if (sscanf(ts, "%d", tmp)==1) lvls[tmp] = 1;
  }    
  s = m_sizeof(lvls);
  if (s) minlvl = m_indices(lvls)[0];
  if (s>1) maxlvl = m_indices(lvls)[s-1];

  us = users();
  if (ds)
    us = m_values(mkmapping(us->query_level(), us));

  s = sizeof(us);
  for (i=0; i<s; i++) {
    u = us[i];
    tmp = u->query_level();
    if (maxlvl < tmp || minlvl > tmp) continue;
    
    if (dd) 
      items = deep_inventory(u);
    else
      items = all_inventory(u);
    is = sizeof(items);
    pn = u->query_real_name();
    if (stringp(pn))
      pn = capitalize(pn);
    for (j=0; j<is; j++) {
      it = items[j];
      if (!(it -> id(thing))) continue; /* Next thing */
      nr++;
      ws += sprintf("%-12s", pn);
      if (sn) 
	ws += sprintf("%-20s", (it->query_name()+"")[0..18]);
      else
	ws += sprintf("%-20s", (it->short()+"")[0..18]);
      if (sv) ws += sprintf("%-6s", (it->query_value())+"");
      if (sw) ws += sprintf("%-3s", (it->query_weight())+"");
      if (sf) {
	fn = file_name(it);
	fn = replace(fn, "players/", "~");
	ws += sprintf("%-30s\n", fn);
      }
      else
	ws += sprintf("%-12s\n", creator(it)+"");
    } /* End of item loop. */
  } /* End of user loop */

  if (!nr) {
    write("No " + thing + " found.\n");
    return 1;
  }

  head += sprintf("\n%-12s", "Who");
  if (sn) 
    head += sprintf("%-20s", "Item name");
  else
    head += sprintf("%-20s", "Short");
  if (sv) head += "gc   ";
  if (sw) head += " w  ";
  if (sf) 
    head += "File name\n";
  else
    head += "Creator\n";
  this_player() -> simplemore(head);
  this_player() -> simplemore(ws);
  this_player() -> flush();
  return 1;
} /* cmd_owner */

int change_light(string s)
{
  int amount;

  if (!s) {
    set_light(1-set_light(0));
    return 1;
  }
  if (sscanf(s, "%d", amount)!=1) {
    notify_fail("Non-numeric argument.\n");
    return 0;
  }
  set_light(amount);
  return 1;
} /* change_light */

int lockfile(string f)
{
  mixed *flags,*a,*b, ret;
  int e;
  mapping locks;

  if(!f)
  {
    ret=0;
    locks="/obj/daemon/lockd"->query_locks(this_player()->query_real_name());
    if(m_sizeof(locks))
    {
      ret++;
      a=m_indices(locks);
      b=m_values(locks);
      write("Files currently locked by you:\n");
      for(e=0;e<sizeof(a);e++)
	write(sprintf("%-25s %s %s\n",a[e],ctime(b[e][3])[4..15],b[e][2]));
    }

    locks="/obj/daemon/lockd"->query_temp_locks(this_player()->query_real_name());
    if(m_sizeof(locks))
    {
      ret++;
      a=m_indices(locks);
      b=m_values(locks);
      write("Files locked by you until reboot:\n");
      for(e=0;e<sizeof(a);e++)
	write(sprintf("%-25s %s %s\n",a[e],ctime(b[e][3])[4..15],b[e][2]));
    }
    if(!ret)
      write("You have no locked files.\n");
    return 1;
  }
  flags=get_flags(f,"crst");
  f=flags[0];

  ret="editing";
  if(!sscanf(f,"%s %s",f,ret) && flags[3])
  {
    write("You must give a reason when stealing a lock.\n");
    return 1;
  }

  f=(string)mk_path(f);

  if(!f)
  {
    write("Not a valid file name.\n");
    return 1;
  }

  if(flags[1]) /* 'c' for check */
  {
    if(ret="/obj/daemon/lockd"->check_lock(f))
      write(ret);
    else
      write("Lock intact.\n");
    return 1;
  }

  if(flags[2]) /* 'r' for release */
  {
    if(ret="/obj/daemon/lockd"->release_lock(f))
      write(ret);
    else
      write("Lock released.\n");
    return 1;
  }
  
  if(ret="/obj/daemon/lockd"->lock(f,flags[4],flags[3],ret))
    write(ret);
  else
    write("File locked.\n");
  return 1;
}

cmd_uptime()
{
  int rt, dt, days, hours, minutes, seconds;
  string msg, *parts;

  rt = _driver_stat()[0];
  msg = "\nNannyMUD was last rebooted on " + ctime(rt) + ",\n";
  
  dt = time() - rt;

  days = dt / 86400;
  dt %= 86400;

  hours = dt / 3600;
  dt %= 3600;

  minutes = dt / 60;
  seconds = dt % 60;

  parts = ({});
  if (days)    parts += ({ days    + " day"    + (days==1    ? "" : "s") });
  if (hours)   parts += ({ hours   + " hour"   + (days==1    ? "" : "s") });
  if (minutes) parts += ({ minutes + " minute" + (minutes==1 ? "" : "s") });
  if (seconds) parts += ({ seconds + " second" + (days==1    ? "" : "s") });

  msg += (implode_nicely(parts) + " ago.\n");
  msg += "NannyMud has in average used "+_driver_stat()[1]/(time()-rt) + " bytes/sec.\n";
  write(msg);
  return 1;

} /* cmd_uptime */


int query_prevent_shadow() { return 1; }
int query_prevent_move_from_statue() { return 1; }


