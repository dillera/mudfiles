/***********************************************************************\
*		    A shell to protect the innocent			*
*      By Profezzorn, he whom innocents need to be protected from	*
*   _____________________________.oO()Oo.____________________________   *
* This  code  may  be  used in any what way you like providing that you *
* 1) mail me any changes you feel are good.                             *
* 2) mail me the file /log/profezzorn.rep once in a while.              *
*                                             albert_e@lysator.liu.se   *
\***********************************************************************/
#define INHERIT BRAINLIB
#include "brain.h"

/* Define this to get lots of silly debug-messages */
/* #define debug 3 */

string bearer;		/* the real name of the user of this object */
string fnam;		/* filename of this object */
MYMAP ass,aliases;	/* variables & aliases */
MYMAP running;		/* a mapping to stop recursive aliases */
MYMAP nicknames;        /* guess 2 times */
MYMAP flagvalues;	/* some flags */
MYMAP flagtypes;	/* what are they?  */
MYMAP actions;	        /* All actions */
MYMAP help;	        /* All the shellhelp */
string *history_list;	/* The history */
int current;		/* Command number */
object *obj_history;	/* The history of objects */
int current_obj;	/* Object number */
object tagging;		/* The tagged object */
int history_off;	/* record history or not */
status shell_off;	/* Shell responses or not? */
status alias_off;	/* Alias responses or not? */
string this_default;	/* function specific defaults */
string storage;		/* A temporary global string for eval to store in */
int newsread;		/* When did this player read the shellnews last ? */
mixed stack,alt_stack;  /* array of commands and stopped commands */
#ifdef SCAN
object *oldobjects;     /* array with all the objects that already was there */
#endif

#define COM__PREV 0
#define COM__CMD 1
#define COM_DO 0
#define COM_DO_COUNTER 2
#define COM_DO_COMMANDS 3

#define COM_DOTIMES 1
#define COM_DOTIMES_NUM 2
#define COM_DOTIMES_COMMAND 3

#define COM_WAIT 2
#define COM_WAIT_UNTIL 2

#define COM_WALK 3
#define COM_WALK_CMD 2
#define COM_WALK_UNTIL 3

#define COM_SET_NICE 4
#define COM_SET_NICE_FLAG 2

/* Some variables for the 'Make' command */
object makeobj,wheretomove;
string *patches;
int makenum;

int needsave;

status wizard;		/* Is the bearer a wiz? */
int master;		/* Is this the masterobject? */
MYMAP verbmatch,charmatch;
MYMAP modules;

/****** declarations *******/
int do_call(string str,string *data,string com);
varargs mixed parse_string(string str,int type,int start);
mixed handle_quotations(string alfa,int type);
mixed patch2(string a,mixed q,int g);
string fast_desc(mixed a);
string enhance_result(mixed res);
static mixed internal_eval(string str);
static object make_object(string str);
string complete_file_name(string f);
mixed *mid_array(mixed *arr, int a, int b);
string get_string(string str);
void my_destruct(object obj,int rec,int sil);
mixed flagval(string x);
void  flagassign(string x,mixed y);
int _do(string str);
string *expand0(string s);
MYMAP get_map_dir();
int load(string str);
static varargs void internal_add_command(string com,string fun,mixed ob);
void reorganize(int doit);

/***** My own simulating functions *******/
object the_bearer() { return find_player(bearer); }

#if NATIVE
int trusted(object ob)
{
  if(ob==the_bearer())
    return seteuid(geteuid(ob)),1;
  else
    return 0;
}

int check_read_rights(string fil)
{
  if(geteuid()) return 1;
  notify_fail("The shell must be trusted to do that.\n");
  return 0;
}

int check_write_rights(string fil)
{
  if(geteuid()) return 1;
  notify_fail("The shell must be trusted to do that.\n");
  return 0;
}
#else
int check_read_rights(string fil) { return 1; }
int check_write_rights(string fil) { return 1; }

#endif

static int my_command(string a) 
{
  int b;
  if(!a || a=="") return 0;
  a=implode(explode(a," ")," ");
  history_off=time();
#if debug
  write("command:"+a+"\n");
#endif 
  b=INTERNAL_COMMAND(a);
  history_off=0;
#if debug>2
  write("result:"+b+"\n");
#endif 
  return b;
}

void my_mapc(string func,mixed a,mixed b)
{
  int e,c,t;
  t=flagval("muffle");
  if(!pointerp(a)) a=({a});
  c=sizeof(a);
  while(e<c) call_other(this_object(),func,a[e++],b);
  flagassign("muffle",t);
}

/****** Functions for map_array() *******/
mixed preadd(mixed a,mixed b) { return b+a; }
mixed postadd(mixed a,mixed b) { return a+b; }
object my_find_object(string a) { return find_object(a); }
mixed lookup_map(string a,MYMAP d) { return m_assoc(d,a); }

/****** Functions for filter_array() ****/
int no_zero(mixed a) { return !!a; }
int is_object(mixed o) { return objectp(o); }

/****** Primitives for 'assigns' ********/
int unique;

#define unique_assign() ("__UNIQUE"+(++unique)+"_")
#define forget(X) ass=m_delete(ass,X)
#define clean_assigns() ass=my_clean_alist(ass)

void assign(string x,mixed y) { ass=m_assign(ass,x,y); }
mixed get_assign(string a) { return m_assoc(ass,a); }
MYMAP query_variables() { return ass; }

/****** Primitives for 'flags' ********/
mixed flagval(string x) { return m_assoc(flagvalues,x); }
void  flagassign(string x,mixed y) { flagvalues=m_assign(flagvalues,x,y); }

/***** Some object appearance stuff *******/
string query_short()
{ 
  if(!flagval("invis")) return flagval("short");
}

string extra_look() { return flagval("extra_look"); }

string query_long()
{
  return "This is a shell. Use 'shellhelp' for more info.\n"+
	"The shell was created by Profezzorn.\n";
}

#if DISKWORLD
string long(string item,int dark) { return query_long(); }
string short(int dark)
#else
void long(string item) { write(query_long()); }
string short()
#endif
{
#if debug>17
  tell_object(this_player(),"Short: debug is "+debug+"\n");
#endif
  if(bearer && previous_object()!=the_bearer() && iswizard() &&
     previous_object()!=this_object())
  {
    assign("checkob",previous_object());
    assign("checker",this_player());
  }
  return query_short();
}


string query_name() { return flagval("name"); }
int drop() { return 1; }

int get(int silent)
{
#if NO_QUICKTYPER
  if(present("quicktyper",this_player()))
  {
    if(!silent) 
      write("You can not have both a quicktyper and a shell.\n");
    return 0;
  }
#endif
  return 1;
}

int id(string s)
{ return s=="Noedid" || s==query_name() || s==query_short(); }

/***** Get data from another shell *******/

mixed *query_save_data()
{
  string a;
  if(!sscanf(my_file_name(previous_object()),BRAINBASE+"%s",a)) return 0;
  return
    ({
      m_indices(aliases),m_values(aliases),
      m_indices(nicknames),m_values(nicknames),
      m_indices(flagvalues),m_values(flagvalues),
      newsread,m_indices(modules),m_values(modules)
    });
}


string adder;

static void low_link(string str,mixed data)
{
  string str2; /* Milamber, som vill ha sin hj{rna... */
#if debug>2
  write("low_link:"+str+"\n");
#endif
  if(str2=catch(call_other(str,"??")))
  {
    if(wizard)
    {
      write("Error in linking module\n"+str2);
      return;
    }
  }
  sscanf(str2=MODULE+".c","/%s",str2);
  if(!find_object(str) ||
     -1==member_array(str2,inherit_list(find_object(str))))
  {
    write("Not a shell module.\n");
    if(find_object(str) && creator(find_object(str))==bearer)
      write("You must inherit "+MODULE+".\n");
    return;
  }
  if(str->query_clone_module())
  {
    object o;
    str=my_file_name(clone_object(str));
    move_object(o,this_object());
  }
  adder=str;
  if(!data) data=({0});
  modules=m_assign(modules,adder,data);
  if(str2=catch
     ((adder->set_what_shell()),
      (adder->module_init())))
  {
    modules=m_delete(modules,adder);
    write("Error when linking "+adder+": "+str2);
    adder=0;
    return;
  }
  adder=0;
  if(str->query_open_module())
    basepatch("register_module",my_file_name(find_object(str)));
}

static void link_modules(string *mods,mixed *data)
{
  int e;
#if debug>3
  write("link_modules: "+enhance_result(mods)+"\n");
#endif

  if(!data) data=allocate(sizeof(mods));
  for(e=0;e<sizeof(mods);e++)
    low_link(mods[e],data[e]);

  if(sizeof(mods)!=m_sizeof(modules))
    needsave=1;
}

/*****************************************/
string compress_file_name(string a)
{
  string b,c;
  if(!sscanf(a,"/"+WIZDIR+"/%s",b)) return a;
  if(!sscanf(b,"%s/%s",b,c)) return a;
  if(b==bearer) return "~/"+c;
  return "~"+b+"/"+c;
}

#define short_file_name(X) (compress_file_name(my_file_name(X)))

void reset()
{
  string a,b;
  mixed *dum;
#if debug>18
  tell_object(this_player(),"Shell: reset.\n");
#endif
  if(fnam) return;
#if NATIVE
  seteuid(0);
#endif
  fnam=my_file_name(this_object());
  if(sscanf(fnam,"%s#%s",a,b)==2) fnam=a; else master=1;
  if(!isshell(this_object())) { destruct(this_object()); return; }
#ifndef BRAINSHADOW
  pipes=({});
#endif
  if(master) return;
  history_list=allocate(HISTORY_SIZE);
  obj_history=allocate(HISTORY_SIZE);
  alt_stack=stack=0;
  ass=EMPTY_MAP;
  aliases=EMPTY_MAP;
  nicknames=EMPTY_MAP;
  charmatch=EMPTY_MAP;
  verbmatch=EMPTY_MAP;
  modules=EMPTY_MAP;
  running=EMPTY_MAP;
  dum=share("flags",0);
  flagtypes=dum[0];
  flagvalues=copy_mapping(dum[1]);
  history_off=0;
  internal_add_command("$%s","setvar");
  internal_add_command(":%s","override");
#ifdef ECHO
  internal_add_command("'%s","echofnutt");
#endif
}

void dclean(object obj)
{
  int d;
  d=flagval("danger");
  flagassign("danger",6);
  my_destruct(obj,1,1);
  flagassign("danger",d);
}

#define TRY(X) if(q=X) return q

mixed find_it(string what)
{
  object q;
  mixed *d;
  int b,e;
#if debug > 1
  write("find_it: '"+what+"'\n");
#endif
  if(!what || what=="") return 0;
  if(what[strlen(what)-1]=='!') what=my_extract(what,0,strlen(what)-2);
  switch(what)
  {
    case "me": 	  return the_bearer();
    case "env":	  return BEARERS_ENV();
    case "users": return users();
    case "called":
    {
      d=call_out_info();
      for(e=0;e<sizeof(d);e++)
      {
	if(pointerp(d[e]))
	{
	  d[e]=d[e][0];
	}else{
	  d=my_exclude_array(d,e);
	  e--;
	}
      }
      return d;
    }
  }

  switch(what[0])
  {
  case '%':
  {
    string a;
    if(strlen(what)>1) a=extract(what,1);
    if(a=="%" || what=="%") return obj_history[current_obj % HISTORY_SIZE];
    if(a[0]=='%')
    {
      b=current_obj+1-atoi(extract(a,1));
    }else{
      b=atoi(a);
    }
    if(b>0)
    {
      if(current_obj-HISTORY_SIZE<b && b<=current_obj)
	return obj_history[b % HISTORY_SIZE];
      return 0;
    }
    for(b=0;b<HISTORY_SIZE;b++)
    {
      e=(HISTORY_SIZE+current_obj-b) % HISTORY_SIZE;
      if(obj_history[e] && obj_history[e]->id(a)) return obj_history[e];
    }
  }
  case '/':
  case '~':
    return find_object(complete_file_name(what));
  }

  TRY(present(what,this_player()));
  TRY(present(what,BEARERS_ENV()));
  TRY(find_player(what));
  TRY(find_living(what));
  TRY(find_object(complete_file_name(what)));
  return find_object(what);
}

mixed relative_find(string what,mixed a)
{
  int d;
  mixed q,qq,*s;
  string str,dum,dummare,op;
  object o;

#if debug > 1
  write("relative_find("+enhance_result(what)+","+enhance_result(a)+")\n");
#endif
  if(sscanf(what,"%s$$%s",op,dum))
  {
    switch(op)
    {
      case "ffilter":
      case "fmap":
      {
        dummare=get_assign(dum);
        dummare=my_extract(dummare,1,strlen(dummare)-2);
        if(!pointerp(a)) a=({a});
        if(op=="ffilter")
          o=make_object("func(x) { return !!("+dummare+"); }\n");
        else
          o=make_object("func(x) { return "+dummare+"; }\n");

        if(!o) return 0;
        if(op=="ffilter")
          catch(s=filter_array(a,"func",o));
        else
          catch(s=map_array(a,"func",o));
        o->resetglob();
        dclean(o);
        return s;
      }
      case "filter":
      case "map":
      {
        dummare=get_assign(dum);
        dummare=my_extract(dummare,1,strlen(dummare)-2);
        q=get_assign("x");
        if(!pointerp(a)) a=({a});
        if(op=="filter")
	{
	  s=({});
          for(d=0;d<sizeof(a);d++)
          {
            assign("x",a[d]);
            if(parse_string(dummare,-1)) s+=({a[d]});
          }
	}else{
	  s=allocate(sizeof(a));
          for(d=0;d<sizeof(a);d++)
          {
            assign("x",a[d]);
            s[d]=parse_string(dummare,-1);
          }
	}
        assign("x",q);
        return s;
      }
    }
  }
  if(what=="add")
  {
    dummare=get_assign(dum);
    if(!pointerp(a)) a=({a});
    if(!sizeof(a)) return 0;
    q=a[0];
    for(d=1;d<sizeof(a);d++) q+=a[d];
    return q;
  }
  if(pointerp(a))
  {
    s=({});
    for(d=0;d<sizeof(a);d++)
    {
      q=relative_find(what,a[d]);
      if(pointerp(q)) s+=q;
      else s+=({q});
    }
    return s;
  }
  if(sscanf(what,"all$$%s",dum))
  {
    dummare=get_assign(dum);
    dummare=my_extract(dummare,1,strlen(dummare)-2);
    dummare=get_string(dummare);
    s=all_inventory(a);
    q=({});
    for(d=0;d<sizeof(s);d++)
      if(s[d]->id(dummare) || (string)s[d]->short()==dummare) q+=({s[d]});
    return q;
  }
  if(!a || !objectp(a) || !what || what=="") return 0;
  switch(what)
  {
    case "env":	  return ENV(a);
    case "all":	  return all_inventory(a);
    case "allx":  return all_inventory(a)+({a});
    case "dall":  return deep_inventory(a);
    case "dallx": return deep_inventory(a)+({a});
    case "next":  return next_inventory(a);
    case "shadow":  return shadow(a,0);
    case "clones":  return get_all_clones(a);
    case "shadows":
        {
          object *t;
          t=({});
          while(shadow(a,0)) t+=({a=shadow(a,0)});
          return t;
        }
    case "master":
    	{
	  string w,ww,www;
	  w=my_file_name(a);
	  www="";
	  while(sscanf(w,"%s#%s",ww,w)==2) www+=ww;
	  return find_object(www);
	}
  }
  if(sscanf(what,"i%d",d)==1 ||
     (flagval("fizban_mode") && sscanf(what,"%d",d)==1))
  {
    object *b;
    b=all_inventory(a);
    if(d>sizeof(b)-1 || d<0) return 0;
    return b[d];
  }
  if(sscanf(what,"#%d",d)==1)
  {
    if(!sscanf(my_file_name(a),"%s#%s",dum,dummare)) dum=my_file_name(a);
    return find_object(dum+"#"+d);
  }
  if(q=present(what,a)) return q;
  s=all_inventory(a);
  str="%s"+what+"%s";
  for(d=0;d<sizeof(s);d++)
    if(sscanf(" "+my_file_name(s[d])+" ",str,dum,dummare)==2) return s[d];
}

mixed *deal_pars(string str,int found,int e,int type)
{
  string dum,a;
  int c;
#if debug>2
  write("deal_pars("+str+","+found+","+e+","+type+")\n");
#endif

  if(!found && e==strlen(str)-1)
    return ({handle_quotations(str,type)});
  dum=unique_assign();
  assign(dum,my_extract(str,found,e));
  a=my_extract(str,0,found-1)+"$$"+dum;
  c=strlen(a);
  a+=my_extract(str,e+1,strlen(str)-1);
  return ({a,c,dum});
}

varargs mixed parse_string(string str,int type,int start)
{
  mixed b,z,x;
  string *c,q,w,r;
  int e,typ;
  object a;
  int parlvl,quote,found;
  string *dum;
  
#if debug > 1
  write("parse_string : "+enhance_result(str)+","+type+","+start+"\n");
#endif
  PROFILE("parse_string");
  if(stringp(str))
  {
    c=explode(str," ");
    if(pointerp(c))
      str=implode(c," ");
    else
      str="";
  }
  switch(start)
  {
  default:
  case 0:
#if debug>9
    write("case 0:\n");
#endif
    if(str=="$x") return get_assign("x");
    if(!str) if(this_default) str=this_default; else str="$default";
    this_default=0;
    if(str=="") return 0;
    if(strlen(str)==3 && str[0]=='\'' && str[2]=='\'') return str[2];

    if(sscanf(str,"%s(%s)%s",b,q,w)==3 ||
       sscanf(str,"%s\"%s\"%s",b,q,w)==3 ||
       sscanf(str,"%s[%s]%s",b,q,w)==3 ||
       sscanf(str,"%s\"%s",q,w))
    {
      dum=({});
      found=-1;
      for(;e<strlen(str);e++)
      {
	switch(str[e])
	{
	case '"':
	  if(!parlvl)
	  {
	    found=e;
	    e=quotesearch(str,e+1);
	    b=deal_pars(str,found,e,type);
	    if(sizeof(b)==1) return b[0];
	    found=-1;
	    str=b[0];
	    e=b[1]-1;
	    dum+=({b[2]});
	    typ=1;
	  }else
	    e=quotesearch(str,e+1);
	  break;
	case '(':
	case '[':
	  if((parlvl+=1)==1 && found<0) found=e;
	  break;
	case ')':
	case ']':
	  if(!(parlvl-=1))
	  {
	    b=deal_pars(str,found,e,type);
	    if(sizeof(b)==1) return b[0];
	    found=-1;
	    str=b[0];
	    e=b[1]-1;
	    dum+=({b[2]});
	    typ=1;
	  }
	}
      }
      if(typ) b=parse_string(str,type,2);
      for(e=0;e<sizeof(dum);e++) forget(dum[e]);
      if(typ) return b;
    }

  case 2:
#if debug>9
    write("case 2:\n");
#endif
#if mappings
    if(iswizard() && (type & (T_POINTER | T_MAPPING)) &&
       my_extract(str,0,3)=="####")
    {
      if(my_extract(str,4,4)!="#")
#else
	if(iswizard() && type & T_POINTER && my_extract(str,0,3)=="####")
#endif
	{
	  str=my_extract(str,4,strlen(str)-1);
	  if(!strlen(str)) return ({});
	  if(str[strlen(str)-1]==',')
	    c=explode(str,",");
	  else
	    c=my_explode(str,",");
	  b=allocate(sizeof(c));
	  for(e=0;e<sizeof(c);e++) b[e]=parse_string(c[e],-1,2);
	  return b;
#if mappings
	}else{
	  str=my_extract(str,5,strlen(str)-1);
	  if(!strlen(str)) return ([]);
	  if(str[strlen(str)-1]==',')
	    c=explode(str,",");
	  else
	    c=my_explode(str,",");
	  b=allocate(sizeof(c));
	  for(e=0;e<sizeof(c);e++) 
	  {
	    sscanf(c[e],"%s:%s",q,w);
	    b[e]=parse_string(q,-1);
	    c[e]=(string)parse_string(w,-1,2);
	  }
	  return mkmapping(b,c);
	}
#endif
    }

  case 5:
#if debug>9
    write("case 5:\n");
#endif
    if(type & T_POINTER && 2==sscanf(str,"%s..%s",q,w))
      return ({parse_string(q,-1,5),parse_string(w,-1,5)});
    if(sscanf(str+"@","%s?%s:%s@",q,w,r)==3)
    {
      if(parse_string(q,-1,5)) return parse_string(r,type,5);
      else return parse_string(w,type,5);
    }
    if(type & T_NUMBER && 2==sscanf(str,"%s==%s",q,w))
      return (parse_string(q,-1,5) == parse_string(w,-1,5));
    if(type & T_NUMBER && 2==sscanf(str,"%s&&%s",q,w))
      return (parse_string(q,T_NUMBER,5) && parse_string(w,T_NUMBER,5));
    if(type & T_NUMBER && 2==sscanf(str,"%s||%s",q,w))
      return (parse_string(q,T_NUMBER,5) || parse_string(w,T_NUMBER,5));

    str=replace(str,"->",";;");
    if(type & T_NUMBER && 2==sscanf(str,"%s<%s",q,w))
      return (parse_string(q,T_NUMBER,5) < parse_string(w,T_NUMBER,5));
    if(type & T_NUMBER && 2==sscanf(str,"%s>%s",q,w))
      return (parse_string(q,T_NUMBER,5) > parse_string(w,T_NUMBER,5));
    str=replace(str,"%-","%%");

    if(type & (T_NUMBER | T_MAPPING | T_POINTER))
    {
      str=replace(str,"-","+-");
      c=my_explode(str,"+");
      if(sizeof(c)>1)
      {
	if(type & T_STRING) typ=type | T_NUMBER; else typ=type;
	if(!strlen(c[0])) c=mid_array(c,1,sizeof(c)-1);
	if(c[0][0]=='-')
	  b=-parse_string(my_extract(c[0],1,strlen(c[0])-1),typ,10);
	else
	  b=parse_string(c[0],typ,10);
#if debug>20
	write("b eq "+enhance_result(b)+"\n");
#endif
	for(e=1;e<sizeof(c);e++)
	{
	  if(c[e][0]=='-')
	    b-=parse_string(my_extract(c[e],1,strlen(c[e])-1),typ,10);
	  else
	    b+=parse_string(c[e],typ,10);
	}
	return b;
      }
    }
    if(type & T_STRING)
    {
      c=my_explode(str,"+");
      if(sizeof(c)>1)
      {
	for(e=1,b="";e<sizeof(c);e++)
	  b+=parse_string(c[e],T_STRING | T_NUMBER,10);
#if debug>15
	write("Returning "+enhance_result(b)+"\n");
#endif
	return b;
      }
    }

  case 10:
#if debug>9
    write("case 10:\n");
#endif
    if((type & T_NUMBER) && str[0]!='/' && 
       (sscanf(str,"%s/%s",q,w) || sscanf(str,"%s*%s",q,w)))
    {
      q=replace(str,"/","*/");
      c=explode(q,"*");
      b=parse_string(c[0],T_NUMBER,20);
      if(intp(b))
      {
	for(e=1;e<sizeof(c);e++)
	{
	  if(c[e][0]=='/')
	    b/=parse_string(my_extract(c[e],1,strlen(c[e])-1),T_NUMBER,20);
	  else b*=parse_string(c[e],T_NUMBER,20);
	}
#if debug>15
	write("Returning "+enhance_result(b)+"\n");
#endif
	return b;
      }
    }

  case 20:
#if debug>9
    write("case 20:\n");
#endif
    if(sscanf(str,"!%s",q)) return !parse_string(q,-1,20);
    if(iswizard())
    {
      if(sscanf(str+"@","%s;;%s$$%s@",q,w,r)==3)
      {
	r=(string)get_assign(r);
	r="####"+my_extract(r,1,strlen(r)-2);
	return patch2(q,({parse_string(w,T_OBJECT | T_POINTER | T_STRING,20)})
		      + parse_string(r,T_POINTER) ,0);
      }
    }

  case 30:
#if debug>9
    write("case 30:\n");
#endif
    if(iswizard())
    {
      c=my_explode(str,".");
      if(sizeof(c)>1)
      {
	a=parse_string(c[0],T_OBJECT | T_POINTER,30);
	for(e=1;e<sizeof(c);e++)
	{
	  a=relative_find(c[e],a);
	  if(a && objectp(a)) book(a);
	}
	return a;
      }

      if(flagval("fizban_mode"))
      {
	c=my_explode(str,",");
	if(sizeof(c)>1)
	{
	  a=parse_string(c[0],T_OBJECT | T_POINTER,30);
	  for(e=1;e<sizeof(c);e++)
	  {
	    a=relative_find(c[e],a);
	    if(a && objectp(a)) book(a);
	  }
	  return a;
	}
      }

      x=explode(str,"$$");
      if(sizeof(x)>1)
      {
	q=implode(x[0..sizeof(x)-2],"$$");
	w=x[-1];

	if(q=="")
	{
	  if(2==sscanf(w,"%s$$%s",q,r))
	  {
	    q="$$"+q;
	    w=r;
	  }
	  else
	    q="";
	}
	w=get_assign(w);
	if(stringp(w))
	{
	  if(w[0]=='[' && w[strlen(w)-1]==']')
	  {
	    b=parse_string(my_extract(w,1,strlen(w)-2),-1);
	    c=parse_string(q,-1,20);
#if mappings
	    if(mappingp(c))
	      return m_assoc(c,b);
#endif
	    if(intp(b)) return c[b];
	    if(pointerp(b) && sizeof(b)==2)
	    {
	      if(stringp(b)) z=strlen(b);
	      else if(pointerp(b)) z=sizeof(b);
	      else z=0;
	      return c[ (b[0]<0)? (z-b[0]) : b[0]..b[1]<0?z-b[1]:b[1] ];
	    }
	  }
	  if(w[0]=='(' && w[strlen(w)-1]==')')
	  {
	    b=my_extract(w,1,strlen(w)-2);
	    switch(q)
	    {
            case "eval":
              return internal_eval("func() { return "+b+";}");
            case "OBJ":
              return parse_string(b,T_OBJECT,30);
	    case "popen":
	      return popen("my_command",b,0,0,0);
	    case "size":
	    {
	      b=parse_string(b,-1);
	      if(stringp(b)) return strlen(b);
	      if(pointerp(b)) return sizeof(b);
#if mappings
	      if(mappingp(b)) return m_sizeof(b);
#endif
	      return 0;
	    }
#if mappings
	    case "m_indices":
	      return m_indices(parse_string(b,T_MAPPING));
	    case "m_values":
	      return m_values(parse_string(b,T_MAPPING));
#endif
	      
            case "get_files":
            case "get_loaded":
	      c=expand0(get_string(b));
              if(q=="get_loaded")
              {
                c=map_array(c,"my_find_object",this_object());
                c=filter_array(c,"no_zero",this_object());
              }
              return c;
            }
	  }
	}
      }
      if(type & T_OBJECT && (a=find_it(str)))
      {
	book(a);
	return a;
      }
    }
    if((type & T_NUMBER) && sscanf(str,"rnd$$%s",b))
    {
      if(intp(b=parse_string("$$"+b,T_NUMBER)))
	return random(b);
      else
      {
	write("rnd() must have integer argument.\n");
	return 0;
      }
    }else if((type & T_NUMBER) && sscanf(str,"holding$$%s",b))
    {
      if(stringp(b=parse_string("$$"+b,T_STRING)))
	return !!present(b,the_bearer());
      else
      {
	write("holding() must have string argument.\n");
	return 0;
      }
    }
    if(type & T_STATUS)
    {
      if(str=="t" || str=="on" || str=="true") return 1;
      if(str=="nil" || str=="off" || str=="false") return 0;
    }
    if(type & (T_NUMBER | T_STATUS) && sscanf(str+"@","%d@",b)) return b;
    if(my_extract(str,0,1)=="$$")
    {
      return handle_quotations((string)get_assign(my_extract(str,2,strlen(str)-1)),type);
    }
    if(str[0]=='$') return get_assign(my_extract(str,1,strlen(str)-1));
    if(iswizard())
      return replace(str,"\\n","\n");
    else
      return str;
  }
}


mixed handle_quotations(string alfa,int type)
{
  string beta,gamma;

#if debug
  write("handle_qoute:"+alfa+","+type+"\n");
#endif
  if(alfa[0]=='\"' && alfa[strlen(alfa)-1]=='\"')
    return stringmake(my_extract(alfa,1,strlen(alfa)-1))[1];

  if(alfa[0]=='(' && alfa[strlen(alfa)-1]==')')
  {
    beta=my_extract(alfa,1,strlen(alfa)-2);
    if(!strlen(beta)) return beta;
    if(iswizard() && beta[0]=='{' && beta[strlen(beta)-1]=='}')
      return parse_string("####"+my_extract(beta,1,strlen(beta)-2),T_POINTER);
#if mappings
    if(stringp(beta) && beta[0]=='[' && beta[strlen(beta)-1]==']')
      return parse_string("#####"+my_extract(beta,1,strlen(beta)-2),T_MAPPING);
#endif
    return parse_string(beta,type);
  }else if(alfa[0]=='[' && alfa[strlen(alfa)-1]==']')
  {
    beta=my_extract(alfa,1,strlen(alfa)-2);
  }
  return parse_string(alfa,type);
}

varargs object *get_arrofob(string str,mixed def)
{
  mixed o,o2,o3;
  int e;
  if(def) this_default=def;
  o2=my_explode_on_char(str,' ');
  o3=({});
  for(e=0;e<sizeof(o2);e++)
  {
    if(o2[e][strlen(o2[e])-2..strlen(o2[e])-1]==".c")
      o2[e]=o2[e][0..strlen(o2[e])-3];

    o=parse_string(o2[e], T_OBJECT | T_POINTER );
    if(pointerp(o)) o=filter_array(o,"is_object",this_object());
    else if(objectp(o)) o=({o});
    else continue;
    o3+=o;
  }
  return o3;
}

varargs mixed get_oborarr(string str,mixed def)
{
  mixed o;
  if(def) this_default=def;
  o=parse_string(str, T_OBJECT | T_POINTER );
  if(!pointerp(o) && !objectp(o)) return 0;
  return o;
}

varargs object get_ob(string str,mixed def)
{
  mixed o;
  if(def) this_default=def;
  o=parse_string(str, T_OBJECT);
  if(!objectp(o)) return (object)0;
  return (object)o;
}

varargs int get_int(string str,mixed def)
{
  mixed o;
  if(def) this_default=def;
  o=parse_string(str, T_NUMBER);
  if(!intp(o)) return (int)0;
  return (int)o;
}

string fast_desc(mixed a)
{
  string s,t,b;
  if(intp(a)) return a+"";
  if(objectp(a))
  {
    if(flagval("verbatim")) return short_file_name(a);
    if(!living(a)) return short_file_name(a);
    if(interactive(a)) return (string)call_other(a,REAL_NAME_FUNC)+"!";
    if(function_exists(REAL_NAME_FUNC,a)) 
      s=(string)call_other(a,REAL_NAME_FUNC);
    else if(function_exists("query_name",a)) s=NAME(a);
    if(!s) return short_file_name(a);
    s+="!";
    if(sscanf(my_file_name(a),"%s#%s",t,b)==2) s+=".#"+b;
    return s;
  }
  if(stringp(a))
  {
    int qq;
    qq=flagval("columns")-27;
    a=replace(a,"\n","\\n");
    if(strlen(a)<qq) return "\""+a+"\"";
    return "\""+extract(a,0,qq)+">";
  }
#if mappings
  if(mappingp(a)) return "mapping["+m_sizeof(a)+"]";
#endif
  if(pointerp(a)) return "array["+sizeof(a)+"]";
  return "unknown type";
}

mixed *printing;

void print_enhance_result2(mixed res)
{
  string t,*k;
  int e,sz;
  mixed c,*k2,tt;

  if(stringp(res))
  {
    write("\""+replace(res,"\n","\\n")+"\"");
    return;
  }
  if(intp(res)) return write(res+"");
  if(objectp(res)) return write(fast_desc(res));

  if(-1!=(tt=member_array(res,printing)))
  {
    write("@"+tt);
    return;
  }
#if mappings
  if(mappingp(res))
  {
    if(!(sz=sizeof(k=m_indices(res)))) return write("([])");
    k2=m_values(res);
    e=0;
    write("([");
    printing+=({res});
    while(e<sz)
    {
      print_enhance_result2(k[e]);
      write(":");
      print_enhance_result2(k2[e]);
      if(++e<sz) write(",");
    }
    printing=mid_array(printing,0,sizeof(printing)-2);
    write("])");
    return;
  }
#endif
  if(!(sz=sizeof(res))) return write("({})");
  printing+=({res});
  write("({");
  e=0;
  while(e<sz)
  {
    print_enhance_result2(res[e]);
    if(++e<sz) write(",");
  }
  printing=mid_array(printing,0,sizeof(printing)-2);
  write("})");
}

void prettyprint(mixed foo,int in)
{
  int e;
  mixed *a,*b;
  if(pointerp(foo))
  {
    if(!sizeof(foo))
    {
      write("({})");
      return;
    }
    write("({\n");
    in+=2;
    for(e=0;e<sizeof(foo);e++)
    {
      write("                                                   "[0..in]);
      prettyprint(foo[e],in);
      write(",\n");
    }
    in-=2;
    write("                                                   "[0..in]);
    write("})");
    return;
  }
#if mappings
  if(mappingp(foo))
  {
    if(!m_sizeof(foo))
    {
      write("([])");
      return;
    }
    a=m_indices(foo);
    b=m_values(foo);
    write("([\n");
    in+=2;
    for(e=0;e<sizeof(a);e++)
    {
      write("                                                   "[0..in]);
      write(fast_desc(a[e])+":");
      prettyprint(b[e],in);
      write(",\n");
    }
    in-=2;
    write("                                                   "[0..in]);
    write("])");
    return;
  }
#endif
  write(fast_desc(foo));
}

string enhance_result(mixed res)
{ 
  return popen("print_enhance_result",res,0,0,0);
}

void print_enhance_result(mixed res)
{
  printing=({});
  print_enhance_result2(res);
}

string antiparse(mixed a)
{
  if(intp(a)) return a+"";
  if(stringp(a)) return a;
  if(objectp(a)) return my_file_name(a);
  if(pointerp(a)) return enhance_result(a);
  return a;
}

string reparse(string s) { return antiparse(parse_string(s,-1)); }

string get_string(string str)
{
  mixed q;
  if(str=="") return "";
  q=parse_string(str,T_STRING | T_NUMBER);
  return antiparse(q);
}

int test_destruct(object obj)
{
  string b,d;
  if(!objectp(obj)) return 0;
  b=my_file_name(obj);
  switch(flagval("danger"))
  {
  default: return 0;
  case 76..100: if(living(obj)) return 0;
  case 50..75: if(sscanf(b,"/room/%s",d)) return 0;
  case 40..49: if(sscanf(b,"/obj/soul#%s",d)) return 0;
  case 35..39: if(sscanf(b,"/obj/door%s",d)) return 0;
  case 30..34: if(sscanf(b,"/%s/castle%s",d,d) && d=="") return 0;
  case 20..29: if(obj==this_object()) return 0;
  case 10..19: if(interactive(obj)) return 0;
  case 5..9: if(obj==the_bearer()) return 0;
  case 0..4: return 1;
  }
}

void my_destruct(object obj,int rec,int sil)
{
  object a,b;
  string d;
  if(!test_destruct(obj))
  {
    if(!sil) write("Danger: "+fast_desc(obj)+" not destructed.\n");
    return;
  }
  if(rec)
  {
    my_mapc("my_destruct",all_inventory(obj),rec);
  }

#ifdef UNDOLOG
  basepatch("log",UNDOLOG,"Undo "+fast_desc(obj)+".\n");
#endif

  a=ENV(obj);
  b=BEARERS_ENV();
  if(!sil)
  {
    if(a && (a==b || a==the_bearer()) && (d=(string)obj->short()))
    {
      my_say(this_player()->query_name()+" zaps "+d+" with a flash of green fire.\n");
    }else{
      my_say(this_player()->query_name()+" makes some magical gestures.\n");
    }
    flagassign("muffle",1);
  }
  d=fast_desc(obj);
#if REMOVE_FUNC
  obj->REMOVE_FUNC();
  if(obj)
  {
    if(!sil) write(d+" tried not to be destructed\n");
    if(obj && flagval("danger")<3) destruct(obj);
  }
#else
  if(obj) destruct(obj);
#endif
  if(!sil)
    if(!obj) my_write("Disposed: "+d+"\n");
    else my_write("Failed to destruct: "+d+"\n");
}

/**************************************************/

static int initialize()
{
  string q;
  object a;
  mixed p;
  int e;
  bearer=(string)call_other(environment(),REAL_NAME_FUNC);
#ifdef alt_name
  if(alt_name!=fnam && my_creator(this_object())!=bearer &&
     !NO_ALT_SHELL(this_player()))
  {
#if NATIVE
    seteuid(getuid());
#endif
    q=catch(move_object(clone_object(alt_name),environment()));
#if NATIVE
    seteuid(0);
#endif
    if(q) write(q);
    if(this_object()) destruct(this_object());
    return 0;
  }
#endif
  wizard=WIZARD_P(this_player());
  p=share("flags",wizard);
  flagtypes=p[0];
  flagvalues=copy_mapping(p[1]);

#ifdef BRAINSAVE
  p=0;
  catch(p=basepatch("restore",bearer,0));
  if(p)
  {
    if(intp(p))
    {
      switch(get_assign("hold_on") % 15)
      {
      case 0:
	write("Please wait while loading aliases.\n");
	break;

      case 1:
	write("The thing in the shell taps it's foot impatiently.\n");
	break;

      case 2:
	write("The shell plays some pause music.\n");
	break;

      case 3:
	write("300 shell workers starts a strike because they want higher salaries.\n");
	break;

      case 4:
	write("Profezzorn takes the workers aside and whisper something to their leader.\n");
	break;

      case 5:
	write("The shell workers goes back to work somewhat pale-faced.\n");
	break;

      case 6:
	write("Profezzorn grins evilly and excuses for the delay.\n");
	break;

      case 7:
	write("The shell suddenly humms loudly.\n");
	break;

      case 8:
	write("The thing in the shell tells you: Did you take the aliases?\n");
	break;

      case 9:
	write("Game driver tells you: The shell will soon have it's aliases.\n");
	break;

      case 10:
	write("You wonder if somone is putting you on.\n");
	break;

      case 11:
	write("Someone grins evilly.\n");
	break;

      case 12:
	write("The shell starts looking for it's aliases.\n");
	break;

      case 13:
	write("The thing in the shell tells you: I've found them!\n");
	break;

      case 14:
	write("The thing in the shell tells you: Sorry, wrong ones..\n");
	break;

      }
      call_out("reorganize",p,17);
      bearer=0;
      assign("hold_on",get_assign("hold_on")+1);
      return 1;
    }
    if(get_assign("hold_on"))
    {
      write("Your aliases has now been loaded.\n");
    }

    for(e=0;e<sizeof(p[0]);e++)
    {
      if(!p[0][e] || !p[1][e])
      {
	p[0]=p[0][0..e-1]+p[0][e+1..100000];
	p[1]=p[1][0..e-1]+p[1][e+1..100000];
	e--;
	needsave=1;
      }
    }

    aliases=mkmapping(p[0],p[1]);

    nicknames=mkmapping(p[2],p[3]);
    nicknames=my_clean_alist(nicknames);
    for(e=0;e<sizeof(p[4]);e++)
      if(wizard || !(WIZFLAG & m_assoc(flagtypes,p[4][e])))
	flagassign(p[4][e],p[5][e]);

    if(flagval("danger")<25) flagassign("danger",25);
    flagassign("muffle",0);
    flagassign("light",0);
    flagassign("shell",1);
    if(flagval("fizban_mode"))
      write("Huba!\n");
#ifdef SCAN
    if(flagval("invscan"))
      oldobjects=all_inventory(the_bearer());
#endif
#ifdef PEOP
    if(flagval("waitforall")) basepatch("setwaitforall",this_object());
#endif
    newsread=p[6];
  }
  if(wizard)
  {
    if(p && sizeof(p)>7)
    {
      link_modules(p[7],sizeof(p) > 8 ? p[8] : 0 );
    }else{
      link_modules(DEFAULT_WIZMODULES,0);
    }
  }
  call_out("checknews",1);
#endif

  if(flagval("short")=="a shell" && strlen(bearer))
    flagassign("short",capitalize(
	  bearer+(bearer[strlen(bearer)-1]=='s'?"'":"'s")+" shell"));
      
  if(m_assoc(aliases,".login"))
  {
    stack=({stack,COM_DO,0,({".login"})});
    call_out("ticker",1);
  }
  actions=share("actions",wizard);

#if NATIVE
  if(getuid()==bearer) seteuid(getuid());
  else if(wizard) write("Don't forget to 'trust' the shell.\n");
#endif
  if(a=present("Noedid 2",environment()))
    destruct(a);
  return 0;
}

void init()
{
  if(environment()==this_player() && interactive(this_player()))
  {
    PROFILE("init()");
    if((string)call_other(this_player(),REAL_NAME_FUNC)!=bearer)
    {
      if(bearer) return;
      if(initialize()) return;
    }
    if(flagval("shell"))
    {
      add_action("do_it","",1);
      remove_call_out("reorganize");
      call_out("reorganize",60+random(5));
    }else{
      add_action("shellstatus","shellstatus",1);
    }
#ifdef BUGD
    BRAINBASE->do_error_check();
#endif
  }
}

static string *expand2(string p,string *pp)
{
  int e;
  string a,*res,*res2;
#if debug>2
  write("expand2("+p+","+enhance_result(pp)+"\n");
#endif
  if(!sizeof(pp)) return ({p});
  p+="/";
  if(!sscanf(pp[0],"%s*%s",a,a)) return expand2(p+pp[0],mid_array(pp,1,sizeof(pp)-1));
  res=get_dir(p+pp[0]);
  if(!pointerp(res)) return ({});
  res=map_array(res-({"..","."}),"preadd",this_object(),p);
  pp=mid_array(pp,1,sizeof(pp)-1);
  if(!sizeof(pp)) return res;
  for(res2=({}),e=0;e<sizeof(res);e++)
    if(file_size(res[e])==-2)
      res2+=expand2(res[e],pp);
  return res2;
}

static string *expand1(string s)
{
  string t;
#if debug>1
  write("expand1("+s+")\n");
#endif
  s=complete_file_name(s);
  sscanf(s,"/%s",s);
  if(!check_read_rights(s)) return ({});
#if debug>5
  write("expand1!!("+s+")\n");
#endif
  return expand2("",my_explode(s,"/"));
}

static string *expand0(string s)
{
  string *dum,*p;
  int e;
#if debug>1
  write("expand0("+s+")\n");
#endif
  p=({});
  if(s)
  {
    dum=my_explode_on_char(s,' ');
    for(e=0;e<sizeof(dum);e++) p+=expand1(dum[e]);
  }
  return p;
}

string *expand00(string str)
{
  string *dum,a,b;
  if(str=="") str=complete_file_name("");
  if(str[strlen(str)-1]=='/') str+="*";
  dum=expand0(str);
  if(sizeof(dum)) return dum;
  if(!sscanf(str,"%s*%s",a,b))
  {
    if(str[strlen(str)-1]!='/') str+="/*";
    else str+="*";
  }
  return expand0(str);
}

int expand(string s)
{
  if(SAFECHECK()) return 0;
  printarrayofstrings(expand0(s),0,flagval("columns"));
  return 1;
}

#define EGREP_NUMBERS 1
#define EGREP_NO_FILENAME 2
#define EGREP_IGNORE_CASE 3
#define EGREP_FILES_ONLY 4
#define EGREP_INVERT 5

int egrep(string str)
{
  string *a,b;
  mixed *foo;
  if(SAFECHECK()) return 0;
  foo=get_flags(str,"nhilv");
  str=foo[0];
  if(!str || sizeof(a=my_explode_on_char(str,' '))<2)
    endwrite("Usage: egrep <match> <file(s)>\n");
  str=implode(a[1..1000]," ");
  b=get_string(a[0]);
  my_mapc("egrep2",expand0(str),({ b,foo }));
  return 1;
}

static void egrep2(string f,mixed *flags)
{
  string a,ff;
  int e;
  string str;

  str=flags[0];
  flags=flags[1];
  if(!check_read_rights(f)) return;
  if(file_size(f)<0) return;

  if(flags[EGREP_NO_FILENAME])
    ff="";
  else
    ff=basename(f)+":";

  if(flags[EGREP_IGNORE_CASE])
    str=lower_case(str);

  for(e=1;a=read_file(f,e,100);e+=100)
  {
    if(flags[EGREP_IGNORE_CASE])
      a=lower_case(a);

    if(flags[EGREP_FILES_ONLY])
    {
      if(sizeof(regexp(my_explode(a,"\n"),str)))
      {
	if(!flags[EGREP_INVERT])
	  write(basename(f)+"\n");
	return;
      }
    }
    else if(flags[EGREP_NUMBERS] || flags[EGREP_INVERT])
    {
      string *p;
      int q;
      p=my_explode(a,"\n");
      for(q=0;q<sizeof(p);q++)
      {
	if(sizeof(regexp(p[q..q],str)) ^ flags[EGREP_INVERT])
	{
	  if(flags[EGREP_NUMBERS])
	    write(ff+(e+q)+":"+p[q]+"\n");
	  else
	    write(ff+p[q]+"\n");
	}
      }
    }
    else
    {
      a=implode(regexp(my_explode(a,"\n"),str),"\n"+ff);
      if(a!="") write(ff+a+"\n");
    }
  }
  if(flags[EGREP_INVERT] && flags[EGREP_FILES_ONLY])
  {
    write(basename(f)+"\n");
  }
}

#ifdef AUTO
int auto(string str)
{
  object *o;
  int e;
  if(!str) return notify_fail("Usage: auto <command>\n"),0;
  str=replace(str,"auto","");
  o=all_inventory(BEARERS_ENV());
  for(e=0;e<sizeof(o);e++)
  {
    if(((living(o[e]) && o[e]->query_npc()) || interactive(o[e])) &&
       o[e]!=this_player() && o[e]->short())
    {
      if(!this_player()->query_spell_points())
      {
	write("Not enough Spellpoints\n");
	return 1;
      }
      this_player()->restore_spell_points(-1);
      my_command(str+" "+o[e]->query_name());
    }
  }
  return 1;
}
#endif

void make_it(string str,int nono)
{
  mixed q;
  if(str=="**")
  {
    write("Make aborted.\n");
    destruct(makeobj);
    return;
  }
  if(makenum>-1 && makenum<sizeof(patches))
  {
    if(!str || str=="") q=patches[makenum][1];
    else q=parse_string(str,patches[makenum][3]);
    call_other(makeobj,patches[makenum][2],q);
  }else{
    if(!str || str=="")
    {
      write("Ready.\n");
      move_object(makeobj,wheretomove);
      return;
    }else{
      q=my_explode_on_char(str,' ');
      if(sizeof(q)<2) q+=({ 0 });
      else q[1]=parse_string(q[1],-1);
      call_other(makeobj,q[0],q[1]);
    }
  }
  makenum+=1;
  if(!nono)
  {
    if(makenum<sizeof(patches))
    {
      write(patches[makenum][0]+" (");
      print_enhance_result(patches[makenum][1]);
      write(") :");
    }else{
      write("Anything else? (func arg):");
    }
  }
  if(!nono) input_to("make_it");
}

int make(string str)
{
  string *a,b;
  int e;

  if(SAFECHECK()) return 0;
  notify_fail("Make what?\n");
  if(!str || str=="") return 0;
  a=my_explode_on_char(str,' ');
  if(!sizeof(a))
    endwrite("Usage: Make <what> [arguments]\n");

  b=a[0];
  a=a[1..sizeof(a)-1];

  switch(b)
  {
    case "thing":
    {
      makeobj=clone_object(TREASUREFILE);
      patches=({
	({ "name","thing","set_name",T_STRING }),
	({ "short","a thing","set_short",T_STRING }),
	({ "long","It's a thing.\n","set_long",T_STRING }),
	({ "value",0,"set_value",T_NUMBER }),
	({ "weight",1,"set_weight",T_NUMBER }),
      });
      wheretomove=the_bearer();
      break;
    }

    case "food":
    {
      makeobj=clone_object(FOODFILE);
      patches=({
	({ "name","food","set_name",T_STRING }),
	({ "short","some food","set_short",T_STRING }),
	({ "long","It's a thing.\n","set_long",T_STRING }),
	({ "value",0,"set_value",T_NUMBER }),
	({ "weight",1,"set_weight",T_NUMBER }),
	({ "strength",2,"set_strength",T_NUMBER }),
	({ "heal",5,"set_heal",T_NUMBER }),
      });
      wheretomove=the_bearer();
      break;
    }

    case "drink":
    {
      makeobj=clone_object(DRINKFILE);
      patches=({
	({ "name","beer","set_name",T_STRING }),
	({ "short","a beer","set_short",T_STRING }),
	({ "long","It's a thing.\n","set_long",T_STRING }),
	({ "value",0,"set_value",T_NUMBER }),
	({ "weight",1,"set_weight",T_NUMBER }),
	({ "strength",2,"set_strength",T_NUMBER }),
	({ "heal",5,"set_heal",T_NUMBER }),
      });
      wheretomove=the_bearer();
      break;
    }

    case "armour":
    {
      makeobj=clone_object(ARMOURFILE);
      patches=({
	({ "name","armour","set_name",T_STRING }),
	({ "short","an armour","set_short",T_STRING }),
	({ "long","It's a shining armour.\n","set_long",T_STRING }),
	({ "type","armour","set_type",T_STRING | T_NUMBER }),
	({ "value",0,"set_value",T_NUMBER }),
	({ "class",3,"set_class",T_NUMBER }),
	({ "weight",1,"set_weight",T_NUMBER }),
      });
      wheretomove=the_bearer();
      break;
    }
    case "weapon":
    {
      makeobj=clone_object(WEAPONFILE);
      patches=({
	({ "name","sword","set_name",T_STRING }),
	({ "short","a sword","set_short",T_STRING }),
	({ "long","It's a shining sword.\n","set_long",T_STRING }),
	({ "value",0,"set_value",T_NUMBER }),
	({ "class",6,"set_class",T_NUMBER }),
	({ "weight",1,"set_weight",T_NUMBER }),
      });
      wheretomove=the_bearer();
      break;
    }
    case "monster":
    {
      makeobj=clone_object(MONSTERFILE);
      patches=({
	({ "name","orc","set_name",T_STRING }),
	({ "alias",0,"alias",T_STRING }),
	({ "short","an orc","set_short",T_STRING }),
	({ "long","It's a small and confused orc.\n","set_long",T_STRING }),
	({ "gender",1,"set_gender",T_NUMBER }),
	({ "wimpy",0,"set_whimpy",T_NUMBER }),
	({ "level",6,"set_level",T_NUMBER }),
	({ "weapon class",6,"set_wc",T_NUMBER }),
	({ "armour class",3,"set_ac",T_NUMBER }),
      });
      wheretomove=BEARERS_ENV();
      break;
    }
    case "generic":
    {
      if(sizeof(a)<2)
	endwrite("Must know what to make!\n");
      makeobj=clone_object(complete_file_name(a[1]));
      wheretomove=the_bearer();
      patches=({});
      a=a[1..sizeof(a)-1];
    }
    default:
      return 0;
  }
  makenum=0;
  for(e=0;e<sizeof(a) && e<sizeof(patches);e++) make_it(a[e],1);
  for(;e<sizeof(a);e+=2) make_it(a[e]+" "+a[e+1],1);
  if(e<sizeof(patches))
  {
    write("Use '**' to abandon.\n"+patches[makenum][0]+" (");
    print_enhance_result(patches[makenum][1]);
    write(") :");
    input_to("make_it");
  }else{
    make_it("",1);
  }
  return 1;
}
#ifdef RUSAGE
string rtime(int e)
{
  string b;
  b=("0000"+(e%1000));
  return e/1000+"."+b[strlen(b)-3..strlen(b)-1];
}

#if RUSAGE == T_POINTER
int *my_rusage()
{
  return rusage()[0..1];
}
#else
#if RUSAGE == T_MIXED
int *my_rusage()
{
  mixed *foo;
  foo=_rusage();
  return ({foo[13][0]*1000+foo[13][1]/1000000,
	     foo[14][0]*1000+foo[14][1]/1000000});
}
#else
#if RUSAGE == T_MAPPING
int *my_rusage()
{
   mapping foo;
   foo=rusage();
   return ({foo["utime"],foo["stime"});
}
#else
int *my_rusage()
{
  int a,b;
  sscanf(rusage(),"%d %d",a,b);
  return ({a,b});
}
#endif
#endif
#endif
#endif

int gauge(string str)
{
  int c,*t,*tt;
  mixed *a;
  notify_fail("Usage: gauge <command>\n");
  if(!str || str=="") return 0;
#ifndef RUSAGE
  c=my_command(str);
  endwrite("Cost: "+c+"\n");
#else
  t=my_rusage();
  c=my_command(str);
  tt=my_rusage();
  endwrite("Eval cost: "+c+
	   "  CPU time: "+rtime(tt[0]-t[0])+
	   "  System usage: "+rtime(tt[1]-t[1])+
	   "\n");
#endif
}

#ifdef TRACE
int set_traceprefix(string str)
{
  string a;
  if(!str)
  {
    traceprefix(a=traceprefix(0));
    endwrite("Current traceprefix is: "+fast_desc(a)+"\n");
  }
  if(str=="clear") str=(string)0;
  traceprefix(str);
  endwrite("Traceprefix set to "+fast_desc(str)+"\n");
}

int set_trace(string str)
{
  string *args,*a;
  int i, j, trace_arg;

  notify_fail("Usage: trace <off|com|call|call_other|return|args|exec\n"+
	      "             heart_beart|apply|obj_name|number>\n");
  if(!str || str=="")
  {
    trace(i=trace(0));
    endwrite("Current tracelevel is: "+i+"\n");
  }

  a=({"off","com","call","call_other","return","args",
      "exec","heart_beat","apply","obj_name" });

  if (!sizeof(args=my_explode(str, " "))) return 0;
  for(i=0;i<sizeof(args);i++)
  {
    if ((j=member_array(args[i],a))!=-1)
    {
      if(j>1) trace_arg|=({0,0,1,2,4,8,16,32,64,128})[j];
      else if(!j)
      {
	trace(0);
	endwrite("Trace off.\n");
      }else if(j==1)
      {
	history_off=time();
	if(find_function("catch_tell",this_player()))
	  endwrite("Sorry, cannot trace with shadowed catch_tell.\n");
	i=trace(trace_arg);
	INTERNAL_COMMAND(implode(mid_array(args,i+2,sizeof(args)-1)," "));
	history_off=0;
	trace(i);
	return 1;
      }
    }
    else if(sscanf(args[i],"%d",j)) trace_arg|=j;
  }
  if(find_function("catch_tell",this_player()))
    endwrite("Sorry, cannot trace with shadowed catch_tell.\n");

  trace(trace_arg);
  endwrite("Tracelevel set to: "+trace_arg+"\n");
}
#endif

#ifdef my_query_actions
int commands(string str)
{
  mixed *a,b;
  object obj;

  obj=the_bearer();
  if(!str || str=="")
  {
    printarrayofstrings(my_query_actions(obj),0,flagval("columns"));
    return 1;
  }
  a=my_explode_on_char(str,' ');
  switch(sizeof(a))
  {
  case 1:
  {
    if(!(obj=get_ob(a[0])))
      endwrite("No such object\n");
    printarrayofstrings(my_query_actions(obj),0,flagval("columns"));
    break;
  }
  case 2:
  {
    if(!(obj=get_ob(a[0])))
      endwrite("No such object\n");

    b=get_string(a[1]);
    if(!(a=my_query_actions(obj,b)))
      endwrite("That action is not defined by any object\n");

    write("The object "+fast_desc(a[2])+" defined the action like this:\n");
    write("add_action(\""+a[1]+"\",\""+b+"\","+a[0]+");\n");
    break;
  }
  default:
    write("Usage: Localcmd [living] [cmd]\n");
  }
  return 1;
}
#endif

int fork(string a)
{
  object c,d;
  notify_fail("Use : fork shell to <player>\n");
  if(!a || !sscanf(a,"shell to %s",a)) return 0;
  a=lower_case(a);
  if(!(c=find_player(a)))
    endwrite("Fork shell to who?\n");

  if(ENV(c)!=BEARERS_ENV())
    endwrite(capitalize((string)c->query_pronoun())+" is not here.\n");

#if NO_QUICKTYPER
  if(present("quicktyper",c))
    endwrite(capitalize((string)c->query_pronoun())+" got a quicktyper, sorry.\n");
#endif

  if(present("Noedid",c))
    endwrite(capitalize((string)c->query_pronoun())+" already got a shell.\n");

  if(the_bearer()->query_spell_points()<15)
    endwrite("You don't have enough spellpoints.\n");

  the_bearer()->restore_spell_points(-15);
#if NATIVE
  if(!geteuid())
  {
    seteuid(getuid());
    catch(move_object(d=clone_object(fnam),c));
    seteuid(0);
  }else
#endif
   move_object(d=clone_object(fnam),c);
  
  if(!flagval("muffle"))
  {
    say(capitalize(bearer)+" conjures up a shell from nowhere.\n");
    tell_object(c,capitalize(bearer)+" gives you the shell.\n");
    say(capitalize(bearer)+" gives the shell to "+capitalize(a)+".\n",c);
  }
  endwrite("You forked a shell to "+capitalize(a)+".\n");
}

int shellstatus(string str)
{
  int e,invscan;
  string a,b,*c;
  mixed dum;
  int *foo;

  if(SAFECHECK()) return 0;
  if(!str)
  {
    write("flag:     Value:\n");
    c=m_indices(flagtypes);
    foo=m_values(flagtypes);
    for(e=0;e<m_sizeof(flagtypes);e++)
    {
      if((foo[e] & WIZFLAG) && !wizard) continue;
      shellstatus(c[e]);
    }
    return 1;
  }else if(sscanf(str,"%s %s",a,b))
  {
    if(!(e=m_assoc(flagtypes,a)) || ((e & WIZFLAG) && !wizard))
      endwrite("No such flag.\n");

    dum=parse_string(b,e & ~WIZFLAG);
#ifndef BRAINSAVE
    if(stringp(dum) && (sizeof(my_explode(dum,"^!"))>1 ||  sizeof(my_explode(dum,"#"))>1))
      endwrite("You cannot include '^!' or '#' in your status.\n");
#endif
    if(typeof(dum) & e)
    {
      if(a=="light") set_light(dum-flagval("light"));
#ifdef SCAN
      if(a=="invscan" && dum>flagval("invscan"))
	oldobjects=all_inventory(the_bearer());
#endif
#ifdef PEOP
      if(a=="waitforall" && dum>flagval("waitforall"))
	basepatch("setwaitforall",this_object());
#endif
      flagassign(a,dum);
      if(a=="shell") reorganize(1);
      needsave=1;
      my_write("Ok.\n");
    }else{
      endwrite("Wrong type of argument to flag '"+a+"'.\n");
    }
  }else{
    a=str;
    e=m_assoc(flagtypes,a);
    if(!e) endwrite("No such flag.\n");
    if((e & WIZFLAG) && !wizard) return 1;
    if(strlen(a)<10) a=extract(a+"          ",0,9);
    write(a+":");
    if(e & T_STATUS)
      write(onoroff(flagval(str)));
    else
      write(fast_desc(flagval(str)));
    write("\n");
  }
  return 1;
}

int print(string str)
{
  mixed *a;
  a=get_flags(str,"sp");
  if(!a[1])
  {
    write("Result: ");
    if(a[2])
      prettyprint(parse_string((string)a[0],-1),0);
    else
      print_enhance_result(parse_string((string)a[0],-1));
    write("\n");
  }
  else
    write(reparse((string)a[0])+"\n");
  return 1;
}

string *ud;
void updir2(int b)
{
  int a;
  string c;
  object d;

  if(flagval("nice")) a=10; else a=40;
  for(;a && b<sizeof(ud);a--,b++)
  {
    if(d=find_object(ud[b]))
      my_destruct(d,1,0);
  }
  if(b<sizeof(ud)) call_out("updir2",1,b);
  else write("The Thing in the shell tells you: Updated\n");
}

int updir(string s)
{
  if(SAFECHECK()) return 0;
  if(!s) return notify_fail("Usage: Updir <dir>/ or Updir <pattern>\n"),0;
  if(!s || !sizeof(ud=expand00(s)))
    endwrite("No such file or dir.\n");
  ud=regexp(ud,".*\\.c\\>");
  updir2(0);
  endwrite("Updating dir\n");
}

/* There is something strange about this thing.... */
int dir(string str)
{
  string flags,path,*d,name,a,b,fil;
  int e,long,n,max,col,t,m,fast,ladd;
  mixed *q;

  q=get_flags(str,"fl*");
  str=(string)q[0];
  fast=(int)q[1];
  long=(int)q[2];
  ladd=(int)q[3];
  if(!check_read_rights(complete_file_name(str)))
    return 0;

  d=expand00(str);
  if(!sizeof(d))
    endwrite("Permission denied or no such file or dir.\n");

  if(str[strlen(str)-1]=='/')
  {
    write("Contents of "+str+" on "+ctime(time())+"\n");
  }else{
    write("Listing "+str+", "+ctime(time())+"\n");
  }
  n=sizeof(d);
  if(!long)
  {
    for(e=0;e<n;e++)
    {
      if(!fast && file_size(d[e])==-2)
      {
	if(ladd)
	  d[e]=" "+basename(d[e])+"/";
	else
	  d[e]=basename(d[e])+"/";
      }else{
	if(ladd)
	{
	  if(sscanf(d[e],"%s.c%s",a,b) && b=="" && find_object(a))
	    d[e]="*"+basename(d[e]);
	  else
	    d[e]=" "+basename(d[e]);
	}else{
	  d[e]=basename(d[e]);
	}
      }
    }
    printarrayofstrings(d,0,flagval("columns"));
  }else{
    if(!fast)
    {
      for(e=0;e<n;e++)
      {
	m=file_size(d[e]);
	write(m==-2?"     DIR ":right_format(m+" ",9));
#ifdef FILE_DATE
	write(short_time(FILE_DATE(d[e]))+" ");
#endif
	if(ladd)
	{
	  if(m!=-2 && sscanf(d[e],"%s.c%s",a,b) && b=="" && find_object(a))
	  {
	    write("*");
	  }else{
	    write(" ");
	  }
	}
	write(compress_file_name(d[e])+(m==-2?"/\n":"\n"));
      }
    }else{
      if(ladd)
      {
	for(e=0;e<n;e++)
	{
	  if(sscanf(d[e],"%s.%s",a,b) && b=="c" && find_object(a))
	    write("*");
	  else
	    write(" ");
	  write(d[e]+"\n");
	}
      }else{
	write(implode(d,"\n"));
      }
    }
  }
  return 1;
}

string the_more_string;
void simplemore(string str,string thestring)
{
  string a;
  int e,tc;
  tc=flagval("columns")-1;
  if(str=="q") return;
  if(thestring) the_more_string=thestring;
  e=0;
  while(e<22 && the_more_string!="")
  {
    if(!sscanf(the_more_string,"%s\n%s",a,the_more_string))
    {
      a=the_more_string;
      the_more_string="";
    }
    while(a!="" && e<22)
    {
      write(a[0..tc-1]+"\n");
      a=extract(a,tc);
      e++;
    }
  }
  if(stringp(a) && a!="") the_more_string+=a;
  if(the_more_string!="")
  {
    input_to("simplemore");
    write("==== More =====>");
  }
}

int com(string str)
{
  mixed dum;
  dum=get_flags(str,"m");
  str=(string)dum[0];
  dum=dum[1];
  str=implode(map_array(my_explode_on_char(str,' '),
		"get_string",this_object())," ");
  my_write("Com: "+str+"\n");
  if(dum)
  {
    str=popen("my_command",str,0,0,0);
    simplemore("",str);
  }else{
    my_command(str);
  }
  return 1;
}

int page(string str)
{
  str=popen("my_command",str,0,0,0);
  simplemore("",str);
  return 1;
}

int find(string str)
{
  string *a,b,fil,inheritfil,dumpfil,searchpat,excludepat,_shortmatch;
  mixed *c;

  int e,t,tt,hb,liv;
  int filen,filenum,inher,_short;

  c=get_flags(str,"n");
  str=(string)c[e];
  if(str=="")
    endwrite("You must give something to search for.\n");

  if(!c[1]) my_command("dumpallobj");
  a=my_explode_on_char(str,' ');
  tt=t=1;
  for(e=0;e<sizeof(a);e++)
  {
    switch(a[e])
    {
    case "not":
      tt=-tt;
      break;

    case "hb":
      hb=t;
      break;

    case "living":
      liv=t;
      break;

    case "inherit":
      inher=t;
      e++;
      inheritfil=complete_file_name(a[e])+".c";
      if(inheritfil[0]=='/') inheritfil=extract(inheritfil,1);
      break;

    case "short":
      _short=t;
      e++;
      _shortmatch=a[e];
      break;

    default:
      filen=t;
      fil=a[e];
    }
    t=tt;
  }
  if(filen)
  {
    a=my_explode(fil,"*");
    if(sizeof(a)==1)
      endwrite("If you don't have any '*' in the filename you might as well use print.\n");

    filenum=sizeof(a)-1;
    fil=implode(a,"[^ ]*");
  }

  searchpat="";
  if(fil && filen==1) searchpat="^"+fil+"\\> ";
  if(hb==1) searchpat+="\\<HB ";

  excludepat="";
  if(fil && filen==-1) excludepat="^"+fil+"\\> ";
  if(hb==-1) excludepat+="\\<HB ";

  call_out("findob",2,({0,({}),searchpat,liv,inher,inheritfil,excludepat,0,filenum,_short,_shortmatch}));
  endwrite("The thing in the shell starts searching...\n");
}

int findspeed() { return flagval("nice")?75:500; }

object find_map_fun(string a) { sscanf(a,"%s ",a); return find_object(a); }
int find_living_fun(object o,int t) { return living(o)^t; }
int find_inherit_fun(object o,string f)
{
  string *l;
  l=inherit_list(o);
  return pointerp(l) && -1!=member_array(f,l);
}

int find_not_inherit_fun(object o,string f)
{
  string *l;
  l=inherit_list(o);
  return !pointerp(l) || -1==member_array(f,l);
}

int find_short_fun(object o,string f)
{
  string l,*ll;
  catch(l=(string)o->short());
  return stringp(l) && pointerp(ll=regexp(({l}),f)) && !!sizeof(ll);
}

int find_not_short_fun(object o,string f)
{
  string l,*ll;
  catch(l=(string)o->short());
  return !stringp(l) || !pointerp(ll=regexp(({l}),f)) || !sizeof(ll);
}

void findob(mixed arr)
{
  string *alp,y,fil,*lst,*q;
  int e;
  object a,*r;
  if(!(y=read_file(OBJ_DUMP,arr[0],findspeed())))
  {
    assign("found",arr[1]);
    write("The Thing in the shell tells you: I searched "+arr[7]+
	  " objects and found these\n");
	  print_enhance_result(arr[1]);
	  write("\n");
    return;
  }
  arr[0]+=findspeed();
  alp=explode(y,"\n");
  arr[7]+=sizeof(alp);
  if(arr[2]!="") alp=regexp(alp,arr[2]);
  if(arr[6]!="") alp=alp-regexp(alp,arr[6]);
  alp=map_array(alp,"find_map_fun",this_object())-({0});
  if(arr[3]) alp=filter_array(alp,"find_living_fun",this_object(),arr[3]==-1);
  if(arr[4]==1) alp=filter_array(alp,"find_inherit_fun",this_object(),arr[5]);
  if(arr[4]==-1) alp=filter_array(alp,"find_not_inherit_fun",this_object(),arr[5]);
  if(arr[9]==1) alp=filter_array(alp,"find_short_fun",this_object(),arr[10]);
  if(arr[9]==-1) alp=filter_array(alp,"find_not_short_fun",this_object(),arr[10]);
  arr[1]+=(alp-({0}));

  call_out("findob",2,arr);
}

string tmpfile(string ext)
{
  int e;
  string fil,a;
#if NATIVE
  fil="/"+WIZDIR+"/"+geteuid(this_object())+"/ZKPRMB";
#else
  fil="/"+OPENDIR+"/ZKPRMB";
#endif
  e=0;
  while(file_size(fil+e+ext)!=-1 || find_object(fil+e)) e++;
  return fil+e;
}


int _if(string str)
{
  string a,b,*e;
  mixed d;
  int q,cnt,i,err;

  if(!str || !sscanf(str,"%s then %s",a,b))
    endwrite("Usage: if <expr> then <command> [else <command>]\n");

  d=parse_string(a,-1);
  d=d && (string)d!="";
  e=my_explode_on_char(b,' ');
  cnt=1;i=err=0;
  for(q=0;q<sizeof(e) && !err;q++)
  {
    switch(e[q])
    {
    case "if":
      if(i) err=1; else cnt++,i=1; break;
    case "then":
      if(!i) err=2; else i=0; break;
    case "else":
      if(!i) { if(!(--cnt)) err=-1; } break;
    }
#if debug>2
    write("if: "+e[q]+" err:"+err+" i:"+i+" cnt:"+cnt+" d:"+d+" q:"+q+" e[q]:"+e[q]+"\n");
#endif
  }
  switch(err)
  {
  case 1:
    endwrite("error: no then after if.\n");
  case 2:
    endwrite("error: if without then.\n");
  }
  if(err)
  {
    if(d)
      b=implode(mid_array(e,0,q-2)," ");
    else 
      b=implode(mid_array(e,q,sizeof(e)-1)," ");
  }else{
    if(d)
      b=implode(e," ");
    else 
      return 1;
  }

  my_command(b);
  return 1;
}

static object make_object(string str)
{
  string fil,ful,err;
  int e,chuid,nl,nnl;
  mixed a;
  object o;
  string *q,dum2;
  if(SAFECHECK()) return (object)0;
#ifdef EVALLOG
  basepatch("log",EVALLOG,"Eval "+str+"\n");
#endif
  q=m_indices(ass);
  ful="";
  dum2=complete_file_name("~/shell_library.h");
  if(file_size(dum2)>0) ful+="#include \""+dum2+"\"\n";
  dum2=complete_file_name("~/shell_library");
  nl=file_size(LPMUD_LOG);
  if(file_size(dum2+".c")>0)
  {
    if(!find_object(dum2))
    {
      a=catch(call_other(dum2,"???"));
      if(a)
      {
	if(!stringp(a)) a=fast_desc(a);
	write("Error in shell_library.c\n"+a);
	nnl=file_size(LPMUD_LOG);
	if(nnl>nl)
	{
	  err=read_bytes(LPMUD_LOG,nl,nnl-nl);
	  if(err) write(err);
	}
	return 0;
      }
    }
    ful+="inherit \""+dum2+"\";\n";
  }
  if(sizeof(q))
  {
    ful+="private object the_shell,bearer;\n"+
      "void set_what_shell() {\n"+
      "the_shell=previous_object(); \n"+
      "bearer=find_player((string)the_shell->query_bearer()); }\n";

    ful+="int "+replace(implode(q,",")," ","_")+";\nsetglob(){\n";
    for(e=0;e<sizeof(q);e++)
      ful+=replace(q[e]," ","_")+"=previous_object()->get_assign(\""+
	replace(q[e]," ","_")+"\");\n";
    ful+="}\nresetglob(){\n";
    for(e=0;e<sizeof(q);e++)
      ful+="previous_object()->assign(\""+replace(q[e]," ","_")+"\","+
	replace(q[e]," ","_")+");\n";
    ful+="}\nshell(str) { return stringp(str)?(the_shell->parse_string(str,-1)):str; }\n";
  }
#if REMOVE_FUNC
  str+=REMOVE_FUNC_NAME+"() { destruct(this_object()); }\n";
#endif
#if NATIVE
  if(!geteuid())
  {
    write("You must trust the shell to eval LPC.\n");
    return 0;
  }
#endif
  fil=tmpfile(".c");
#if debug
  write(ful+str);
#endif
  write_file(fil+".c",ful+str);
  err=catch(call_other(fil,"??"));
  rm(fil+".c");
  nnl=file_size(LPMUD_LOG);
  if(nnl>nl)
  {
    err=read_bytes(LPMUD_LOG,nl,nnl-nl);
    if(err) write(err);
  }
  if(!(o=find_object(fil)))
  {
    if(!stringp(err)) err=fast_desc(err)+"\n";
    write("Error in compiling statement.\n"+str+err);
    return (object)0;
  }
  o->setglob();
  return o;
}

static mixed internal_eval(string str)
{
  string fil,ful,err;
  int e,chuid;
  mixed a;
  object o;
  string *q;
  
  if(o=make_object(str))
  {
    if(err=catch(a=(mixed)o->func()))
    {
      if(!stringp(err)) err=fast_desc(err)+"\n";
      write("Error occured during evaluation.\n"+err);
    }else{
      o->resetglob();
    }
    dclean(o);
  }
  return a;
}

int eval(string str)
{
  mixed a;
  if(query_verb()=="exec" && !flagval("fizban_mode")) 
    return 0;
  if(!str)
  {
    if(storage) storage+="\n";
    else
    {
      storage="func() {\n";
      input_to("getstr");
      write("Input your function:\n");
      endwrite("End with '.' on a blank line or 'QUIT' to abort.\n:"
	       +storage+":");
    }
  }else{
    if(
       sscanf(str,"string %s",a) ||
       sscanf(str,"int %s",a) ||
       sscanf(str,"return %s",a) ||
       sscanf(str,"mapping %s",a) ||
       sscanf(str,"return %s",a) ||
       sscanf(str,"object %s",a) ||
       sizeof(my_explode_on_char(str,';'))>1)
    {
      storage="func() { "+str+" ;}\n";
    }else{
      storage="func() { return "+str+" ;}\n";
    }
  }
  a=internal_eval(storage);
  storage=(string)0;
  assign("arg",a);
  write("Result: ");
  print_enhance_result(a);
  endwrite("\n");
}

void getstr(string str)
{
  switch(str)
  {
  case ".":
    eval((string)0);
    return;

  case "QUIT":
    storage=(string)0;
    return;
  default:
    storage+=str+"\n";
    write(":");
    input_to("getstr");
  }
}

void shellnews()
{
  if(iswizard())
  {
    PR("News from 15/1-94\n");
    PR("Redo now checks if the masterobject is older than the file before\n");
    PR("updating. Also Ls -l shows the timestamp of the files.\n");
    PR("News from 21/1-94\n");
    PR("The 'I' command have been moved to the default_module and recoded.\n");
    PR("Please read 'shellhelp I'\n");
    PR("News from 14/5-94\n");
    PR("The help about the modules has been moved from 'shellhelp wizard'\n");
    PR("to 'shellhelp <modulename>' some if you have coded modules, reread\n");
    PR("'shellhelp modules'.\n");
    PR("News from 21/5-94\n");
    PR("Redo now writes the actual error when something doesn't load.\n");
    PR("News from 6/8-94\n");
    PR("eval now shows the errors more precisely.\n");
    PR("New command: shellcmd\n");
    PR("News from 19/8-94\n");
    PR("id now shows the number of clones, and is now possible to use\n");
    PR(".clones with shellexpressions. (try 'print /obj/player.clones')\n");
    PR("News from 24/8-94\n");
    PR("Now you can use '::<cmd>' to override an alias, or '\\<cmd>' to \n");
    PR("override _all_ shell commands. (including aliases and nicknames).\n");
    PR("News from 5/6-95\n");
    PR("History now handles replacements similary to csh, see\n");
    PR("'shellhelp %' for more information.\n");
    PR("News from 7/12-95\n");
    PR("egrep now accepts these flags: -vlihn see 'shellhelp egrep' for details.\n");
#define last_wiznews (951207)
  }else{
    PR("News from 19/11-92\n");
    PR("Some useless flags added to the alias command.\n");
    PR("News from 24/11-92\n");
    PR("New shellflag: columns.\n");
    PR("News from 10/2-93\n");
    PR("New flag: echo.\n");
    PR("News from 11/2-93\n");
    PR("Bury and back removed\n");
    PR("News from 12/2-93\n");
    PR("New command: Time\n");
    PR("News from 10/12-93\n");
    PR("New command: page\n");
    PR("News from 10/1-94\n");
    PR("New shellstatus flag: shell (turn shell on/off)\n");
    PR("News from 24/8-94\n");
    PR("Now you can use '::<cmd>' to override an alias, or '\\<cmd>' to \n");
    PR("override _all_ shell commands. (including aliases and nicknames).\n");
    PR("News from 5/6-95\n");
    PR("History now handles replacements similary to csh, see\n");
    PR("'shellhelp %' for more information.\n");
#define last_mortalnews (940605)
  }
  newsread=last_news;
  needsave=1;
}

int help(string str)
{
  string hlp;
  mixed dum;
  if(environment()!=this_player())
    endwrite("You must have the shell to use shellhelp.\n");

  if(str=="news") return shellnews(),1;
  if(!str || str=="")
  {
    PR("The shell allows you the following:\n");
    PR(" alias <name> <def>    :list/undefine/define alias.\n");
    PR(" nickname <name> <def> :list/undefine/define nicknames.\n");
    PR(" com <com> <arg1>....  :Do command with evaluation.\n");
    PR(" page <command>        :Pagebreak the output from a command.\n");
    PR(" Set <var> <value>     :Set/List variables.\n");
    if(!iswizard())
      PR(" history               :list history.\n");
    else
      PR(" history [objects]     :history list.\n");
    PR(" last <n>              :Do the last n commands again.\n");
    PR(" %<nr/start>           :do a command from the history-list.\n");
    PR(" ::<cmd>               :Override aliases\n");
    PR(" \\<cmd>                :Override all shell commands\n");
    if(!iswizard())
      PR(" do <cmd>,<cmd>,.....  :do a list of commands.\n");
    else
      PR(" do [-s] <cmd>,<cmd>.. :do a list of commands.\n");
    PR(" dotimes nr cmd        :do a command a number of times.\n");
    PR(" wait <number>         :Pause an do/dotimes.\n");
   if(iswizard())
    PR(" break [shell | find]  :Stop do/dotimes/find or destruct shell.\n");
   else
    PR(" break [shell]         :Stop do/dotimes or destroy the shell.\n");
    PR(" cont                  :Continue do/dotimes.\n");
    PR(" if..then...else..     :Conditional execution.\n");
    PR(" Tip <message>         :Tip me of how to improve this thing.\n");
    PR(" defaults              :Installs some default aliases.\n");
    PR(" print [-s] <expr>     :Evaluate an expression.\n");
    PR(" shellstatus <fl> <0/1>:Show or change some of the shellflags.\n");
    PR(" shellhelp <topic>     :Get help on a shell command.\n");
    PR(" fork shell to <player>:Give a shell to a player.\n");
#ifdef AUTO
    PR(" auto <command>        :Do the command for everybody present\n");
#endif
#ifdef TIME
    PR(" Time                  :Shows what time it is.\n");
#endif
#ifdef WALK
    PR(" walk <dir> to <word>  :Walk to a specific room.\n");
#endif
    PR("Other topics are: 'expressions', 'news' & 'about'\n");
    if(iswizard())
    {
      PR("Use: 'shellhelp wizard' to list the wizard commands\n");
      PR("or 'shellhelp objects' for info about object-finding.\n");
      PR("Other wiztopics: debugging.\n");
    }
    PR("(If this page doesn't fit your screen, try 'page shellhelp')\n");
    return 1;
  }
  if(!help) help=share("help",wizard);
  if(sscanf(str,"browse %s",str))
  {
    dum=regexp(m_values(help[0]),str);
    dum=map_array(dum,"lookup_map",this_object(),help[1]);
    dum+=basepatch("browse_modulehelp",str,m_indices(modules));
    if(!sizeof(dum)) endwrite("Didn't find anything of interest.\n");
    write("Matched these topics:\n");
    printarrayofstrings(dum,0,flagval("columns"));
    return 1;
  }

  if(!(hlp=m_assoc(help[0],str)))
  {
    mixed foo;
    dum=regexp(m_indices(help[0]),str);
    foo=basepatch("get_modulehelp",str,({m_indices(modules),sizeof(dum)}));
    if(pointerp(foo))
    {
      dum+=foo;
      switch(sizeof(dum))
      {
      case 0:
	endwrite("No such topic.\n");
	break;

      default:
	write("Matched these topics:\n");
	printarrayofstrings(dum,0,flagval("columns"));
	return 1;
      case 1:
	hlp=m_assoc(help[0],dum[0]);

      }
    }else{
      hlp=foo;
    }
  }
/*
  if(m_sizeof(modules) && hlp==m_assoc(help[0],"wizard"))
    hlp+=implode(map_array(m_indices(modules),"get_modulehelp",this_object()),"");
*/
  if(hlp) return printformatted(hlp,flagval("columns")),1;
}

void internal_move(object obj,object dest,int force_move)
{
  object e;
  int q;
  if(!force_move && ENV(obj)==dest)
  {
    my_write(fast_desc(obj)+" is already in "+fast_desc(dest)+" use -m to\n"+
	"override this and move it anyway.\n");
    return;
  }
  if(intp(q=(int)obj->query_weight()))
  {
    if(function_exists("add_weight",dest) && !dest->add_weight(q)
       && !force_move)
    {
      write("Destination can't carry that much, use -m to override.\n");
      return;
    }
    if(ENV(obj)) ENV(obj)->add_weight(-q);
  }
  book(e=ENV(obj));

#ifdef MOVELOG
  basepatch("log",MOVELOG,"Moving "+fast_desc(obj)+" from "+
	    fast_desc(ENV(obj))+" to "+fast_desc(dest)+"\n");
#endif
  if(!flagval("muffle") && !force_move && obj->short())
  {
    if(living(obj))
    {
      tell_room(dest,NAME(obj)+" suddenly appears with a whoop.\n");
      move_object(obj,dest);
      if(e)
	tell_room(e,NAME(obj)+" suddenly disappears with a whoop.\n");
      tell_object(obj,"You are teleported somewhere.\n");
    }else{
      if(obj->short())
      {
	tell_room(dest,capitalize((string)obj->short())
		  +" suddenly appears with a whoop.\n");
	move_object(obj,dest);
	if(e)
	  tell_room(e,capitalize((string)obj->short())
		    +" suddenly disappears with a whoop.\n");
      }
      tell_object(obj,"Something strange has happened.....\n");
      tell_room(obj,"Something strange has happened.....\n");
    }
  }else move_object(obj,dest);
  if(ENV(obj)==dest)
  {
    write(" You teleport "+fast_desc(obj));
    if(e) write(" from "+fast_desc(e));
    write(" to "+fast_desc(dest));
    write("\n");
  }else
    write("Failed to move "+fast_desc(obj)+".\n");
  assign("back",e);
}

object extra_get_object(string str)
{
  object obj;
  string s,t;
#if debug>2
  write("Extra get object "+str+"\n");
#endif
  if(obj=get_oborarr(str)) return obj;
  s=complete_file_name(str);
  t=catch(call_other(s,"??"));
  if(t && t!="*Failed to load file.\n")
  {
    if(!stringp(t)) t=fast_desc(t)+"\n";
    return write("Error: "+t),(object)0;
  }
  if(obj=find_object(s)) return obj;
  s="/room/"+str;
  t=catch(call_other(s,"??"));
  if(t)
  {
    if(!stringp(t)) t=fast_desc(t)+"\n";
    return write("Error: "+t),(object)0;
  }
  if(obj=find_object(s)) return obj;
  return write("No such object.\n"),(object)0;
}

int my_move(string str)
{
  string *a;
  mixed *g,dest,obj;
  int q;

  g=get_flags(str,"m");
  a=my_explode_on_char(g[0],' ');
  if(!sizeof(a))
    endwrite("Usage: Move [-m] <from> <into>\n");

  if(!(obj=get_oborarr(a[0])))
    endwrite("Couldn't find object to move.\n");

  if(sizeof(a)==1) dest=BEARERS_ENV();
  else if(!(dest=extra_get_object(a[1]))) return 1;

  if(pointerp(dest))
  {
    if(!pointerp(obj))
      endwrite("You cannot move one object to many places.\n");

    for(q=0;q<sizeof(obj);q++)
    {
      if(sizeof(dest)<q)
	endwrite("There seems to be less objects to move to than to be moved.\n");

      internal_move(obj[q],dest[q],g[1]);
    }
  }else{
    if(!pointerp(obj)) internal_move(obj,dest,g[1]);
    else for(q=0;q<sizeof(obj);q++) internal_move(obj[q],dest,g[1]);
  }
  return 1;
}

static void _rm2(string str)
{
  if(!check_read_rights(str) || !rm(str))
  {
    write("Failed to remove: "+str+".\n");
  }
}

int _rm(string str)
{
  string *f;
  if(SAFECHECK()) return 0;
  if(!str) f=({ERRORLOG});
  else f=expand0(str);
#ifdef RMLOG
  basepatch("log",RMLOG,"Rm "+implode(f," ")+"\n");
#endif
  my_mapc("_rm2",f,0);
  return 1;
  OK();
}

int _tail(string str)
{
  string *f;
  if(SAFECHECK()) return 0;
  if(!str) f=({ERRORLOG});
  else f=expand0(str);
  my_mapc("_tail2",f,0);
  return 1;
}

static void _tail2(string f)
{
  if(!check_read_rights(f)) return 0;
  tail(complete_file_name(f));
}

int _cat(string str)
{
  string *a;
  int b,t,q,l;

  if(SAFECHECK()) return 0;
  if(!str) return _tail(DEBUG_LOG);

  a=my_explode_on_char(str,' ');
  b=sizeof(a);

  if(b>1 && (t=atoi(a[b-1])))
  {
    if(b>2 && (q=atoi(a[b-2])))
    {
      l=t; t=q;
      b-=2;
    }else{
      l=20;
      b--;
    }
  }else{
    l=20;
    t=1;
  }
  a=expand0(implode(a[0..b-1]," "));
  my_mapc("_cat2",a,({t,l}));
  return 1;
}

static void _cat2(string f,int *b)
{
  if(!check_read_rights(f)) return 0;
  write(read_file(f,b[0],b[1]));
}

int set(string str)
{
  mixed temp;
  string c,d,*q;
  int a;
  mixed *qq;

  clean_assigns();
  if(!str || str=="")
  {
    if(!m_sizeof(ass)) write("No variables.\n");
    else
    {
      write("Variable:             set_to:\n");
      q=m_indices(ass);
      qq=m_values(ass);
      for(a=0;a<m_sizeof(ass);a++)
	write("$"+left_format(q[a],15)+"= "+fast_desc(qq[a])+"\n");
    }
    return 1;
  }
  if(sscanf(str,"%s %s",c,d)!=2)
  {
    forget(str);
    write("Unsetting : "+str+"\n");
  }else{
    temp=parse_string(d,-1);
    assign(c,temp);
    if(flagval("notify"))
    {
      write("Setting: $"+c+" to ");
      print_enhance_result(temp);
      write("\n");
    }
  }
  return 1;
}

int override(string a)
{
  if(a[0]!=':') return 0;
  if(SAFECHECK()) return 0;
  alias_off=time();
  INTERNAL_COMMAND(a[1..10000]);
  alias_off=0;
  return 1;
}

int setvar(string a)
{
  string d,dd;
  mixed b;
/*  a=extract(query_verb()+(a?a:""),1); */
  if(sscanf(a,"%s=%s",d,dd)==2) return set(d+" "+dd);
  if(stringp(b=get_assign(a))) return my_command((string)b),1;
  return 0;
}

int list()
{
  int a;
  object o;
  string b,c;
  if(current_obj==0)
  {
    write("No accessed objects.\n");
  }else{
    write("Object History:\n");
    a=current_obj-HISTORY_SIZE;
    if(a<1) a=1;
    for(;a<=current_obj;a++)
    {
      o=obj_history[a % HISTORY_SIZE];
      if(o)
      {
	c="      "+a;
	c=extract(c,strlen(c)-3);
	write(c+":"+fast_desc(o)+"\n");
      }
    }
  }
  return 1;
}

int tip(string s)
{
  object p;
  int ch;
  if(!s && bearer=="profezzorn") return _tail("/log/profezzorn.tip");
  if(!s || s=="") return 0;
#if NATIVE
  if(!geteuid())
  {
    seteuid(getuid());
    ch=1;
  }
#endif
  catch(log_file("profezzorn.tip",bearer+"'s shell: "+s+"\n"));
#if NATIVE
  if(ch) seteuid(0);
#endif
  write("Profezzorn has been notified.\n");
  if(p=find_player("profezzorn"))
    tell_object(p,
      "The thing in "+this_name()+"'s shell tell you: You have a tip.\n");
  return 1;
}

void tagit()
{
  object ob,thisp;
  thisp=the_bearer();
  if(!thisp || !tagging || !ENV(tagging) || !interactive(thisp))
  {
    tagging=(object)0;
  }else{
    if(ENV(thisp)!=ENV(tagging))
    if(flagval("muffle"))
    {
      move_object(thisp,ENV(tagging));
      write("("+BEARERS_ENV()->short()+")\n");
    }else{
      my_write("The Thing in the shell teleport you to "+NAME(tagging)+".\n");
      thisp->move_player("X",ENV(tagging));
    }
  }
  if(tagging && -1==find_call_out("tagit")) call_out("tagit",1);
}

int tag(string str)
{
  object obj,b;
  if(str)
    if(!(obj=get_ob(str)))
      endwrite("No such object.\n");

  if(tagging && tagging==obj)
  {
    write("The Thing in the shell tells you: You're alredy tagging that.\n");
    return 1;
  }
  if(tagging)
  {
    write("The Thing in the shell lets "+NAME(tagging)+" go.\n");
    tagging=(object)0;
  }

  if(obj==the_bearer())
  {
    write("The Thing in the shell tells you: Tagging yourself is a waste of cpu.\n");
    return 1;
  }

  tagging=obj;
  if(tagging) tagit();
  return 1;
}

#if defined(FILE_DATE) && defined(OBJECT_CREATED)
int check_if_update_needed(object ob)
{
  object o;
  string *f;
  int e,t;
  t=OBJECT_CREATED(ob);
  f=inherit_list(ob);
  for(e=0;e<sizeof(f);e++)
  {
    if(e && (o=find_object(f[e])))
    {
      if(OBJECT_CREATED(o)>=t)
	return 1;
    }else{
      if(file_size(f[e])<=0 || FILE_DATE(f[e])>=t)
	return 1;
    }
  }
  return 0;
}
#endif
void update2(object obj,int noreload)
{
  object temp,b,tt;
  string name,master,err,updated;
  int dum;
  object *ehh,urkel,gurgel;

  if(SAFECHECK()) return;
  if(!test_destruct(obj))
  {
    write("Too dangerous to REDO "+fast_desc(obj)+" , check dangerlevel.\n");
    return;
  }

  if(noreload & 2)
    my_mapc("my_destruct",
	    map_array(inherit_list(obj)[1..256],
		      "my_find_object",this_object()),0);

  temp=ENV(obj);
  ehh=all_inventory(obj);
  call_other(VOID,"??");
  tt=find_object(VOID);
  movem(ehh,tt);
  name=my_file_name(obj);
  if(sscanf(name,"%s#%d",master,dum)==2 && dum)
  {
    if((b=find_object(master)) &&
       !(noreload & 1) &&
#if defined(FILE_DATE) && defined(OBJECT_CREATED)
       ((noreload & 4) || check_if_update_needed(b)) &&
#endif
       -1==member_array(master,get_assign("redone")))
    {
#if debug>2
      write("Update: '"+master+"'\n");
#endif
      dclean(b);
      name=master;
      assign("redone",get_assign("redone")+({name}));
      updated="updated & ";
    }else{
      if(!b) updated="loaded & ";
      else updated="";
    }
#if debug>2
      write("Cloning: '"+master+"'\n");
#endif
    err=catch(urkel=clone_object(master));
    if(err && !stringp(err)) err=fast_desc(err)+"\n";
    if(!urkel)
    {
      write(capitalize(updated+"failed to clone :"+name+"\n"+err));
    }else{
      if(!err)
      {
	write(capitalize(updated+"recloned :"+name+"\n"));
      }else{
	write(capitalize(updated+"recloned with error :"+name+"\n"));
      }
      gurgel=obj;
      obj=urkel;
      book(obj);
      if(temp) move_object(obj,temp);
      if(temp && interactive(temp) && obj->query_wielded())
	my_tell_object(temp,"You suddenly loose the grip on your weapon.\n");

      if(gurgel) dclean(gurgel);
    }
    movem(ehh,obj);
  }else{
    dclean(obj);
#if debug>2
      write("Loading: '"+name+"'\n");
#endif
    err=catch(call_other(name,"??"));
    if(err && !stringp(err)) err=fast_desc(err)+"\n";
    obj=find_object(name);
    if(!obj)
    {
      write("Updated & failed to load :"+name+"\n"+err);
      if(temp) movem(ehh,temp);
    }else{
      if(!err)
      {
	write("Updated & loaded :"+name+"\n");
      }else{
	write("Updated & loaded with error :"+name+"\n"+err);
      }
      if(temp) move_object(obj,temp);
      book(obj);
      movem(ehh,obj);
    }
  }
  return;
}

int update(string str)
{
  int dum,noreload;
  mixed *args,obj;
  int nl;

  args=get_flags(str,"nafl");
  str=(string)args[0];

  if(!args[4]) args[4]=file_size(LPMUD_LOG);

  if(!sizeof(obj=get_arrofob(str)))
  {
    write("No such object.\n");
    load(str);
  }else{
    assign("redone",({}));
    my_mapc("update2",obj,args[1]+args[2]*2+args[3]*4);
    assign("redone",0);
  }
  if(args[4]>0)
  {
    nl=file_size(LPMUD_LOG);
    if(nl>args[4])
    {
      str=read_bytes(LPMUD_LOG,args[4],nl-args[4]);
      if(str) write(str);
    }
  }
  return 1;
}

int undo(string str)
{
  object *obj,a;
  mixed *b;
  if(query_verb()=="Destruct" && !flagval("fizban_mode"))
    return 0;
  b=get_flags(str,"r");
  str=b[0]==""?(string)0:b[0];
  if(!sizeof(obj=get_arrofob(str)))
    endwrite("No such object.\n");

  my_mapc("my_destruct",obj,b[1]);
  return 1;
}

int patch(string str)
{
  mixed res,*q;
  int e;

  if(SAFECHECK()) return 0;
  if(!str)
    endwrite("Usage: patch object function <argument> <argument2> ...\n");

  q=my_explode_on_char(str,' ');
  if(sizeof(q)<2)
    endwrite("Usage: patch object function <argument> <argument2> ...\n");

  for(e=2;e<sizeof(q);e++) q[e]=parse_string(q[e],-1);
  res=patch2(q[0],q[1..10000],1);
  assign("arg",res);
  return 1;
}


static mixed *patch2(mixed a,mixed q,int g)
{
  mixed res;
  int e,fl;
  mixed obj,b,c;

#if debug
  write("patch2 : ("+fast_desc(a)+","+enhance_result(q)+")\n");
#endif
  b=q[0];
  q=mid_array(q,1,sizeof(q)-1)+allocate(16);
  if(objectp(a)) obj=a;
  if(!obj && !(obj=get_oborarr(a)))
  {
    catch(call_other(complete_file_name(a),"??"));
    obj=get_ob(complete_file_name(a));
  }
  if(!obj)
  {
    if(g) write("No such object found.\n");
    return 0;
  }
  if(!pointerp(obj)) { obj=({obj}); fl=1; }
  c=allocate(sizeof(obj));

#ifdef PATCHLOG
  e=sizeof(q)-1;
  while(!q[e] && e) e--;
  basepatch("log",PATCHLOG,"Patch "+enhance_result(obj)+"->"+b+
	    enhance_result(q[0..e])+"\n");

#endif

  for(e=0;e<sizeof(obj);e++)
  {
    if(!obj[e])
    {
      if(g) write("No such object.\n");
    }else if(!find_function(b,obj[e])){
      if(g) write("There is no such function in "+fast_desc(obj[e])+"\n");
    }else{
      res=(mixed)call_other(obj[e],b,q[0],q[1],q[2],q[3],q[4],q[5],q[6]
	,q[7],q[8],q[9],q[11],q[12],q[13],q[14],q[15]);
      if(g)
      {
	write("Result:");
	print_enhance_result(res);
	write("\n");
      }
      c[e]=res;
    }
  }
  if(fl) return c[0]; else return c;
}


void dump_assoc_list(mixed foo,int inverse)
{
  string *a,*b,*tmp,v;
  mixed i;
  MYMAP ut;
  int e,d;

#if mappings
  if(mappingp(foo))
  {
    ut=EMPTY_MAP;
    a=m_indices(foo);
    b=m_values(foo);
    for(e=0;e<sizeof(a);e++)
    {
      i=a[e];
      v=b[e];
      if(!stringp(v)) v=fast_desc(v);
      if(tmp=m_assoc(ut,v))
      {
	ut=m_assign(ut,v,tmp+({i}));
      }else{
	ut=m_assign(ut,v,({i}));
      }
    }
  }
#endif
  if(pointerp(foo))
  {
    ut=EMPTY_MAP;
    for(e=0;e<sizeof(foo)-1;e+=2)
    {
      if(inverse)
      {
	i=foo[e+1];
	v=foo[e];
      }else{
	i=foo[e];
	v=foo[e+1];
      }
      if(!stringp(v)) v=fast_desc(v);
      if(!pointerp(i)) i=({i});
      for(d=0;d<sizeof(i);d++)
      {
	if(tmp=m_assoc(ut,v))
	{
	  ut=m_assign(ut,v,tmp+({i[d]}));
	}else{
	  ut=m_assign(ut,v,({i[d]}));
	}
      }
    }
  }
  if(!ut)
  {
    write("Unknown assoc type.\n");
    return;
  }
  a=m_indices(ut);
  foo=m_values(ut);
  for(e=0;e<sizeof(a);e++)
  {
    if(sizeof(foo[e])!=1)
    {
      write("["+implode(foo[e],",")+"]\n");
    }else{
      write("["+foo[e][0]+"] ");
      if(sscanf(a[e],"%s\n",v))
	write("\n");
    }
    write(a[e]);
    if(a[e][strlen(a[e])-1]!='\n') write("\n");
  }
}

int identify(string str)
{
  object obj;
  mixed b,*arg,*cache;
  string d,flags;
  mixed *lst;
  string *query_list, *query_list2, *not_query_list;
  string *data;
  int c,*age;

  arg=get_flags(str,"alndt");
#define ID_ALL 1
#define ID_LONG 2
#define ID_NO_LONG 3
#define ID_DEBUG 4
#define ID_TIME 5

  if(!(obj=get_ob((string)arg[0])))
    endwrite("No such object.\n");

  if(arg[ID_DEBUG])
  {
    debug_info(0,obj);
    debug_info(1,obj);
    return 1;
  }

  data=({});
    
  data+=({"Filename: "+short_file_name(obj)});

  lst=get_all_clones(obj);
  if(lst && sizeof(lst)>1)
  {
    lst-=({ master_of(obj), 0 });
    if(sizeof(lst)>1)
    {
      data+=({"Clones: "+sizeof(lst)});
    }
  }


#if NATIVE
  data+=({"Uid: "+getuid(obj),"Euid: "+geteuid(obj)});
#else
  if(creator(obj) && creator(obj)!=my_creator(obj))
    data+=({"Creator :"+creator(obj)});
#endif
#ifdef OBJECT_CPU
  data+=({"Object cpu: "+OBJECT_CPU(obj)});
#endif
#ifdef OBJECT_MEMORY
  data+=({"Memory used: "+OBJECT_MEMORY(obj)});
#endif

  if(b=shadow(obj,0)) 
    data+=({"Shadowed by: "+fast_desc(b) });

  if(interactive(obj))
  {
    if(b=query_snoop(obj))
      data+=({"Snooped by: "+fast_desc(b)});

    if(query_idle(obj)) 
       data+=({"Idle: "+nice_period(query_idle(obj))});

#ifdef IDENT
    if(IDENT(obj))
      data+=({"Playing from: "+IDENT(obj)+"@"+query_ip_name(obj)});
    else
#endif
      data+=({"Playing from: "+query_ip_name(obj)});
  }
  
  printarrayofstrings(data,0,flagval("columns"));

  if(function_exists("show_stats",obj) && !arg[ID_LONG])
  {
    obj->show_stats();
    return 1;
  }


#if defined(OBJECT_CREATED) || defined(PEOP)

#ifndef OBJECT_CREATED
  if(sscanf(my_file_name(obj),"%s#%d",d,c) && !arg[ID_TIME])
#endif
    age=query_age(obj);
  
  if(age && (age[0]!=-1 || age[1]!=-1))
  {
    if(age[0] == -1)
    {
      write("Object is older than "+nice_period(time()-age[1])+".\n");
    }
    else if(age[1] == -1)
    {
      write("Object is newer than "+nice_period(time()-age[0])+".\n");
    }
    else if(age[0] == age[1])
    {
      write("Object is "+nice_period(time()-age[0])+" old.\n");
    }
    else
    {
      age[1]-=age[0];
      age[0]+=age[1]/2;
      age[0]+=30;
      age[1]+=59;
      age[0]-=age[0]%60;
      age[1]-=age[1]%60;
      write("Object is "+nice_period(time()-age[0])+" +/- "+nice_period(age[1])+" old.\n");
    }

  }

#endif

  if(sizeof(lst=inherit_list(obj))>1)
  {
    lst=map_array(lst,"compress_file_name",this_object());
    lst[0]="Inherits:";
    printarrayofstrings(lst,1,flagval("columns"));
  }


  if(arg[ID_ALL]) query_list2=share("query_list2",wizard);
  query_list=share("query_list",wizard);
  not_query_list=share("not_query_list",wizard);

  if(!arg[ID_NO_LONG])
  {
    for(c=0;c<sizeof(query_list);c++)
      if(function_exists("query_"+query_list[c],obj))
	if(b=(mixed)call_other(obj,"query_"+query_list[c]))
	  write(left_format(query_list[c],20)+":"+fast_desc(b)+"\n");
    for(c=0;c<sizeof(not_query_list);c++)
      if(function_exists(not_query_list[c],obj))
	if(b=(mixed)call_other(obj,not_query_list[c]))
	  write(left_format(not_query_list[c],20)+":"+fast_desc(b)+"\n");
  }
  if(arg[ID_ALL])
    for(c=0;c<sizeof(query_list2);c++)
      if(function_exists("query_"+query_list2[c],obj))
	if(b=(mixed)call_other(obj,"query_"+query_list2[c]))
	  write(left_format(query_list2[c],20)+":"+fast_desc(b)+"\n");

  cache=(string *)obj->query_items() || (string *)obj->query_item_map();
  if(cache)
  {
    write("Items:\n");
    dump_assoc_list(cache,0);
  }

  cache=(string *)obj->query_dest_dir() || (string *)obj->query_exits();
  if(cache)
  {
    write("Exits:\n");
    dump_assoc_list(cache,1);
  }

  cache=(string *)obj->query_property();
  if(cache && pointerp(cache) && sizeof(cache))
  {
    cache=({"Properties:"})+cache;
    printarrayofstrings(cache,1,flagval("columns"));
  }

  write("Long:\n");
  obj->long();
  return 1;
}

string *query_query_list()
{
  return ({
    share("query_list",wizard)[0..10000],
    share("query_list2",wizard)[0..10000],
    share("not_query_list",wizard)[0..10000]
  });
}

int clone(string str)
{
  mixed *qul;
  int flag;

#ifdef CLONELOG
  basepatch("log",CLONELOG,"Clone "+str+"\n");
#endif
  qul=get_flags(str,"n");
  str=qul[0];
  flag=qul[1];

  if(!str || str=="")
    endwrite("Must have something to clone.\n");

  qul=expand0(str);
  my_mapc("clone2",qul,flag);
  return 1;
}

void clone2(string str,int flag)
{
  object ob;
  string a;

  if(SAFECHECK()) return;
  sscanf(str,"%s.c",str);
  if(flag)
    ob=clone_object(str);
  else
    a=catch(ob=clone_object(str));
    
  if(ob)
  {
    book(ob);
    if(ob->get())
    {
      the_bearer()->add_weight((int)ob->query_weight());
      move_object(ob,the_bearer());
      my_say(this_player()->query_name()+" takes something out of "+this_player()->query_possessive()+" hat.\n");
    }else{
      move_object(ob,BEARERS_ENV());
      if(ob->short())
	my_say(this_player()->query_name()+" summons "+ob->short()+".\n");
      else
	my_say(this_player()->query_name()+" recites a magical formula.\n");
    }
    flagassign("muffle",1);
    write("Ok.\n");
  }else{
    if(!stringp(a)) a=fast_desc(a)+"\n";
    write("Failed to clone :"+str+"\n");
    write("Error: "+a+"\n");
  }
}

int load(string str)
{
  if(!str || str=="")
    endwrite("Must have something to load.\n");

  my_mapc("load2",expand0(str),0);
  return 1;
}

int load2(string str)
{
  object b;
  string a;

  if(stringp(str)) str=complete_file_name(str);
  if(objectp(str) || find_object(str))
    endwrite(fast_desc(str)+" is already loaded\n");
  if(!stringp(str)) endwrite("Argument must be as string!\n");

  a=catch(call_other(str,"??"));
  if(!a)
  {
    my_say(this_player()->query_name()+" mumbles a magical rune.\n");
    flagassign("muffle",1);
    write("Loaded "+str+".\n");
  }else{
   if(!stringp(a)) a=fast_desc(a)+"\n";
    write("Failed to load :"+str+"\n");
    write("Error: "+a+"\n");
  }
  return 1;
}


int executor_executing;

void _stop()
{
  alt_stack=stack;
  stack=0;
  executor_executing=0;
}

#define POP() stack=stack[COM__PREV],s=1;

static void executor()
{
  string tmp,cmd;
  int t,s;

  if(!stack) return 0;
  do
  {
    switch(stack[COM__CMD])
    {
    case COM_DO:
      cmd=stack[COM_DO_COMMANDS][stack[COM_DO_COUNTER]++];
      if(stack[COM_DO_COUNTER] >= sizeof(stack[COM_DO_COMMANDS]))
	POP();
      break;

    case COM_WAIT:
      assign("waiting",1);
      if(!flagval("nice"))
      {
	flagassign("nice",1);
	stack[COM__PREV]=({stack[COM__PREV],COM_SET_NICE,flagval("nice")});
      }
      if(stack[COM_WAIT_UNTIL]<time()) POP();
      break;

    case COM_SET_NICE:
      flagassign("nice",stack[COM_SET_NICE_FLAG]);
      POP();
      s=0;
      break;

    case COM_DOTIMES:
      cmd=stack[COM_DOTIMES_COMMAND];
      assign("donum",stack[COM_DOTIMES_NUM]);
      if(flagval("nice"))
	my_write(stack[COM_DOTIMES_NUM]+" time"+(stack[COM_DOTIMES_NUM]==1?"":"s")+" left.\n");
      if(!--stack[COM_DOTIMES_NUM]) POP();
      break;

#ifdef WALK
    case COM_WALK:
      tmp=lower_case(environment(the_bearer())->short()+" "+
	environment(the_bearer())->query_long());
      if(sscanf(tmp,"%s"+stack[COM_WALK_UNTIL],tmp) ||
	present(stack[COM_WALK_UNTIL],environment(the_bearer())))
      {
        POP();
      }else{
        cmd=stack[COM_WALK_CMD];
      }
      break;
#endif
    }

    if(cmd)
    {
      my_write("Shell doing: "+cmd+"\n");
      t=my_command(cmd);
      cmd=0;
      if(!t)
      {
        write("No such command, aborting.\n");
        _stop();
      }
    }
  }while(stack && !flagval("nice"));
  if(s) my_write("Shell: Finished.\n");
}

void ticker()
{
  mixed p;
  if(executor_executing!=time())
  {
    executor_executing=time();
    p=catch(executor());
    executor();
    executor_executing=0;
    if(stack && !p)
    {
      remove_call_out("ticker");
      call_out("ticker",2);
    }else{
      if(p)
      {
	write("Something went wrong, aborting\n");
	if(wizard) write(p);
	_stop();
      }
    }
  }
}

int _break(string s)
{
  notify_fail("Break what?\n");
  if(s && s!="")
  {
    switch(lower_case(s))
    {
    case "find":
      remove_call_out("findob");
      OK();
    case "saves":
      fnam=0;
      reset();
      needsave=1;
      OK();

    case "shell":
      PROFILE("break")
      write("You break the shell into small small small pieces.\n");
      dclean(this_object());
      return 1;
    }
  }else{
    _stop();
    OK();
  }
  return 0;
}

int last(string s)
{
  int d;
  string *commands;
  if(!s) d=1; else d=get_int(s);
  if(d<1 || d>HISTORY_SIZE)
    endwrite("Cannot do less than 1 or more than "+HISTORY_SIZE+" commands.\n");
  commands=({});
  for(;d;d--)
    commands+=({history_list[(current+HISTORY_SIZE+1-d) % HISTORY_SIZE]});
  stack=({stack,COM_DO,0,commands});
  ticker();
  return 1;
}

int _do(string str)
{
  string a,b,*c;
  object who;
  mixed *q;
  int sile;
  int *ss;
  MYMAP tmp;

#if debug>1
  write("Do :"+str+"\n");
#endif

  q=get_flags(str,"s");
  str=(string)q[0];
  if(iswizard()) sile=(int)q[1];
  who=the_bearer();
  if(!str || str=="") return 0;
  if(who!=environment() || !interactive(who)) return 1;
  stack=({stack,COM_DO,0,my_explode_on_char(str,',')});
  if(sile)
  {
    ss=({flagval("nice"),flagval("notify")});
    flagassign("nice",0);
    flagassign("notify",0);
    ticker();
    flagassign("nice",ss[0]);
    flagassign("notify",ss[1]);
  }else{
    ticker();
  }
  return 1;
}

int cont()
{
  stack=alt_stack;
  alt_stack=0;
  ticker();
  OK();
}

int _wait(string str)
{
  int a;
  a=atoi(str);
  if(a<1) return 1;
  stack=({stack,COM_WAIT,a+time()});
  ticker();
  return 1;
}

int dotimes(string str)
{
  int a,e,ww;
  string b,*u;
  object who;
#if debug>1
  write("Dotimes :"+str+",who="+enhance_result(who)+"\n");
#endif
  who=the_bearer(); 
  if(!str || str=="") return 0;
  if(who!=environment() || !interactive(who)) return 1;

  u=my_explode_on_char(str,' ');
  if(sizeof(u))  a=get_int(u[0]);
  if(a<1 || sizeof(u)<2)
    endwrite("Usage : dotimes nr command\n");

  b=implode(mid_array(u,1,sizeof(u)-1)," ");
  stack=({stack,COM_DOTIMES,a,b});
  ticker();
  return 1;
}

#ifdef WALK
int walk(string str)
{
  string a,b;
  notify_fail("Use as: walk <direction> to <word>\n"+
	"where <word> is the brief description of that room.\n");
  if(!str) return 0;
  if(!sscanf(str,"%s to %s",a,b) && !sscanf(str,"%s until %s",a,b)) return 0;
  switch(a=lower_case(a))
  {
  default:
     endwrite("You must choose a normal direction.\n");
  case "north":     case "n":  case "south":     case "s":
  case "east":      case "e":  case "west":      case "w":
  case "northwest": case "nw": case "northeast": case "ne":
  case "southwest": case "sw": case "southeast": case "se":
  case "up":        case "u":  case "down":      case "d":

  }
  stack=({stack,COM_WALK,a,b});
  ticker();
  return 1;
}
#endif

int nick(string str)
{
  int e;
  string a,b,*c,*q;

  if(SAFECHECK()) return 0;
  if(!str)
  {
    if(!m_sizeof(nicknames))
      endwrite("No nicknames.\n");

    write("Nickname:     Real name:\n");
    c=m_indices(nicknames);
    q=m_values(nicknames);
    for(e;e<m_sizeof(nicknames);e++)
    {
      a=c[e];
      if(strlen(a)<10) a=extract(a+"          ",0,9);
      write(a+":"+q[e]+"\n");
    }
    return 1;
  }else{
    str=implode(explode(str," ")," ");
    e=sscanf(a=str,"%s %s",a,b);
    b=" "+b+" ";
    if(e!=2)
    {
      if(!m_assoc(nicknames,a))	endwrite("No such nickname.\n");
      nicknames=m_delete(nicknames,a);
      my_write("'"+a+"' removed.\n");
    }else if(e==2)
    {
      if(m_sizeof(nicknames)>=MAX_NICKNAMES)
	endwrite("You already have the maximum number of nicknames.\n");

      if(" "+a+" "==b)
	endwrite("Why?\n");
      if(sizeof(m_indices(nicknames)-(m_indices(nicknames)-my_explode(b," "))) ||
	 -1!=member_array(a,my_explode(b," ")))
	endwrite("Don't do recursive nicknames!\n");

#ifndef BRAINSAVE
      if(sizeof(my_explode(b,"^!"))>1 || sizeof(my_explode(a,"^!"))>1 ||
       sizeof(my_explode(b,"#"))>1 || sizeof(my_explode(a,"#"))>1 )
	endwrite("You cannot include '^!' or '#' in your nickname.\n");
#endif

      nicknames=m_assign(nicknames,a,b);
      my_write("Ok.\n");
    }else{
      return notify_fail("Use: nickname <nickname> [real_name]\n"),0;
    }
  }
  needsave=1;
  return 1;
}

int letterorder(string a,string b)
{
  if(a>b) return 1;
  if(a<b) return -1;
  return 0;
}
int macro(string str)
{
  int e,flag;
  string a,b,*c,*q;
  mixed *t;

  if(SAFECHECK()) return 0;
  if(!str)
  {
    if(!m_sizeof(aliases)) endwrite("No aliases.\n");

    write("Alias:     Defined as:\n");
    c=m_indices(aliases);
    c=sort_array(c,"letterorder",this_object());
    for(e;e<m_sizeof(aliases);e++)
    {
      a=c[e];
      write((strlen(a) < 10 ? extract(a+"          ",0,9) : a )+
	    ":"+m_assoc(aliases,a)+"\n");
    }
    return 1;
  }else{
    t=get_flags(str,"lc");
    str=(string)t[0];
    str=implode(explode(str," ")," ");
    e=sscanf(str,"%s %s",a,b);
    flag=t[1]*2+t[2];
    if(flag==1)
    {
      aliases=EMPTY_MAP;
      endwrite("All aliases removed.\n");
    }
    if(flag==2)
    {
      a=m_assoc(aliases,str);
      if(!a)
      {
	c=regexp(m_indices(aliases),"^"+str);
	if(!sizeof(c)) endwrite("No such alias.\n");
	write("The following aliases matched:\n");
	printarrayofstrings(c,0,flagval("columns"));
	return 1;
      }
      endwrite("Alias "+str+" is defined as:\n"+a+"\n");
    }
    if(flag==3) endwrite("Illigal combination of flags.\n");
    if(e!=2)
    {
      if(!m_assoc(aliases,str))
      {
	my_write("No such alias.\n");
      }else{
	aliases=m_delete(aliases,str);
	my_write("'"+str+"' removed.\n");
      }
    }else if(e==2)
    {
      if(m_sizeof(aliases)>=MAX_ALIASES)
	endwrite("You already have the maximum number of aliases.\n");

      if(a=="alias" || a=="shellhelp" || a=="exa" || a=="examine")
	endwrite("You cannot redefine the '"+a+"' command.\n");

#ifndef BRAINSAVE
      if(sizeof(my_explode(b,"^!"))>1 || sizeof(my_explode(a,"^!"))>1 ||
       sizeof(my_explode(b,"#"))>1 || sizeof(my_explode(a,"#"))>1 )
	endwrite("You cannot include '^!' or '#' in your alias.\n");
#endif
      aliases=m_assign(aliases,a,b);
      my_write("Ok, "+a+" = "+b+"\n");
    }else{
      return notify_fail("Use: alias <short> [command]\n"),0;
    }
  }
  needsave=1;
  return 1;
}

MYMAP query_running() { return running; }

int do_macro(string a,string str)
{
  string q,*c,*b,verb,rest;
  int e,d,ny,gammal,count;
  int max;

  if((count=m_assoc(running,verb=query_verb()))>3)
    endwrite("Shell: The alias '"+verb+"' entered itself too many times!\n");

  running=m_assign(running,verb,count+1);
  if(str)
  {
    if(wizard)
      gammal=sizeof(c=my_explode_on_char(str,' '));
    else
      gammal=sizeof(c=my_explode(str," "));
  }else{
    gammal=0;
  }
  ny=sizeof(b=my_explode(a,"%"));
  max=-1;
  for(e=1;e<ny;e++)
  {
    if(b[e]=="") { b[e]="%"; e++; continue; }
    if(sscanf(b[e],"%d%s",d,q) && d>0)
    {
      b[e]=((gammal>--d)?c[d]:"")+q;
      if(max<d) max=d;
    }else{
      b[e]="%"+b[e];
    }
  }
  q=implode(b,"");
  rest=c?implode(c[max+1..10000]," "):"";
  if(sizeof(b=my_explode(q,"%@"))>1)
  {
    q=implode(b,rest);
  }else{
    q+=" "+rest;
  }
  
#if debug
  write("Alias substitution: "+q+"\n");
#endif
  my_command(q);
  running=m_assign(running,verb,count);
  return 1;
}

int history(string str)
{
  int a;
  string b;

  if(SAFECHECK()) return 0;
  if(str && sscanf("objects   ",str+"%s",b) && strlen(b)>1) return list();
  history_off=0;
  if(current==0)
    endwrite("No parent commands.\n");

  write("History:\n");
  a=current-HISTORY_SIZE+1;
  if(a<1) a=1;
  for(;a<=current;a++) write(a+": "+history_list[a % HISTORY_SIZE]+"\n");
  return 1;
}

int history_cmd(string str)
{
  string a,b,c,d;
  int q,e;
  string *tmp;

  notify_fail("No such history command.\n");
  a=query_verb();
  if(str) a+=" "+str;

  a=extract(a,1);
  if(a[0]=='%')
  {
    q=current;
    a=extract(a,1);
  }
  else if(sscanf(a,"%d%s",q,a)==2)
  {
    if(q<0) q=current+q+1;
    if(!(current-HISTORY_SIZE<q && q<=current))
      endwrite("That number is not in the history list.\n");
  }else{
    if(sscanf(a,"%s:%s",a,b))
      b=":"+b;
    else
      b="";

    if(strlen(a))
    {
      a+="%s";
      for(q=0;q<HISTORY_SIZE;q++)
      {
	e=(HISTORY_SIZE+current-q) % HISTORY_SIZE;
	if(sscanf(history_list[e],a,a))
	{
	  break;
	}
      }
    }

    if(q==HISTORY_SIZE) return 0;
    q=current-q;
    a=b;
  }

  b=history_list[q % HISTORY_SIZE];

  while(a[0]==':')
  {
    a=extract(a,1);
    switch(a[0])
    {
    case 's':
      c=a[1..1];
      tmp=my_explode(a,c);
      b=replace(b,tmp[1],tmp[2]);
      a=implode(tmp[3..sizeof(tmp)-1],c);
      break;
    default:
      endwrite("Error in history command.\n");
    }
  }

  b+=a;

  my_write("History replace: "+b+"\n");
  INTERNAL_COMMAND(b);
  return 1;
}

int do_call(string str,string *data,string com)
{
  int e,t,mapflag,type,fin,optional;
  string q,a,b,err;
  mixed *args,*tmp,*many;
  mixed obj;

  PROFILE("do_call")
  mapflag=-1;
  args=allocate(sizeof(data));
  for(e=2;e<sizeof(data) && !err;e++)
  {
#if debug>4
    write("Parsing command: '"+str+"' ("+e+" = "+data[e]+")\n");
#endif
    q=data[e];
    optional=fin=type=0;
    for(t=0;t<strlen(q) && fin<1 && !err;t++)
    {
      switch(q[t])
      {
      case '~': optional++; break;
      case '@': mapflag=e; type|=T_POINTER; break;
      case '#': many=({}); break;
      case '*': type|=T_POINTER; break;
      case '+': fin--; break;
      case 'O': type|=T_OBJECT; fin++; break;
      case 'S': type|=T_STRING; fin++; break;
      case 'P': type|=T_POINTER; fin++; break;
      case 'D': type|=T_NUMBER; fin++; break;
#ifdef mappings
      case 'M': type|=T_MAPPING; fin++; break;
#endif
      case 'A': type=-1; fin++; break;
      case '-':
	tmp=get_flags(str,extract(q,t+1));
	str=tmp[0];
	args[e]=tmp[1..10000];
	t=4711;
	break;

      case 'R':
	tmp=explode_partially(str,q[t]);
	if(tmp)
	{
	  str=tmp[1];
	  args[e]=extra_get_object(tmp[0]);
	  if(!objectp(args[e])) err="No such room";
	}else{
	  if(!optional) err="Missing room name";
	}
	break;

      case 'F':
	if(strlen(q)>t+1)
	{
	  if(!sscanf(str,"%s"+extract(q,t+1)+"%s",a,str))
	    err="Syntax error";
	}else{
	  a=str;
	  str="";
	}
	if(strlen(a))
	{
	  if(type&T_POINTER)
	  {
	    args[e]=expand0(a);
	  }else{
	    args[e]=complete_file_name(a);
	  }
	  if(!args[e]) err="No such file";
	}else{
	  if(!optional) err="Missing filename";
	}
	break;

      case 'o':
	if(strlen(q)>t+1)
	{
	  if(!sscanf(str,"%s"+extract(q,t+1)+"%s",a,str))
	    err="Syntax error, can't have that here or something extra or missing before";
	}else{
	  a=str;
	  str="";
	}

	if(strlen(a))
	{
	  args[e]=present(a,environment(this_player()));
	  if(!args[e]) args[e]=present(a,this_player());
	  if(!args[e]) err="No such object";
	}else{
	  if(!optional) err="Missing object";
	}
	break;

      case 'd':
	if(strlen(q)>t+1)
	{
	  if(!sscanf(str,"%d"+extract(q,t+1)+"%s",args[e],str))
	    err="Syntax error";
	}else{
	  if(!sscanf(str,"%d",args[e]))
	    if(!optional)
	      err="Missing number";
	  str="";
	}
	break;

      case 's':
	if(strlen(q)>t+1)
	{
	  if(!sscanf(str,"%s"+extract(q,t+1)+"%s",args[e],str))
	    err="Syntax error";
	}else{
	  args[e]=str;
	  str="";
	}
	break;

      default:
	if(!sscanf(str,extract(q,t+1)+"%s",str))
	  err="Syntax error";
	break;
      }
    }
    if(fin==1)
    {
      tmp=explode_partially(str,q[t]);
      if(tmp)
      {
	str=tmp[1];
	args[e]=parse_string(tmp[0],type);
	if(!(typeof(args[e]) & type))
	{
	  switch(type)
	  {
	  case T_OBJECT: err="No such object"; break;
	  default: err="Wrong type of argument";
	  }
	}
      }else{
	if(!optional) err="Missing argument";
      }
    }
    if(many)
    {
      if(!err && strlen(str))
      {
	many+=({args[e]});
	e--;
      }else{
	args[e]=many;
	many=0;
      }
    }
  }
  if(err)
  {
    if(stringp(err))
    {
      notify_fail(com+" failed: "+err+".\n");
    }else{
      notify_fail(com+" failed.\n");
    }
  }else{
    obj=data[0];
    args+=({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,});
    if(!obj) return 0;
    if(obj==1) obj=this_object();
    obj->set_what_shell();
    if(mapflag==-1)
    {
      return (int)call_other(obj,data[1],args[2],args[3],args[4],args[5],args[6],args[7]);
    }else{
      tmp=args[mapflag];
      if(!pointerp(tmp)) tmp=({tmp});
      if(!sizeof(tmp))
	return notify_fail("No key argument to the command "+com+".\n"),0;
      for(e=0;e<sizeof(tmp);e++)
      {
	args[mapflag]=tmp[e];
	a=(string)call_other(obj,data[1],args[2],args[3],args[4],args[5],args[6],args[7]);
      }
      if(stringp(a)) write(a);
      return 1;
    }
  }
}

void reorganize(int doit)
{
  object tsp;
  int a;
  object *o,*oo;
  MYMAP flags;
  mixed dum;
  
  if(doit==17)
  {
    move_object(this_object(),environment());
    return;
  }
  if(!bearer) return;
  PROFILE("reorganize")
  running=EMPTY_MAP;  /* nothing can be running here */
  tsp=the_bearer();
  if(!tsp || environment()!=tsp || !interactive(tsp)) return;
  a=wizard;
  wizard=WIZARD_P(tsp);
  if(a<wizard)
  {
    help=share("help",wizard);
    actions=share("actions",wizard);
    dum=share("flags",wizard);
    flagtypes=dum[0];
    flags=copy_mapping(dum[1]);
    dum=m_indices(flagvalues);
    for(a=0;a<sizeof(dum);a++)
      flags=m_assign(flags,dum[a],flagval(dum[a]));
    flagvalues=flags;
    write("Your shell accepts your new rank of wizard.\n");
    write("Do 'shellhelp wizard' too see your new commands.\n");
#if NATIVE
    write("Don't forget to 'trust' the shell.\n");
#endif
    link_modules(DEFAULT_WIZMODULES,0);
    doit=1;
  }
#ifdef SCAN
  if(wizard && flagval("invscan"))
  {
    o=all_inventory(the_bearer());
    if(oldobjects)
    {
      oo=o-oldobjects;
      for(a=0;a<sizeof(oo);a++)
	write("Shell: new object in your inv: "+fast_desc(oo[a])+"\n");
    }
    oldobjects=o;
  }
#endif

  move_object(this_object(),tsp);
}

/* I hate the buggy type-checking... */
do_it(string str)
{
  string b,q;
  int a;
  mixed *d;

  if(shell_off == time()) return 0;

  PROFILE("do_it");

  switch(query_verb())
  {
  case "history":  return history(str);
  case "last":     return last(str);
  case "nickname": return nick(str);
  case "break":    return _break(str);
  }

  q=(str?str:"");

  switch(query_verb()[0])
  {
    /* history */
  case '%':
    PROFILE("history_cmd")
      return history_cmd(str);

  case '\\':
    PROFILE("override2")
    if(SAFECHECK()) return 0;
    shell_off=time();
    INTERNAL_COMMAND((query_verb()+" "+q)[1..1000000]);
    shell_off=0;
    return 1;
  }

  if(alias_off != time())
  {
    /* record history */
    if((!previous_object() || previous_object()==this_object()) &&
       history_off!=time())
      record_history(query_verb()+" "+q);

    /* aliases */
    if(b=m_assoc(aliases,query_verb()))
    {
      PROFILE("do_macro")
	return do_macro(b,q);
    }

    /* Nicknames */
    if(str && m_sizeof(nicknames) && (d=explode(q," ")))
    {
#if dworkins_mappings && alists
      d=intersect_alist(m_indices(nicknames),order_alist(({d}))[0]);
#else
      d=m_indices(nicknames) & d;
#endif
      if(sizeof(d))
      {
	PROFILE("nicknames")
	  q=" "+q+" ";
	a=sizeof(d)-1;
	while(a>=0)
	{
	  q=replace(q," "+d[a]+" ",m_assoc(nicknames,d[a--]));
	}
	my_command(query_verb()+q);
	return 1;
      }
    }

  }
  /* commands */
  if(d=m_assoc(verbmatch,query_verb()))
  {
    PROFILE("do_call:"+query_verb());
    return do_call(q,d,query_verb());
  }

  /* char-based commands */
  if(d=m_assoc(charmatch,query_verb()[0]))
  {
    PROFILE("do_call:"+query_verb()[0..0]);
    return do_call(extract(query_verb()+(str?" "+str:""),1),d,q[0..0]);
  }

  /* Normal commands */
  if(b=m_assoc(actions,query_verb()))
  {
    PROFILE(query_verb())
      return (int)call_other(this_object(),b,str);
  }
}

string query_auto_load()
{
#ifdef BRAINSAVE
  if(bearer && needsave) basepatch("save_it");
  needsave=0;
  return fnam+":";
#else
  return fnam+":"+implode(m_indices(aliases),"#")+"##"+
    implode(m_values(aliases),"#")+"##0#"+newsread+"#"+
      implode(m_indices(nicknames),"#")+"##"+
	implode(m_values(nicknames),"#")+"##"+
	  implode(m_indices(flagvalues),"#")+"##"+
	    implode(map_array(m_values(flagvalues),"postadd",this_object(),""),"#")+"##";
#endif
}

void init_arg(string str)
{
  string a,b,d,*c;
  int dum;
  mixed *q;
  if(newsread) return;
  if(!str || str=="") return;
  sscanf(str,"%s##%s##%s",a,b,d);
  if(d && d!="") sscanf(d,"%d#%d#%s",dum,newsread,d);
  call_out("checknews",1);
  aliases=mkmapping(my_explode(a,"#"),my_explode(b,"#"));
  if(m_assoc(aliases,"") || member_array("",m_values(aliases))>-1) aliases=EMPTY_MAP;
  if(d && d!="")
  {
    sscanf(d,"%s##%s##%s",a,b,d);
    nicknames=mkmapping(my_explode(a,"#"),my_explode(b,"#"));
    nicknames=my_clean_alist(nicknames);
    if(m_assoc(nicknames,"") || member_array("",m_values(nicknames))>-1) nicknames=EMPTY_MAP;
  }
  if(d && d!="")
  {
    sscanf(d,"%s##%s##",a,b);
    c=my_explode(a,"#");
    q=my_explode(b,"#");
    for(dum=0;dum<sizeof(c);dum++)
      if(m_assoc(flagtypes,c[dum]) & (T_STATUS | T_NUMBER)) q[dum]=atoi(q[dum]);
    flagvalues=mkmapping(c,q);
  }
  if(flagval("danger")<25) flagassign("danger",25);
}

void checknews()
{
  if(!newsread) newsread=last_news;
  if(newsread<last_news)
    write("You have unread news about the shell, use 'shellhelp news' to catch up.\n");
}

int defaults()
{
  int b;
  needsave=1;
  b=flagval("notify");
  flagassign("notify",0);
  macro("l look");
  if(iswizard())
  {
    macro("tail Tail");
    macro("cat Cat");
    macro("st shellstatus");
    macro("cleanid Undo -r env.id(\"%1\")");
    macro("clean Undo -r %1.all");
    macro("undo Undo -r %1");
    macro("apport Move %1");
    macro("deport Move %1 $back");
    macro("goto goin OBJ(%1).env");
    macro("In do -s goin -m %1,%@,goin -m $back");
    macro("see if %1 then I OBJ(%1).env else I env");
    macro("reset if %1 then patch %1 reset 1 else patch env reset 1");
/*  macro("frog do -s print -s %1->frog_curse(!%1->query_frog())?\"You turn %1 into a frog\":\"You turn %1 human again\"");
pp * uhh this EATS nodes......
 */
    macro("whosolved  print users.ffilter(x->query_quests(\"%1\"))");
  }else{
    macro("sc score");
    macro("five dotimes 5 %1");
    macro("ten dotimes 10 %1");
#if AA
    macro("mi cast magic missile");
    macro("bu cast burning hands");
    macro("co cast cone of cold");
    macro("dp drink potion");
    macro("tox do dotimes 3 buy %1,dotimes 3 drink %1,s,s,e,n,dotimes 3 sell bottle");
#else
    macro("tox do dotimes 3 buy %1,dotimes 3 drink %1,w,s,e,n,dotimes 3 sell bottle");
    macro("fi fireball");
    macro("mi missile");
    macro("sh shock");
#endif
    macro("df drink firebreather");
    macro("bf buy firebreather");
  }
  flagassign("notify",b);
  endwrite("Default aliases have been installed.\n");
}

string query_bearer() { return bearer; }

int mktable(string str)
{
  string *dum;
  mixed p,*grotesk,*blob;
  object tmp;
  int e,d,r,c,w;

  if(SAFECHECK()) return 0;
  if(str)
  {
    dum=my_explode_on_char(str,' ');
    r=sizeof(dum);
  }
  if(!str || !r)
    endwrite("Usage: mktable <array> <expression> <expression> ...\n");
  p=parse_string(dum[0],-1);
  if(!pointerp(p)) p=({p});
  grotesk=allocate(r);
  blob=allocate(r);
  grotesk[0]=p;
  c=sizeof(p);
  for(e=1;e<r;e++)
  {
    tmp=make_object("func(x) { return "+dum[e]+"; }\n");
    if(!tmp) return 0;
    grotesk[e]=map_array(p,"func",tmp);
    destruct(tmp);
  }
  for(e=0;e<r;e++)
  {
    grotesk[e]=map_array(grotesk[e],"fast_desc",this_object());
    for(d=0;d<c;d++)
    {
      w=strlen(grotesk[e][d]);
      if(w>blob[e]) blob[e]=w;
    }
  }
  for(e=0;e<r;e++)
    blob[e]="                                                       "[0..blob[e]];

  for(d=0;d<c;d++)
  {
    for(e=0;e<r;e++)
      write(grotesk[e][d]+extract(blob[e],strlen(grotesk[e][d])));
    write("\n");
  }
  return 1;
}

void update_me()
{ move_object(clone_object(fnam),the_bearer()); }

#if PROF
int see_profile()
{
  MYMAP sort;
  int e;
  sort=basepatch("get_profile");
  sort=mkmapping(m_values(sort),m_indices(sort));
  for(e=0;e<m_sizeof(sort);e++)
    write(left_format(m_values(sort)[e],25)+":"+m_indices(sort)[e]+"\n");
  return 1;
}
#endif

#ifdef PEOP
int peopledata(string str)
{
  string *q,data;
  int t,s,need;

  t=atoi(str);
  if(!t) t=24;
  need=t*((MAX_USERS>50?MAX_USERS:50)+11);
  s=file_size("/log/"+USERDATA);

  if(s<0)
  {
    write("No data.\n");
    return 1;
  }else{
    if(s>=need)
    {
      data=read_bytes("/log/"+USERDATA,s-need,need);
    }else{
      if(s>0) data=read_file("/log/"+USERDATA); else data="";
      s=file_size("/log/"+USERDATA+".old");
      need-=strlen(data);
      if(s>=need)
      {
	data=read_bytes("/log/"+USERDATA+".old",s-need,need)+data;
      }else{
	if(s>0) data=read_file("/log/"+USERDATA+".old")+data;
      }
    }
  }

  q=explode(data,"\n");
  if(sizeof(q)>t) q=q[sizeof(q)-t..sizeof(q)-1];
  write("Average number of people logged on the last "+(sizeof(q))+" hours:\n");
  write("        ^\n");
  for(s=0;s<sizeof(q);s++) write(q[s]+"\n");
  write("        +---------^---------^---------^---------^---------^---------^---->\n");
  write("        0         10        20        30        40        50        60\n");
  return 1;
}

int waitfor(string s)
{
  object o;
  string *ss;
  int e;
  if(!s) return notify_fail("Usage: waitfor <person>\n"),0;
  s=lower_case(s);
  ss=my_explode(s," ");
  for(e=0; e<sizeof(ss); e++)
  {
    if((o=find_player(ss[e])) && interactive(o))
    {
      write(capitalize(ss[e])+" is already on.\n");
    }else{
      basepatch("waitfor",ss[e]);
      write("Ok, waiting for "+capitalize(ss[e])+".\n");
    }
  }
  return 1;
}

#endif
#ifdef ECHO
int backecho(string s)
{
  if(!s || !flagval("echo")) return 0;
  write("You "+query_verb()+": "+s+"\n");
  return 0;
}

int echofnutt(string s)
{
  if(!s || !flagval("echo")) return 0;
  write("You say: "+s+"\n");
  return 0;
}

int echotell(string s)
{
  string a,b;
  if(!s || !flagval("echo") || !sscanf(s,"%s %s",a,b) ||
     !find_living(a)) return 0;
  write("You tell "+capitalize(a)+": "+b+"\n");
  return 0;
}
#endif

#ifdef TIME
int Time()
{
  write(ctime(time()+3600*(flagval("timezone")-TIME))+"\n");
  return 1;
}
#endif

link(string str)
{
  int e;
  string *a;
  string *others;
  if(SAFECHECK()) return 0;
  if(!str)
  {
    a=m_indices(modules);
    for(e=0;e<sizeof(a);e++)
      write(left_format(compress_file_name(a[e]),38)+": "+a[e]->short()+"\n");

    others=basepatch("register_module")-a;
    for(e=0;e<sizeof(others);e++)
      others[e]=compress_file_name(others[e]);

    if(sizeof(others))
    {
      write("Other modules available:\n");
      printarrayofstrings(others,0,flagval("columns"));
    }
    return 1;
  }
  str=complete_file_name(str);
  low_link(str);
  needsave=1;
  write("Ok.\n");
  return 1;
}

int unlink(string s)
{
  int e;
  string *d;
  mixed *q;
  if(SAFECHECK()) return 0;
  if(!s) return notify_fail("Usage: unlink filename\n"),0;
  s=complete_file_name(s);
  if(!m_assoc(modules,s))
  {
    write("No such modules linked.\n");
    return 1;
  }
  catch(s->expunge());
  if(s->query_clone_module()) dclean(find_object(s));
  modules=m_delete(modules,s);
  d=m_indices(verbmatch);
  q=m_values(verbmatch);
  for(e=0;e<sizeof(d);e++)
  {
    if(q[e][0]==s)
    {
      d=my_exclude_array(d,e);
      q=my_exclude_array(q,e);
      e--;
    }
  }
  verbmatch=mkmapping(d,q);
  d=m_indices(charmatch);
  q=m_values(charmatch);
  for(e=0;e<sizeof(d);e++)
  {
    if(q[e][0]==s)
    {
      d=my_exclude_array(d,e);
      q=my_exclude_array(q,e);
      e--;
    }
  }
  charmatch=mkmapping(d,q);
  write("Ok.\n");
  needsave=1;
  return 1;
}

static varargs void internal_add_command(string com,string fun,mixed ob)
{
  string a,*c;
  if(!ob) ob=1;
  c=my_explode(com,"%");
  a=c[0];
  c=c[1..1000];
  if(a[strlen(a)-1]==' ')
  {
    verbmatch=m_assign(verbmatch,a[0..strlen(a)-2],({ob,fun})+c);
  }else if(strlen(a)==1){
    charmatch=m_assign(charmatch,a[0],({ob,fun})+c);
  }else{
    verbmatch=m_assign(verbmatch,a,({ob,fun})+c);
  }
}

int shellcmds(string str)
{
  MYMAP modules;
  mixed *a,*b,*c,tmp;
  int e,d;

  if(SAFECHECK()) return 0;

  modules=EMPTY_MAP;

  a=m_indices(verbmatch);
  b=m_values(verbmatch);
  for(e=0;e<sizeof(a);e++)
  {
    if(!(c=m_assoc(modules,b[e][0])))
    {
      modules=m_assign(modules,b[e][0],a[e..e]);
    }else{
      modules=m_assign(modules,b[e][0],c+a[e..e]);
    }
  }
  modules["Base shell object"]=m_indices(actions);
  a=m_indices(modules);
  if(str && str!="") a=regexp(a,str);
  if(sizeof(a))
  {
    for(e=0;e<sizeof(a);e++)
    {
      if(e) write("\n");
      write("Commands in "+a[e]+".\n");
      printarrayofstrings(m_assoc(modules,a[e]),0,flagval("columns"));
    }
  }else{
    write("No such module linked.\n");
  }
  return 1;
}

void add_command(string com,string fun)
{
  if(my_file_name(previous_object())!=adder) return;
  internal_add_command(com,fun,my_file_name(previous_object()));
}

int islinked(object o)
{
  return m_assoc(modules,my_file_name(o)) && (object)o->this_shell()==this_object();
}

void module_put_data(mixed data)
{
  if(islinked(previous_object()))
  {
    modules=m_assign(modules,my_file_name(previous_object()),({data}));
    needsave=1;
  }
}

mixed module_get_data()
{
  mixed a;
  if(a=m_assoc(modules,my_file_name(previous_object())))
    if((object)previous_object()->this_shell()==this_object())
      return a[0];
}


string module_read_file(string file,int start, int end)
{
  if(!check_read_rights(file)) return 0;
  if(islinked(previous_object())) return read_file(file,start,end);
}

string module_read_bytes(string file,int start, int end)
{
  if(!check_read_rights(file)) return 0;
  if(islinked(previous_object())) return read_file(file,start,end);
}

int module_file_size(string file,int start, int end)
{
  if(!check_read_rights(file)) return 0;
  if(islinked(previous_object())) return file_size(file);
}

string *module_get_dir(string dir)
{
  if(!check_read_rights(dir))
  {
    write("Bad file name\n");
    return 0;
  }
  if(islinked(previous_object())) return get_dir(dir);
}

int module_command(string a) 
{
  if(!islinked(previous_object())) return 0;
  return my_command(a);
}

int query_prevent_shadow() { return 1; }
string query_brainbase() { return BRAINBASE; }

