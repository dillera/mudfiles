#define INHERIT BRAINLIB
#include "brain.h"
#pragma strict_types

/*
   #define D(X) if(find_player("profezzorn")) tell_object(find_player("profezzorn"),(X));
*/

#define D(X) 

/* note that if you change the hashsize, the files will be rehashed, BUT
 * someone MIGHT loose their aliases in the meantime, so don't to it to
 * often /Profezzorn
 */

#ifdef PERFECT_SAVE_OBJECT
#define VERSION 0
#else
#define VERSION 1
#endif
MYMAP savevar;
static int hold_on;
#if PROF
static MYMAP prof;
#endif
int version;
static mixed curr;
static MYMAP temporary;
static int resetted;
static MYMAP modulehelp;
static MYMAP date_logged;

static void rehash_all();

#ifdef PEOP
int iswiz(object o) { return o && interactive(o) && WIZARD_P(o); }

static int usr;
static int wizzes;
static int statistics;

#ifndef OBJECT_CREATED
static int *tim;
static int *numbers;
#endif

static MYMAP waitingfor;
static object *waitforall;
static string *prevusers;

void count_people();

string getname(object o)
{
  string n;
  n=(string)call_other(o,REAL_NAME_FUNC);
  if(!n) n="";
  return capitalize(n);
}

void setwaitforall(object shell)
{
  if(!sizeof(waitforall) && !m_sizeof(waitingfor)) count_people();
  if(-1==member_array(shell,waitforall))
    waitforall+=({shell});
}

int waitfor(string s)
{
  int i;
  MYMAP q;
  s=capitalize(lower_case(s));
  if(!sizeof(waitforall) && !m_sizeof(waitingfor)) count_people();
  if(!(q=m_assoc(waitingfor,s))) q=EMPTY_MAP;
  if(i=m_assoc(q,s))
  {
    q=m_delete(q,getname(this_interactive()||this_player()));
  }else{
    q=m_assign(q,getname(this_interactive()||this_player()),1);
  }
  waitingfor=m_assign(waitingfor,s,q);
  return 1;
}


#ifdef BUGD
static int last_size;
void do_error_check()
{
  int i,e;
  string str,last,s;
  string fil,fil2,line,wiz,tmp;
  object o,sh,*p;

#if !NATIVE
  if(!this_player()) return;
#endif 
  if(-1!=find_call_out("do_error_check")) return;
  i=file_size(DEBUG_LOG);
#if 0
  if(find_player("profezzorn"))
    tell_object(find_player("profezzorn"),i+":"+last_size+":"+(this_player()->query_level())+":"+this_player()->query_real_name()+"\n");
#endif

  if(i<0) return;
  remove_call_out("do_error_check");
  call_out("do_error_check",6);
  if(i>last_size+25)
  {
    if(i>last_size+1000) last_size=i-1000;
    str=read_bytes(DEBUG_LOG,last_size,i-last_size);
#if 0
  if(find_player("profezzorn"))
    tell_object(find_player("profezzorn"),"Read error:'"+str+"'\n");
#endif
    for(last="error";sscanf(str,"%s\n%s",s,str);last=s)
    {
      if(sscanf(s,"program: %s, object: %s line %d",fil,fil2,line)==3)
      {
	if(fil2 &&
	   sscanf(fil2,WIZDIR+"/%s/",wiz) &&
	   (o=find_player(wiz)) && 
	   (sh=present("Noedid",o)) &&
	   BRAINBASE==(string)sh->query_brainbase() &&
	   sh->flagval("errors"))
	{
	  tell_object(o,"The thing in the shell tells you: "+last+"\n"+
		      " line "+line+", file "+fil+", object "+fil2+"\n");
	}
      }
    }
  }
  last_size=i;
}

#endif

void count_people()
{
  object *o,q;
  string a,b,c,*un,*uu;
  int e,u;
  mixed t;

  remove_call_out("count_people");
  call_out("count_people",45);
  statistics++;
  o=users();
  if(m_sizeof(waitingfor) || sizeof(waitforall))
  {
    un=map_array(users()-({0}),"getname",this_object())-({"Logon"});
    if(prevusers)
    {
      uu=un-prevusers;
    }else{
      uu=un;
    }
    for(e=0;e<sizeof(uu);e++)
    {
      if(t=m_assoc(waitingfor,uu[e]))
      {
	t=m_indices(t);
	for(u=0;u<sizeof(t);u++)
	{
	  if(q=find_player(lower_case(t[u])))
	  {
	    tell_object(q,"The thing in the shell tells you: "+
			uu[e]+" is here.\n");
	  }
	}
	waitingfor=m_delete(waitingfor,uu[e]);
      }
    }
    if(sizeof(waitforall))
    {
      a="";
      if(sizeof(uu))
	a+="The thing in the shell tells you: "+implode_nicely(uu)+
	  " entered the game.\n";
      if(prevusers) uu=prevusers-un; else uu=({});
      if(sizeof(uu))
	a+="The thing in the shell tells you: "+implode_nicely(uu)+
	  " left the game.\n";

      if(strlen(a))
      {
	for(e=0;e<sizeof(waitforall);e++)
	{
	  if(!objectp(waitforall[e]) ||
	     !waitforall[e]->flagval("waitforall") ||
	     !environment(waitforall[e]))
	  {
	    waitforall=waitforall[0..e-1]+waitforall[e+1..sizeof(waitforall)-1];
	    e--;
	  }else{
	    tell_object(environment(waitforall[e]),a);
	  }
	}
      }
    }
    prevusers=un;
  }
  usr+=sizeof(o);
  wizzes+=sizeof(filter_array(o,"iswiz",this_object()));
  sscanf(ctime(time()),"%s %s %s:%s:",a,c,a,b);
  if(b=="00" && statistics>2)
  {
    switch(c)
    {
    case "Jan": c=" 1"; break;
    case "Feb": c=" 2"; break;
    case "Mar": c=" 3"; break;
    case "Apr": c=" 4"; break;
    case "May": c=" 5"; break;
    case "Jun": c=" 6"; break;
    case "Jul": c=" 7"; break;
    case "Aug": c=" 8"; break;
    case "Sep": c=" 9"; break;
    case "Oct": c="10"; break;
    case "Nov": c="11"; break;
    case "Dec": c="12"; break;
    }
    a=c+"/"+a+"|";
    wizzes/=statistics;
    usr/=statistics;
    u=usr<50?50:usr;
    for(e=1;e<u+1;e++)
    {
      if(e<=wizzes)
      {
	a+=e%10?"=":"#";
      }else if(e<=usr){
	a+=e%10?"-":"+";
      }else{
	a+=e%10?" ":"|";
      }
    }
    log_file(USERDATA,a+"\n");
    statistics=usr=wizzes=0;
  }

#ifndef OBJECT_CREATED
  if(!(statistics%8))
  {
    q=clone_object(WEAPONFILE);
    sscanf(file_name(q),"%s#%d",a,e);
    if(sizeof(tim)==1000)
    {
      for(e=1;e<sizeof(tim);e++)
      {
	tim=my_exclude_array(tim,e);
	numbers=my_exclude_array(numbers,e);
      }
    }
    tim+=({time()});
    numbers+=({e});
#ifndef REMOVE_FUNC
    destruct(q);
#else
    q->REMOVE_FUNC();
#endif
  }
#endif
}
#endif

void reset()
{
#if NATIVE
  seteuid((string)MASTER->creator_file(my_file_name(this_object())));
#endif
  ::reset();
#if PROF
  if(!prof)
    prof=EMPTY_MAP;
#endif
  if(resetted) return;
  resetted=1;
#ifdef PEOP
  call_out("count_people",10);
#ifndef OBJECT_CREATED
  numbers=tim=({});
#endif
  waitingfor=EMPTY_MAP;
  waitforall=({});
#endif
  modulehelp=EMPTY_MAP;
  date_logged=EMPTY_MAP;
}

nomask static int safe()
{
  return previous_object() &&
    isshell(previous_object()) &&
    previous_object()->query_issafe();
}

#if HASHSIZE
mixed hash(string a)
{
  int e,h;
  for(h=e=0;e<strlen(a);e++) h=(h*33+a[e]) % HASHSIZE;
  return h+1;
}
#else
mixed hash(string a) { return a; }
#endif

mixed prepare(mixed what)
{
  if(pointerp(what)) return map_array(what,"prepare",this_object());
  if(!stringp(what)) return what;
  return what+"#";
}

mixed unprepare(mixed what)
{
  if(pointerp(what)) return map_array(what,"unprepare",this_object());
  if(!stringp(what)) return what;
  return my_extract(what,0,strlen(what)-2);
}

static void sav()
{
#ifdef BRAINSAVE
 save_object(BRAINSAVE+curr);
#endif
}

static void update_file()
{
  int e;
  mixed *a,*b;
  if(version==VERSION) return;
  a=m_indices(savevar);
  b=m_values(savevar);
  
  switch(version)
  {
    case 0:
    {
      for(e=0;e<m_sizeof(savevar);e++) b[e]=prepare(b[e]);
      break;

#ifdef PERFECT_SAVE_OBJECT
    case 1:
      for(e=0;e<m_sizeof(savevar);e++) b[e]=unprepare(b[e]);
      break;
#endif

    }
    default: return;
  }
  version=VERSION;
  savevar=mkmapping(a,b);
  sav();
}
	  
static void res(mixed p)
{
  mixed q,qq;
  if(curr != p)
  {
    savevar=EMPTY_MAP;
    version=0;
#ifdef BRAINSAVE
    D("restoring "+BRAINSAVE+p+".\n");
    q=catch(qq=restore_object(BRAINSAVE+p));
    D("m: "+m_sizeof(savevar)+" "+q+" "+qq+".\n");
    curr=p;
    if(!mappingp(savevar))
    {
      savevar=EMPTY_MAP;
      version=VERSION;
    }else{
      savevar=order_mapping(savevar);
    }
    if(VERSION!=version) update_file();
#endif
  }
  temporary=savevar;
}

static mixed saveit;

#ifdef BRAINSAVE
static void internal_save_it(mixed *what,string a)
{
#ifndef PERFECT_SAVE_OBJECT
  what=prepare(what);
#endif
  res(hash(a));
  savevar=m_assign(savevar,a,what);
  sav();

  /* if we failed to save it, we go back to the previous value */
  if(catch(restore_object(BRAINSAVE+curr)))
  {
    savevar=temporary;
    sav();
  }
}

void try_save()
{
  if(!pointerp(saveit)) return;
  if(!hold_on)
  {
    internal_save_it(saveit[0],saveit[1]);
    saveit=saveit[2..100000];
    if(!sizeof(saveit)) saveit=0;
  }
  if(!pointerp(saveit)) return;
  remove_call_out("try_save");
  call_out("try_save",60);
}

#endif

void save_it()
{
  string a;
  mixed *what;
#ifdef BRAINSAVE
  if(!safe()) return;
  what=(mixed *)previous_object()->query_save_data();
  a=(string)previous_object()->query_bearer();
  if(hold_on)
  {
    if(pointerp(saveit))
    {
      saveit+=({what,a});
    }else{
      saveit=({what,a});
    }
    if(find_call_out("try_save")<0)
      call_out("try_save",30);
    return;
  }
  internal_save_it(what,a);
#endif
}

mixed restore(string a)
{
  mixed foo;
  if(!safe() || !stringp(a) || (string)previous_object()->query_bearer()!=a)
    return;
  if(hold_on)
    return hold_on;

  res(hash(a));
  foo=m_assoc(savevar,a);
  if(!foo)
  {
    foo=read_file(BRAINSAVE+"hashsize");
    if(!foo || !sscanf(foo,"%d",foo) || foo!=HASHSIZE)
    {
      rehash_all();
      return hold_on;
    }
    return 0;
  }
#ifdef PERFECT_SAVE_OBJECT
  return foo;
#else
  return unprepare(foo);
#endif
}

static MYMAP *helps,*ac;

static MYMAP get_help(int w)
{
  MYMAP d,c;
  mixed h,wh;
  int e;
  string dum,path,*tmp;
  tmp=my_explode(my_file_name(this_object()),"/");
  path=implode(tmp[0..sizeof(tmp)-2],"/");

  if(helps) return helps[w];
  h=({
#ifdef TIME
      "Time",":Shows what time it is.\n"+
      "Uses the shellstatus 'timezone' to determine where you are.\n"+
      "See also: shellstatus",
#endif
#ifdef AUTO
      "auto",":auto <command>\n"+
      "Does the command for everyone who is present. "+
      "example: 'auto smile' will make you smile at everyone in the room. "+
      "This will cost 1 spellpoint per person present.\n",
#endif
#ifdef WALK
      "walk",":walk <direction> to <word>\n"+
      "This command lets you walk in a specific direction until <word> "+
      "appears in a room description. You might want to use brief mode "+
      "when using this command. The command can be aborted with 'break' "+
      "as with do and dotimes.\n",
#endif

      "last",":last <n>\n"+
      "Last repeats the last <n> commands in your history list. If you don't specify "+
      "a number, 1 will be assumed.\n",

      "fork",":fork shell to <player>\n"+
      "Fork simply conjures up a new shell and gives it to <player>. "+
      "This will cost you 15 spellpoints.",

      "about","Note from the author:\n"+
      "This shell is updated constantly, if there is anything you would like "+
      "added to or changed in this object you should use the 'Tip' command and "+
      "I will try to oblige (within the rules of the mud of course :) "+
      "It has taken me several years to write this tool to what it is presently "+
      "and it's getting kind of tricky to come up with good improvements myself. "+
      "So please! If you got a suggestion let me know. This object can also be "+
      "obtained by ftp from ftp.lysator.liu.se (/pub/lpmud/lpc/profezzorn/)\n"+
      "                 Profezzorn\n"+
      "PS: Remember that commands mentioned in the 'news' section can be excluded "+
      "from your mud if it is against some rule there....",

      "defaults",":defaults\n"+
      "Installs some nice aliases that might be useful. "+
      "Please note that this will redefine any aliases with "+
      "the same names.",

      "print",":print [-sp] <expr>\n"+
      "Print evaluates an expression and writes the result. "+
      "Use 'shellhelp expressions' for info on expressions. "+
      "If -s is used the result will be printed raw (without 'Result :') "+
      "If -p is used, an indenting prettyprint-algorithm will be used.",

      "Set",":Set <var> <value>\n"+
      "With this you can set/list variables in your shell. "+
      "Use 'Set' to list variables. There is a short form "+
      "of this command: instead of 'Set <var> <value>' "+
      "You can use: '$<var>=<value>' "+
      "Don't forget to read about expressions.",

      "cont",":cont\n"+
      "Cont continues a break:ed do/dotimes.\n",

      "com",":com [-m] <command> <argument1> <argument2> ......\n"+
      "Com will evaluate all its arguments and then use the "+
      "result as a command with arguments. "+
      "i.e. 'com sm+ile 6+7' "+
      "will result in 'smile 13' being executed. "+
      "If you use -m and the command is defined by the shell then "+
      "it will pipe all output through a very simple more-utility.",

      "page",":page <command>\n"+
      "Page will execute the command given and page-break the output in "+
      "a more-similar manner. It may not work with some commands though.",

      "if",":if <expr> then <command1> [else <command2>]\n"+
      "If will evaluate <expr> and run command1 if it was true "+
      "else it will run command2. It is also possible to use nested "+
      "if...then---else",

      "wait",":wait <number>\n"+
      "Wait will pause a do/dotimes command for about <number> "+
      "seconds. $waiting will be set to 1 in the meantime.",

      "alias",":alias [-lc] <name> <definition>\n"+
      "Alias lets you define your own commands. "+
      "i.e. 'alias l look' "+
      "allows you to write 'l' instead of 'look'. "+
      "Alias also takes arguments, %1 will be replaced by "+
      "the first argument given %2 to the next etc. "+
      "If there are more arguments than '%n' then those "+
      "left will be appended to the end of the definition. "+
      "If you put '%@' in the definition, arguments left will NOT "+
      "be appended to the definition, but inserted at that place. "+
      "It is also possible to have several '%@'. 'Alias -l <alias>' will "+
      "list that alias or try to find aliases that starts like that. "+
      "'alias -c' will clear _all_ your aliases.",

      "nicknames",":nickname <nickname> <real name>\n"+
      "Nicknames allows you to shorten a person's name to some nickname. "+
      "If you for example do 'nickname prof profezzorn' then "+
      "'prof' will be replaced in every command you do with 'profezzorn'. "+
      "Be careful when using this feature as you can redefine words "+
      "that must not be replaced.",

      "%",":%<nr/start>[:s/<something>/<something else>/]\n"+
      "This command allows you to execute commands from your "+
      "history-list, the argument given can be the following:\n"+
      "A number: representing a command from your list.\n"+
      "A negative number: will execute the command that number of places back "+
      "Using :s/<something>/<something else>/ replaces something for "+
      "something else before executing the command is executed.",

      "::","::<command>\n"+
      "With this syntax you can do a command even if you have an alias "+
      "with the same name. This allows you do do aliases like: "+
      "'alias smile ::smile sadly' so that when you smile, you will always"+
      "smile sadly.",

      "\\","\\<command>\n"+
      "With this syntax you can call a command, even if there is a command "+
      "int the shell with the same name. Let's assume there is a command "+
      "called 'wait' beside the 'wait' in the shell. To do that other 'wait'"+
      "command, you write '\\wait'.",

      "dotimes",":dotimes <nr> <command>\n"+
      "This command simply executes <command> <nr> times. "+
      "The variable $donum will count down while doing so.\n"+
      " See also: Set",

      "Tip",":Tip <message>\n"+
      "This just saves the message so that I can read it later. "+
      "Try to make accurate requests.",

      "shellhelp",":shellhelp [browse] <topic>\n"+
      "Well you can probably already use this feature. But here are a few hints: "+
      "The shell always does a regexp search after topics, this means that any "+
      "substring of a topic is valid. If the browse option is used the shell will "+
      "scan the actual helptexts after that particular word and print any topic "+
      "containing it.",

      "shellstatus",":shellstatus [<flag> <value>]\n"+
      "Shellstatus can show or change the internal flags of the shell. "+
      "If you don't give any arguments at all, your flag settings will be listed. "+
      "Currently only these flags are available for mortals:\n"+
#ifdef TIME
      " timezone: Should be your local timezone for the command Time.\n"+
#endif
#ifdef ECHO
      " echo    : Writes what you actually said/told/shouted.\n"+
#endif
      " notify  : turns off some output from the shell.\n"+
      " invis   : with this set the shell doesn't show at all.\n"+
      " shell   : Turns entire shell on or off. (except shellstatus).\n"+
      " columns : Set how many columns your screen has.",
	 
      "expressions","When evaluating something the shell does the following: "+
      "$<var> is replaced by it's value. "+
      "\"<string>\" will quote a string. "+
      "It evaluates: +,-,*/,!,&,|,>,<,= "+
      "Observe that !,&,| are LOGICAL not bitwise "+
      "Please note that '-' cannot be used first in an argument or the shell"+
      "will try to interpret the expressions as flags. Use '0-' if you wish "+
      "a negative number instead. "+
      "You can also use 'rnd(x)' to get a random integer: 0<=rnd(x)<x "+
      "and holding(<string>) to evaluate if you are holding anything "+
      "called 'string'.\n",

      "break",":break [shell]\n"+
      "'break' aborts a do/dotimes. "+
      "If do or dotimes crashes it is often a good idea to do 'break' "+
      "before trying another do or dotimes or it might not work. :( "+
      "'break shell' gets rid of the shell and all its wonderful commands.",

      "history",":history\n"+
      "This command will give you a list of your recently "+
      "executed commands.\n"+
      " history objects        :List remembered objects.\n"+
      "See also: %",

      "do",":do <cmd>,<cmd>,<cmd>,<cmd>......\n"+
      "Do takes a list of commands separated by commas and "+
      "executes them sequentially.",
      });
  wh=
    ({
      "wizard",
      "Wizcommands:\n"+
      " Undo [-r] <object>     :Destructs an object and everything in it.\n"+
      " redo [-nafl] <object>  :Updates and replaces an object.\n"+
      " Clone [-n] <name>      :Clones an object.\n"+
      " Load <name>            :Loads an object.\n"+
      " id [-landb] <object>   :Shows status of an object.\n"+
      " history objects        :List remembered objects.\n"+
      " Cat <file> <beg> <end> :Read a file from <beg> to <end>.\n"+
      " Tail <file> <offset>   :Read the end of a file.\n"+
      " Rm <file>              :Delete a file.\n"+
      " Ls [-lf*] <file/dir>   :Show contents of a dir.\n"+
      " Updir <directory>      :Updates all the files in <directory>\n"+
      " tag <object>           :Start/Stop tagging <object>.\n"+
      " Move [-m] <obj> <obj>  :Move object into object.\n"+
      " eval <expression>      :Evaluate an LPC statement.\n"+
      " mktable <expr> <lpc> ..:Make a table from expressions.\n"+
      " patch obj fun <arg>    :Patches fun in obj with <arg>.\n"+
      " Find <template>        :Finds all objects that match the template.\n"+
      " trace <flags> [com]    :Set your tracelevel or traces through a command.\n"+
      " traceprefix <prefix>   :Set your traceprefix.\n"+
      " Make <what>            :Manufacture an object or monster\n"+
      " gauge <command>        :Check how much eval_cost a command takes.\n"+
      " link <file>            :Link or list modules\n"+
      " unlink <file>          :Unlink a module\n"+
      " shellcmd [module]      :List commands in the shell or a module\n"+
      " egrep [-vlinh] <match> <file> :Grep a file for all regexp matches\n"+
#ifdef PEOP
      " peopledata             :Shows the average number of people and wizards.\n"+
      " waitfor <player>       :Waits until a player logs on.\n"+
#endif
#ifdef my_query_actions
      " Localcmd [living] [cmd]:Finds out which object defines which verb.\n"+
#endif
      "",
      "link",":link <module filename>\n"+
      "Link adds a module to the shell, or if you don't give any file, it "+
      "lists all modules currently present in the shell.\n"+
      "See also: unlink, modules, shellcmd", 

      "unlink",":unlink <module filename>\n"+
      "Unlink removes a module from the shell.\n"+
      "See also: link, modules, shellcmd",

      "shellcmd",":shellcmd <part of module filename>\n"+
      "Shows all commands in modules which filename matches the argument. "+
      "If no argument is given, all modules are listed.\n"+
	"See also: link, unlink, modules",
      
      "modules",
      "Modules are optional packages of functions dynamically linked into "+
      "the shell with the link/unlink commands. You can of course make your "+
      "modules if you like and here is how you do it:\n"+
      "These two lines should be in the beginning of the module:\n"+
      "#define INHERIT MODULE\n"+
      "#include \""+path+"/brain.h\"\n\n"+
      "And then you have to know this:\n"+
      "module_init() is called when the module is linked to a shell.\n"+
      "query_name(), should return what the module is called, it's for "+
      "shellhelp, so 'shellhelp <name returned by query_name()> will "+
      "show the long() of this module.\n"+ 
      "short() should return a short (one line) description of the module.\n"+
      "long() should _return_ a description of the same format as "+
      "'shellhelp wizard'\n"+
      "query_clone_module() should be defined to return 1 if the module "+
      "needs to be cloned to the player (often it doesn't have ot be)\n"+
      "query_open_module() should return 1 if you want the module to "+
      "show up on the 'other modules available' list in 'link'.\n"+
      "module_help() should return an array with the first element being the "+
      "helptopic, the second the help for that topic, the third the next "+
      "topic etc. etc.\n"+
      "reset() is called when the object is created, and now and then "+
      "afterwards too, there is no argument differing them apart you will "+
      "have to separate them yourself. And don't forget to call ::reset()\n"+
      "Note that modules are not cloned, all linking goes directly to the "+
      "master object.\n"+
      "From module_init() you can use add_command(), "+
      "it has the following syntax:\n"+
      "void this_shell()->add_command(string format,string func)\n"+
      "where func is the function to be called when the command is executed "+
      "and format is a string that contains the verb + a format info. "+
      "the format info uses % in about the same manner as sscanf and accepts "+
      "the following types:\n"+
      " o : an object (found with present).\n"+
      " s : a string (works as sscanf)\n"+
      " d : an integer (works as sscanf).\n"+
      " O : object (evaluated)\n"+
      " S : string (evaluated)\n"+
      " P : array (evaluated)\n"+
      " D : integer (evaluated)\n"+
#ifdef mappings
      " M : mapping (evaluated)\n"+
#endif
      " A : any type (evaluated)\n"+
      " R : an object, loaded if not already loaded.\n"+
      " F : a file (expanded etc.)\n"+
      " - : flags, example \"%-bar\" would give an array of 3 integers, "+
      "that are true if that flag is given.\n"+
      "and the following modifiers:\n"+
      " * : Add the array type to the type.\n"+
      " @ : If the corresponding argument is an array this will cause the "+
      "function to be called once for each index in that array.\n"+
      " + : Consider next type as a modifyer and add it's type to the "+
      "possible types for this argument.\n"+
      " # : Make the rest of the arguments on this line use the same "+
      "format info and pack them into an array.\n"+
      " ~ : Makes this % optional, if not given by the user a zero is sent "+
      "to the called function.\n"+
      "The following functions has been added/redefined for your convenience:\n"+
      "this_shell() returns the shell that called the module.\n"+
      "flagval(string flag) returns the value of a shellstatus flag.\n"+
      "assign(string var,mixed what) sets the shell-variable (not flag) "+
      "var to the value of what, useful for storing temporary data.\n"+
      "get_assign(string var) returns the value of the variable var.\n"+
      "the_bearer() is almost equal to this_player), but returns the "+
      "registered bearer of this_shell().\n"+
      "fast_desc(mixed q) retuns a short description of q.\n"+
      "enhance_result(mixed q) returns a longer description of q.\n"+
      "print_enhance_result(mixed q) prints a long description of q.\n"+
      "write(string s) is equal to write(), but workes with shell-pipes.\n"+
      "put_data(mixed data) enables your module to save data in the shell\n"+
      "get_data() returns those data.\n"+
      "and\n"+
      "read_file(), read_bytes(), file_size() and get_dir() has been "+
      "redefined to use the same permissions as the shell.\n"+
      "If this seems awfully confusing, check out if there is a directory "+
      "called /open/shellmodules/ the files that might be in it.",

#ifdef PEOP
      "peopledata",":peopledata\n"+
      "It shows the average number of people logged the last hours with the "+
      "last hour on the top. '=' means wizards and '-' means mortals.",

      "waitfor",":waitfor <player>\n"+
      "It waits until the player logs on and tells you so when "+
      "he/she/it does. It can be stopped with the command "+
      "'break waitfor <player>'",
#endif
      "egrep",":egrep [-vlihn] <regexp> <file>\n"+
      "Egrep works just about as the unix-command egrep, it simply writes all "+
      "lines in the file matching the regexp. For those not familiar "+
      "with regexp, just use a word you want to find. There are also some "+
      "flags available:\n"+
      " -i ignore case\n"+
      " -h do not display filenames, not meaningful with -l\n"+
      " -v inverse result\n"+
      " -l show only filenames of files that conains the regexp (once)\n"+
      " -n show line numbers\n",

      "trace",":trace <off|com|call|call_other|return|args|exec|heart_beart\n"+
      "            |apply|obj_name|number>\n"+
      "Trace sets your tracelevel or traces through a single command. "+
      "'Trace <flags>' where flags is one or more of: "+
      "call, call_other, return, args, exec, heart_beat, apply and obj_name "+
      "sets your tracelevel to those flags. You can also specify a number "+
      "directly like 'trace 2'. "+
      "If you include the keyword 'off' among the flags then the tracelevel "+
      "will be set to zero. "+
      "Trace without an argument will view your current tracelevel. "+
      "'trace <flags> com <command>' will trace through the command with the "+
      "the flags specifyed and then set your tracelevel to zero. "+
      "Read the /doc/efun/trace for more info.",

      "traceprefix",":traceprefix <prefix>\n"+
      "Set or clear your traceprefix. "+
      "With traceprefix set tracing will only occur in objects starting with "+
      "that prefix. If you don't specify any prefix your current will be shown "+
      "'traceprefix clear' will clear the prefix to zero.",

      "Make",":Make <thing|weapon|armour|monster|food|drink>\n"+
      "Make lets you manufacture objects or monsters 'on the fly' "+
      "Make will ask you all the specifics of the object you want to make. "+
      "If you just press return as an answer the default (the value between the "+
      "pharentheses) will be assumed. Please observe that all input to make is "+
      "evaluated so if you wish to input a string with an operator in it then "+
      "you must quote it with '\"'. Also note that there is no problem using "+
      "'\\n' within your strings.",

      "gauge",":gauge <command>\n"+
      "Gauge just runs the command given to it and outputs how much eval_cost "+
#ifdef RUSAGE
      "and Cpu-time "+
#endif
      "the command took. Great for checking optimizations.",

#ifdef my_query_actions
      "Localcmd",":Localcmd [living] [command]\n"+
      "Localcmd checks what commands a player has at the moment: "+
      "'Localcmd' Works approximately like the normal localcmd. "+
      "'Localcmd <living>' Shows that living's commands instead. "+
      "'Localcmd <living> <command>' Finds out which object defined that verb.",
#endif
      "Find",":Find [not] [file <pattern>] [short <regexp>] [inherit <object>] [hb] [living]\n"+
      "Find searches all objects in the mud and returns all objects that "+
      "match the template. The template consists of one or more of the "+
      "following:\n"+
      "file <pattern>   : Find all files that match <pattern>.\n"+
      "                 : '*' matches any string\n"+
      "living           : Find living objects.\n"+
      "hb               : Find objects with heartbeats.\n"+
      "inherit <object> : Find all objects which have inherited <object>.\n"+
      "short <regexp>   : Find all objects whos short matches regexp.\n"+
      "If you specify more than one of these options, all of them must be "+
      "true for the object to be reported. " +  
      "All the options can be prepended with 'not' to invert them.  "+
      "The result of the search will be an array placed in the variable "+
      "$found.",

      "dangerlevels","When you destruct something the shell will consider the dangerlevel "+
      "before doing so and if the dangerlevel is too high for that object "+
      "it will not be destructed. Current dangerlevels are:\n"+
      "level:   Don't destruct:\n"+
      "  5      Yourself\n"+
      " 10      Players\n"+
      " 20      This shell\n"+
      " 30      Castles\n"+
      " 35      Doors\n"+
      " 40      Souls\n"+
      " 50      Livings\n"+
      " 75      Objects in /room\n"+
      "100      Anything\n",

      "Updir",":Updir <directory>\n"+
      "Updir updates all the files in a directory, if <directory> "+
      "is omitted your homedir will be updated. After that you "+
      "will have to reload all important files. "+
      "This works more slowly but is better than the ordinary 'updir'",

      "Ls",":Ls [-flags] <dir/file>\n"+
      "Ls given a filename will show its size, "+
      "if given a directory it will show the files in it. "+
      "flags can be one or more of the following: "+
      "'-l' Shows files in one column with their size. "+
      "'*l' Shows what files are loaded at the moment. "+
      "'-f' Turns off filesize and directory check, much faster.",

      "exec",":exec [<LPC expression>]\n"+
      "Same as eval in fizban mode, otherwise does nothing.",

      "eval",":eval [<LPC expression>]\n"+
      "Lets you evaluate LPC from inside the mud. "+
      "If you omit the expression you will be prompted for one "+
      "then you can enter several lines. This command now also "+
      "allows you to use the variables from the shell. "+
      "ie. 'set g \"smile\"' and 'eval PR(smile+\"\\n\")' "+
      "would write 'smile' to you. It is also possible to call a shell-expression "+
      "from within eval, this is done by calling the function shell() with a "+
      "string containing the expression. Thus 'print me' and 'eval shell(\"me\")' "+
      "is equivalent. It is also possible to use a library of functions "+
      "in your dir - if you have the files 'shell_library.c' & 'shell_library.h' "+
      "they will be inherited, included respectively when you evaluate lpc. "+

      "This feature is also used by ffilter() & fmap(). \n"+
      "Note: functions in shell_library.c can be static for security.\n",

      "Move",":Move [-m] <object1> <object2>\n"+
      "Moves <object1> into <object2>. "+
      "If '-m' is used Move will move the object into object2 even if it is already "+
      "there. This flag will always move objects silently.",

      "Rm",":Rm [<file>]\n"+
      "Remove <file>. "+
      "If <file> is omitted, your log will be assumed.",

      "Tail",":Tail [<file>]\n"+
      "Write <file> to screen from line <begin line>. "+
      "If <file> is omitted your log will be assumed.",

      "Cat",":Cat [<file>] [<begin line>] [<offset>]\n"+
      "Write some lines from <file>. "+
      "If <offset> is omitted 20 lines will be shown. "+
      "If <file> is omitted the end of the mud's log will be shown.",

      "tag",":tag <object>\n"+
      "start/stop following an object. "+
      "(preferably a player or monster)",

      "patch",":patch <object> <function> [<argument>]\n"+
      "Patch will try to run <function> in <object> with <argument>. "+
      "The result will be stored in '$arg'.",

      "Load",":Load <object>\n"+
      "This is just a smarter version of 'load' - handles wildcards and multiple filenames.",

      "Clone",":Clone [-n] <object>\n"+
      "This is just a smarter version of 'clone' if you use '-n' to disable "+
      "catching when cloning - handles wildcards and multiple filenames.",

      "redo",":redo [-nafl] <object>\n"+
      "redo will try to replace an object with an updated and "+
      "cloned new one. It will replace in the position of the old object "+
      "and move all things back into the object. If something fails, an "+
      "error message will be given, and possibly some lines from the log. "+
      "If '-n' is used, redo will replace the object without reloading. If "+
      "'-a' is used, all objects in the inherit_list of that object will be "+
      "updated too. If -f is used, redo will not try to figure out if the "+
      "object needs to be updated or not. With -l, redo will not print "+
      "anything from the log, even if an error occurs.",

      "Undo",":Undo [-r] <object>\n"+
      "Undo will destruct one or more objects. "+
      "Dangerlevels will be considered. "+
      "If '-r' is used, all objects in <object> will be recursivley destructed too.\n"+
      "See also :'dangerlevels'",

      "id",":id [-flags] object\n"+
      "This will show some info about an object, flags are:\n"+
      " -l will force long output.\n"+
      " -a will force extra long output.\n"+
      " -n will turn off the normal long output.\n"+
      " -d will show system internal info (only).\n"+
      " -t will show age of master objects.\n",

      "filters","The '.' used to find objects relative to others - also works with arrays in "+
      "general. 3 operators can be used with all arrays: map, filter & add. "+
      "All of these apply a function to an array. "+
      "'foo.add' adds all elements of the array foo together. "+
      "'foo.filter(bar)' evaluates the expression bar for every element in foo with $x "+
      "set to the current element. The result will be an array of all objects "+
      "for which bar was true. "+
      "'foo.map(bar)' evaluates bar in much the same way as 'filter' but returns "+
      "an array of the results of bar.\n"+
      "example: users.map($x->query_level()).add or users->query_level().add will "+
      "return the sum off all the players levels.\n"+
      "example: users.filter(25=$x->query_level()) returns all archwizards logged on. "+
      "Now there is also a fmap() and ffilter() which takes an LPC-expression "+
      "and works much faster. Otherwise they work like map() and filter().",
      
      "objects",
      "The shell tries to find an object in the following order:\n"+
      " 1: history                           : %<number or id>\n"+
      " 2: variable                          : $<varname>\n"+
      " 3: last object                       : %% or last\n"+
      " 4: yourself                          : me\n"+
      " 5: your env                          : env\n"+
      " 6: all players                       : users\n"+
      " 7: all objects in the call_out list  : called\n"+
      " 8: present                           : <id>\n"+
      " 9: a player                          : <name>\n"+
      "10: a living                          : <name>\n"+
      "11: an object                         : <filename>\n"+
      "When an object has been found relative finding can be used:\n"+
      " 1: an object's environment                       : <object>.env\n"+
      " 2: the n:th object in an object                 : <object>.i<n>\n"+
      " 3: object with <id> in object                   : <object>.<id>\n"+
      " 4: everything in the object                     : <object>.all\n"+
      " 5: everything in <object> AND the object itself : <object>.allx\n"+
      " 6: recursively find all in <object>             : <object>.dall\n"+
      " 7: recursive and inclusive find all in object    : <object>.dallx\n"+
      " 8: find all objects in <object> with id==<id>   : <object>.all(\"<id>\")\n"+
      " 9: find the masterobject to an object           : <object>.master\n"+
      "10: find the object shadowing object             : <object>.shadow\n"+
      "11: find all objects shadowing object            : <object>.shadows\n"+
      "12: find all the clones of an objectt            : <object>.clones\n"+
      "13: first object that has <str> in its filename  : <object>.<str>\n"+
      "All relative operators work with arrays.\n"+
      "i.e. To find this object you can use:\n"+
      "'shell' , 'me.shell' , 'env.me.brain' or\n"+
      "'me.i0' ( shell tries to be first in inventory )",

      "debugging","This is just a tip:\n"+
      "There are two nice functions in the shell that can be patched from the outside:\n"+
      "void assign(string var,mixed value): "+
      "sets the variable 'var' in the shell to value.\n"+
      "mixed get_assign(string var): "+
      "gets the value of the variable var.\n"+
      "These functions can easily and advantageously be used in debugging.",

    "expressions","When evaluating something the shell does the following: "+
      "$<var> is replaced by it's value. "+
      "\"<string>\" will quote a string. "+
      "It evaluates: +,-,*/,!,&,|,>,<,= "+
      "Observe that !,&,| are LOGICAL not bitwise "+
      "Please note that '-' cannot be used first in an argument or the shell"+
      "will try to interpret the expressions as flags. Use '0-' if you wish "+
      "a negative number instead. "+
      "You can also use 'rnd(x)' to get a random integer: 0<=rnd(x)<x "+
      "and holding(<string>) to evaluate if you are holding anything "+
      "called 'string'.\n"+
      "The following only applies to wizards: "+
      "Your shell also handles arrays with: ({ }) [] "+
#if mappings
      "And mappings with: ([ : ]) [] "+
#endif
      "Calls to other objects can be made like: foo->bar(baz) "+
      "and calls to other objects can be made like: foo->bar(baz) "+
      "If foo is an array then bar will be called with baz in all objects in foo. "+
      "The result will be an array with the results of foo. "+
      "There are other kinds of filters like this - see 'shellhelp filters' "+
      "As a wiz you can also use eval(Lpc-expresion) to eval LPC "+
      "within a shell-expression. "+
      "There is also a built-in function OBJ(name) wich can handle objects with names "+
      "Including '-' and other operators, however it does NOT handle relative finding "+
      "however, relative finding can be specifyed like OBJ(object).env or similar... "+
      "The other built-in functions are: eval() get_files() & get_loaded(). eval() takes "+
      "an LPC-expression as argument and evaluates it. The others take a number of files "+
      "(specifyed with wildcards and/or separated by space) and return all existing files or "+
      "all loaded object from that those files respectively.\n"+
      "Read more with 'shellhelp objects'",

   "history",":history [objects]\n"+
      "Without arguments this command will give you a list of your most recently "+
      "executed commands.\n"+
      "With arguments this will show the object-history list.\n"+
      "See also: %",

   "shellstatus",":shellstatus [<flag> <value>]\n"+
      "Shellstatus can show or change the internal flags of the shell. "+
      "If you don't give any arguments your flag settings will be listed. "+
      "Currently only these flags are available for mortals:\n"+
#ifdef ECHO
      " echo    : Writes what you actually said/told/shouted.\n"+
#endif
      " notify  : turns off some output from the shell.\n"+
      " invis   : With this set the shell doesn't show at all.\n"+
      " shell   : Turns entire shell on or off. (except shellstatus).\n"+
      " columns : Set how many columns your screen has.\n"+
      "For wizards there are more flags:\n"+
      " nice    : should the shell be fast or nice to the mud?\n"+
      " muffle  : turns off messages to others when using the shell.\n"+
      " danger  : This is the internal dangerlevel, usually an integer 0-100\n"+
      " light   : Setting this to a positive number makes the shell into a light source.\n"+
      " short   : With this you can set the shell's short description.\n"+
      " name    : What id shall the shell respond to?\n"+
#ifdef PEOP
      " invscan : You will be notifyed when an object enters your inventory.\n"+
#endif
      " verbatim: If verbatim is on, the shell always describes objects\n"+
      "           with filenames, not living names.\n"+
      " errors  : Makes the shell alert you when an error occurs in one\n"+
      "           of your objects.\n"+
      " extra_look: Let's you decide what the shell's extra_look should return.\n"+
      " waitforall: Tells you who logged on or off.\n"+
      " fizban_mode: Make the shell emulate fizban.staff a little.\n"+
      "see also: 'dangerlevels' and 'Undo'",

   "do",":do [-s] <cmd>,<cmd>,<cmd>,<cmd>......\n"+
      "Do takes a list of commands separated by commas and "+
      "executes them sequentially. Wizards can use the -s switch "+
      "and do then works as one command: fast and silent."+
      "Try 'defaults' and 'alias' for some examples.",

    "break",":break [shell | find]\n"+
      "'break' aborts a do/dotimes. "+
      "If do or dotimes crashes it is often a good idea to do 'break' "+
      "before trying another do or dotimes or it might not work. :( "+
      "'break shell' _always_ destructs the shell. 'break find' stops "+
      "an ongoing Find command.",
    "mktable",":mktable <array> <lpc-expr.> <lpc-expr.> ...\n"+
      "This command makes tables of things. The first expression is "+
      "supposed to be a shellexpression that returns an array and all the "+
      "other expressions are lpc-expressions of the same type as you give "+
      "to eval. Each lpc-expression will become a column in the resulting "+
      "table and the variable x will be whatever is in the first column "+
      "on that row. Is that to complicated? Here are some examples:\n"+
      ">mktable users x->query_level() x->short()\n"+
      ">mktable me.all x->query_value() x->query_name()\n"+
      "See also: eval, filters and expressions.",
      });

  d=EMPTY_MAP;
  for(e=0;e<sizeof(h);e+=2) d=m_assign(d,h[e],h[e+1]);
  c=copy_mapping(d);
  for(e=0;e<sizeof(wh);e+=2) d=m_assign(d,wh[e],wh[e+1]);
  helps=({({c,mkmapping(m_values(c),m_indices(c))}),({d,mkmapping(m_values(d),m_indices(d))})});
  return helps[w];
}

static MYMAP get_actions(int w)
{
  string *blob,*blob2,dum;
  MYMAP d,c;
  int e;

  if(ac) return ac[w];

  blob=
  ({
#ifdef ECHO
    "backecho","say",
    "backecho","shout",
    "echotell","tell",
#endif
#ifdef PEOP
    "peopledata","peopledata",
#endif
#ifdef TIME
    "Time","Time",
#endif
#ifdef AUTO
    "auto","auto",
#endif
#ifdef WALK
    "walk","walk",
#endif
    "macro","alias",
    "_do","do",
    "dotimes","dotimes",
    "help","shellhelp",
    "tip","Tip",
    "_if","if",
    "set","Set",
    "com","com",
    "page","page",
    "_wait","wait",
    "_stop","break",
    "cont","cont",
    "defaults","defaults",
    "print","print",
    "shellstatus","shellstatus",
    "fork","fork",
  });

  blob2=
  ({
#ifdef PEOP
    "waitfor","waitfor",
#endif
    "expand","expand",
    "dir","Ls",
    "clone","Clone",
    "load","Load",
    "identify","id",
    "patch","patch",
    "undo","Undo",
    "undo","Destruct",
    "update","redo",
    "tag","tag",
    "_cat","Cat",
    "_tail","Tail",
    "_rm","Rm",
    "my_move","Move",
    "eval","eval",
    "eval","exec",
    "find","Find",
    "updir","Updir",
    "set_trace","trace",
    "gauge","gauge",
    "set_traceprefix","traceprefix",
    "make","Make",
    "link","link",
    "unlink","unlink",
    "shellcmds","shellcmd",
    "egrep","egrep",
    "mktable","mktable",
#ifdef my_query_actions
    "commands","Localcmd",
#endif
#if PROF
    "see_profile","PROFILE",
#endif
  });

  d=EMPTY_MAP;
  for(e=0;e<sizeof(blob);e+=2) d=m_assign(d,blob[e+1],blob[e]);
  c=copy_mapping(d);
  for(e=0;e<sizeof(blob2);e+=2) d=m_assign(d,blob2[e+1],blob2[e]);
  ac=({c,d});
  return ac[w];
}



static string *query_list,*query_list2,*not_query_list;

static void fix_query_lists()
{
  string *f,*ff;
  if(query_list) return;
  f=explode(read_file(BRAINQUERYLIST),"#\n");
  query_list=explode(f[0],"\n");
  query_list2=explode(f[1],"\n");
  not_query_list=explode(f[2],"\n");
}

static string *get_query_list() { fix_query_lists(); return query_list; }
static string *get_query_list2() { fix_query_lists(); return query_list2; }
static string *get_not_query_list()
{ fix_query_lists(); return not_query_list; }

static mixed *flags;
static mixed *_get_flags(int wiz)
{
  mixed *dum;
  int e;
  if(flags) return flags[wiz];
  dum=({
    "shell",    T_STATUS,           TRUE,
    "echo",     T_STATUS,           FALSE,
    "notify",   T_STATUS,           TRUE,
    "invis",    T_STATUS,           FALSE,
    "columns",  T_NUMBER,           80,

    "muffle",   WIZFLAG | T_STATUS, FALSE,
    "nice",     WIZFLAG | T_STATUS | NEEDEDFLAG , TRUE,
    "danger",   WIZFLAG | T_NUMBER, 99,
    "light",    WIZFLAG | T_NUMBER, 0,
    "verbatim", WIZFLAG | T_STATUS, FALSE,
    "extra_look",WIZFLAG| T_STRING | T_NUMBER, 0,
    "fizban_mode",WIZFLAG| T_STATUS, 0,
    "short",    WIZFLAG | T_STRING | NEEDEDFLAG, "a shell",
    "name",     WIZFLAG | T_STRING | NEEDEDFLAG, "shell",
#ifdef TIME
    "timezone", T_NUMBER,           TIME,
#endif
#ifdef BUGD
    "errors",   WIZFLAG | T_STATUS, TRUE,
#endif
#ifdef PEOP
    "invscan",  WIZFLAG | T_STATUS, FALSE,
    "waitforall", WIZFLAG | T_STATUS, FALSE,
#endif
    });
  flags=({({EMPTY_MAP,EMPTY_MAP}),({EMPTY_MAP,EMPTY_MAP})});
  for(e=0;e<sizeof(dum);e+=3)
  {
    flags[1][0]=m_assign(flags[1][0],dum[e],dum[e+1]);
    flags[1][1]=m_assign(flags[1][1],dum[e],dum[e+2]);
    if(!(dum[e+1] & WIZFLAG) || (dum[e+1] & NEEDEDFLAG))
    {
      flags[0][0]=m_assign(flags[0][0],dum[e],dum[e+1]);
      flags[0][1]=m_assign(flags[0][1],dum[e],dum[e+2]);
    }
  }
  return flags[wiz];
}

#if PROF
MYMAP get_profile() { return prof; }

void profile(string arg)
{
  prof=m_assign(prof,arg,m_assoc(prof,arg)+1);
}
#endif

/* share and enjoy */
mixed enjoy(string what,int wizard)
{
  if(!safe() || !what) return 0;
  switch(what)
  {
    case "help": return get_help(wizard);
    case "flags": return _get_flags(wizard);
    case "actions": return get_actions(wizard);
    case "query_list": return get_query_list();
    case "query_list2": return get_query_list2();
    case "not_query_list": return get_not_query_list();
#ifdef PEOP
#ifndef OBJECT_CREATED
    case "timedata": return ({tim,numbers});
#endif
#endif
  }
}

static mixed get_help_for_module(string module)
{
  MYMAP data,invdata;
  int e;
  string *d,*d2;

  if(data=m_assoc(modulehelp,module))
  {
    return data;
  }else{
    data=EMPTY_MAP;
    invdata=EMPTY_MAP;
    d=(string *)module->module_help();
    d2=(string *)module->module_help2();
    if(!d2) d2=({});
    if(!pointerp(d) || sizeof(d)%2 || !pointerp(d2) || sizeof(d2)%2)
    {
      write("Error in modulehelp.\n");
    }else{
      d+=d2;
      for(e=0;e<sizeof(d);e+=2)
      {
	data=m_assign(data,d[e],d[e+1]);
	invdata=m_assign(invdata,d[e+1],d[e]);
      }
      modulehelp=m_assign(modulehelp,module,({data,invdata}));
    }
  }
  return ({data,invdata});
}

void flush_modulehelp(string module)
{
  modulehelp=m_delete(modulehelp,module);
}

mixed get_modulehelp(string str,mixed *modules)
{
  int e,has;
  string *t,h;
  MYMAP d;

  has=modules[1];
  modules=modules[0];
  t=({});
  for(e=0;e<sizeof(modules);e++)
  {
    d=get_help_for_module(modules[e])[0];
    if(!d) continue;
    if(h=m_assoc(d,str)) return h;
    t+=m_indices(d);
  }
  t=regexp(t,str);
  if(sizeof(t)==1 && !has)
    return get_modulehelp(t[0],({modules,0}));
  return t;
}

string *browse_modulehelp(string str,string *modules)
{
  int e,q;
  string *t,*u;
  MYMAP d;
  t=({});
  for(e=0;e<sizeof(modules);e++)
  {
    d=get_help_for_module(modules[e])[1];
    if(!d) continue;
    u=regexp(m_indices(d),str);
    for(q=0;q<sizeof(u);q++)
    {
      u[q]=m_assoc(d,u[q]);
    }
    t+=u;
  }
  return t;
}

int query_prevent_shadow() { return 1; }

static MYMAP foo;

static void rehash_all()
{
  int e;
  hold_on=15;
  foo=EMPTY_MAP;
  D("rehash_all\n");

  rm(BRAINSAVE+"hashsize");

  call_out("do_rehash",1,1);
}

static int rehash_one(string e)
{
  mixed d,q;
  mixed *bar,*gazonk;
  string FOOO;
  MYMAP tmp;

  if(!foo || !hold_on) return 0;
 
  FOOO=BRAINSAVE;
  if(FOOO[0]!='/') FOOO="/"+FOOO;
  sscanf(e,"%s.o",e);
  if(file_size(FOOO+e+".o")>=0)
  {
    D("file found\n");
    res(e);
    D(m_sizeof(savevar)+"\n");
    bar=m_indices(savevar);
    gazonk=m_values(savevar);
    for(d=0;d<sizeof(bar);d++)
    {
      q=hash(bar[d]);
      tmp=m_assoc(foo,q);
      if(!tmp)
	tmp=mkmapping( bar[d..d], gazonk[d..d] );
      else
	tmp=m_assign(tmp, bar[d], gazonk[d]);
      foo=m_assign(foo, q, tmp);
      D("foo now has "+m_sizeof(foo)+" entries\n");
    }
    rm(FOOO+e+".o");
    return sizeof(bar)+8;
  }
  return 2;
}

void do_rehash(int e)
{
  int d,q;
  mixed *bar,*gazonk,*files;
  string FOOO;

  if(!foo || !hold_on) return;
 
  FOOO=BRAINSAVE;
  if(FOOO[0]!='/') FOOO="/"+FOOO;

  call_out("do_rehash2",1,regexp(get_dir(FOOO),"\\.o$"));
}

mixed do_rehash2(string *files)
{
  string f;
  int d,e;
  if(!foo || !hold_on) return;

  d=e=0;
  while(e<100 && d<sizeof(files))
  {
    e+=rehash_one(files[d]);
    d++;
  }
  files=files[d..sizeof(files)-1];
  if(sizeof(files))
    call_out("do_rehash",1,files);
  else
    call_out("do_save",1,1);
}

void do_save()
{
  mixed e;
  int d;
  if(!foo || !hold_on) return;

  D("m_sizeof(foo)="+m_sizeof(foo)+"\n");
  for(d=0; d<20 && m_sizeof(foo); d++)
  {
    e=m_indices(foo)[0];

    savevar=m_assoc(foo,e);
    curr=e;
    sav();

    foo=m_delete(foo, e);
  }
  if(m_sizeof(foo))
  {
    call_out("do_save", 1);
  }else{
    write_file(BRAINSAVE+"hashsize",HASHSIZE+"\n");
    foo=0;
    hold_on=0;
  }
}


void log(string file,string message)
{
  string tp,tp1;
  if(!safe()) return;
  if(this_player())
    tp=(string)call_other(this_player(),REAL_NAME_FUNC);
  else
    tp="no this_player()";

  tp1=(string)call_other(this_interactive()||this_player(),REAL_NAME_FUNC);
  if(tp==tp1)
  {
    message=tp+" "+message;
  }else{
    message=tp1+" ("+tp+") "+message;
  }
  if(time()-m_assoc(date_logged,file)>300)
  {
    message=ctime(time())+"\n"+message;
    date_logged=m_assign(date_logged,file,time());
  }
  write_file(file,message);
}

static string *modules;
string *register_module(string module)
{
  if(!safe()) return ({});
  if(!modules) modules=({});
  if(!stringp(module)) return modules;
  if(module[strlen(module)-1]=='c' && module[strlen(module)-2]=='.')
    module=module[0..strlen(module)-3];
  sscanf(module,"/%s",module);
  sscanf(module,"%s#",module);
  module="/"+module;
  if(-1==member_array(module,modules)) modules+=({module});
  return modules;
}
