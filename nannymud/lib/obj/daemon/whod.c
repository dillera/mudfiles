#define FINGERFILE  "/etc/FINGERDATA"

static mapping whoregs=([]);
static mapping players=([]);

static object *callbacks=({});
static mapping named_callbacks=([]);

void add_callback(mixed ob)
{
  if(-1 != member_array(ob, callbacks)) return;
  callbacks-=({0});
  callbacks+=({ob});
}

void add_named_callback(string name,object o, mixed data)
{
  if(named_callbacks[name])
    named_callbacks[name]+=({o,data});
  else
    named_callbacks[name]=({o,data});
}

void register_player()
{
  object o;
  mixed *tmp;
  string name;
  int e;

  o=previous_object();
  if(!interactive(o)) return;
  players[name=(string)o->query_real_name()]=o;

  for(e=sizeof(callbacks)-1; e>=0; e--)
  {
    if(!callbacks[e] ||  catch(callbacks[e]->login_callback(o, name)))
      callbacks=callbacks[0 .. e-1] + callbacks[e+1 .. sizeof(callbacks)];
  }

  if(tmp=named_callbacks[name])
  {
    named_callbacks=m_delete(named_callbacks,name);
    for(e=0; e<sizeof(tmp); e+=2)
      if(tmp[e])
	catch(call_other(tmp[e], "login_callback", o, name, tmp[e+1]));
  }
  tell_room(environment(),"The who-daemon says: "+capitalize(name)+" just logged on.\n");
}

void register_logout()
{
  object o;
  mixed *tmp;
  string name;
  int e;

  o=previous_object();

  for(e=sizeof(callbacks)-1; e>=0; e--)
  {
    if(!callbacks[e] || catch(callbacks[e]->logout_callback(o, name)))
      callbacks=callbacks[0 .. e-1] + callbacks[e+1 .. sizeof(callbacks)];
  }

  if(tmp=named_callbacks[name])
  {
    named_callbacks=m_delete(named_callbacks,name);
    for(e=0; e<sizeof(tmp); e+=2)
      if(tmp[e])
	catch(call_other(tmp[e], "logout_callback", o, name, tmp[e+1]));
  }

  tell_room(environment(),"The who-daemon says: "+capitalize(name)+" just logged out.\n");
}

object find_player_or_statue(string name)
{
  return players[name];
}

mapping remove_map_value(mapping map,mixed val)
{
  mixed *a,*b;
  int e;

  e=member_array(val,b=m_values(whoregs));
  if(e<0) return map;
  a=m_indices(whoregs);
  do{
    a=a[0..e-1]+a[e+1..10000000];
    b=b[0..e-1]+b[e+1..10000000];
  }while((e=member_array(val,b))>=0);
  return mkmapping(a,b);
}


string plan_desc;
string name;
string title;
string al_title;
int level;
string last_login_time;
int gender;

static void setup_fake(object o)
{
  plan_desc=0;
  name=(string)o->query_real_name();
  title=al_title=0;
  if(title=(string)o->query_title())
  {
    sscanf(title,capitalize(name)+" %s",title);
    al_title=(string)o->query_al_title();
  }else if(title=(string)o->short()){
    sscanf(title,capitalize(name)+" %s (%s)",title,al_title);
  }
  if(!title || !al_title) return;
  level=(int)o->query_level();
  last_login_time=ctime(time())[4..15];
  gender=(int)o->query_gender();
  name=replace(name,".","");
  name=replace(name,"/","_");
  save_object("etc/fake_finger_save/"+name);
}

void register(object who)
{
  string n;
  if(!who->query_npc()) return;
  n=(string)who->query_real_name();
  if(file_size("/banish/"+n+".o")>0)
  {
    whoregs[n]=who;
    setup_fake(who);
  }
}

void unregister(object who)
{
  mapping old;
  whoregs=remove_map_value(old=whoregs,who);
  if(m_sizeof(old)>m_sizeof(whoregs))
     setup_fake(who);
}

object *query()
{
  return m_values(whoregs=remove_map_value(whoregs,0));
}

object find_player(string name)
{
  object o;
  if(o=find_player_or_statue(name)) return o;
  if(whoregs[name]) return whoregs[name];
  return 0;
}

string who_present_fun(object o)
{
  string s;
  if(o && (s=(string)o->short()))
    return capitalize(o->query_real_name())+":"+s;
}

make_uptime()
{
  int rt, dt, days, hours, minutes, seconds;
  string msg, *parts;

  rt = _driver_stat()[0];
  msg = "NannyMUD was last rebooted on " + ctime(rt) + ",\n";
  
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
  msg += "There are currently "+sizeof(users()-({0}))+" players on.\n";
  rm("/etc/uptime_info");
  write_file("/etc/uptime_info",msg);
  "obj/master"->do_chmod("/etc/uptime_info");
  return 1;
}


void make_fingerdata()
{
  mixed *list;
  string sh;
  int i, w, m;

  list = users() + query();
  
  if(list) 
  {
    list = sort_array( list, "who_sort_fun", this_object() );
    list = map_array( list, "who_present_fun",this_object() );
    sh=implode(list-({0}),"\n");

    if(file_size(FINGERFILE)>0 && !rm(FINGERFILE))
      _syslog("Failed to remove fingerfile.");
    else
      {
	write_file(FINGERFILE, sh + "\n");
	"/obj/master"->do_chmod(FINGERFILE);
      }
  }
  i = sizeof(list = users());
  while(i--)
  {
    if(list[i]->query_level() > 19)
      w++;
    else
      m++;
  }
  rm("/etc/number_logged_on");
  write_file("/etc/number_logged_on", sprintf("%d:%d\n", w, m));
  make_uptime();
}

void update_fingerdata()
{
  /* Get rid of command_giver */
  if(this_player())
  {
    enable_commands();
    disable_commands();
  }
  say("The statue seems to move.\n");
  remove_call_out("update_fingerdata");
  call_out("update_fingerdata", 60);
  make_fingerdata();
}
    
status who_sort_fun(object ob1, object ob2) 
{
  string name1, name2;
  if(!objectp(ob1) ||
     !(name1 = (string) ob1->query_real_name())) return 0;
  if(!objectp(ob2) ||
     !(name2 = (string) ob2->query_real_name())) return 0;
  return name1 > name2;
}

void reset(status arg)
{
   if(arg)
      return;
   if(_isclone(this_object()))
      return;
   call_out("update_fingerdata",1);
   move_object(this_object(),"/obj/daemon/daemonroom");
}

status id(string str)
{
  return (str == "finger daemon" ||
	  str == "fingerd" ||
	  str == "finger daemon statue" ||
	  str == "fingerd statue" ||
	  str == "statue" ||
	  str == "whod");
}

void long()
{
   write("This is a small grey stone statue of a daemon.\n");
   write("A small plate at the foot says: The Finger Daemon.\n");
}

string short() { return "a small grey stone statue"; }


int query_prevent_shadow() { return 1; }
