/* Coded by Profezzorn, your friendly demon */

mapping locks;
static mapping temp_locks=([]);

#define SAVEFILE "etc/lockd"

#define NAME 0
#define STAT 1
#define REASON 2
#define TIME 3

void save(int now)
{
  if(find_object("/obj/shut") || now)
  {
    save_object(SAVEFILE);
  }else{
    if(-1!=find_call_out("save"))
    {
      remove_call_out("save");
      call_out("save",20,1);
    }
  }
}

mixed lock(string file,int temporary, int steal, string reason)
{
  string name;
  mixed stat, tmp;

  if(file[0]!='/') file="/"+file;

  if(!interactive(this_player()))
    return "Only players can lock files";

  if(!steal)
  {
    if((tmp=locks[file]) || (tmp=temp_locks[file]))
      return "File is locked by "+capitalize(tmp[NAME])+" "+
	ctime(tmp[TIME])+"\nreason: "+tmp[REASON]+"\n";
  }else{
    if(locks[file])
    {
      locks=m_delete(locks,file);
      save(0);
    }else{
      temp_locks=m_delete(temp_locks,file);
    }
  }

  stat=_file_stat(file);

  name=this_player()->query_real_name();

  tmp=({name, stat, reason, time()});
  if(temporary)
  {
    temp_locks[file]=tmp;
  }else{
    locks[file]=tmp;
    save(0);
  }
  
  tell_room(environment(),"Lock daemon says: "+capitalize(name)+" just locked "+file+".\n");

  return 0; /* success */
}

int lock_owner(mixed *tmp,string who)
{
  return tmp[NAME]==who;
}
mapping query_locks(string who)
{
  if(who)
  {
    return filter_mapping(locks,"lock_owner",this_object(),who);
  }else{
    return locks;
  }
}

mapping query_temp_locks(string who)
{
  if(who)
  {
    return filter_mapping(temp_locks,"lock_owner",this_object(),who);
  }else{
    return temp_locks;
  }
}

mixed check_lock(string file)
{
  string name, ret;
  mixed tmp, stat;

  if(file[0]!='/') file="/"+file;

  if(!interactive(this_player()))
    return "Only players can release locks\n";

  if(!(tmp=locks[file]) && !(tmp=temp_locks[file]))
    return "File is not locked\n";

  ret="";

  name=this_player()->query_real_name();
  if(name!=tmp[NAME])
    return "Lock was stolen by "+capitalize(tmp[NAME])+" "+
      ctime(tmp[TIME])+"\nreason: "+tmp[REASON]+"\n";

  stat=_file_stat(file);

  if(!stat)
  {
    if(tmp[STAT])
      ret+="File is gone.\n";
  }else{
    if(tmp[STAT])
    {
      if(stat[0] != tmp[STAT][0])
	ret+="File protection has changed.\n";

      if(stat[1] > tmp[STAT][1])
	ret+="File has grown.\n";

      if(stat[1] < tmp[STAT][1])
	ret+="File has shrunk.\n";

      if(stat[3] != tmp[STAT][3])
	ret+="File has been modified.\n";
    }else{
      ret+"File has been created.\n";
    }
  }

  if(strlen(ret)) return ret;
  return 0; /* success */
}


mixed release_lock(string file)
{
  string name;
  mixed tmp;

  if(file[0]!='/') file="/"+file;

  if(!objectp(this_player()) || !interactive(this_player()))
    return "Only players can release locks";

  if(!(tmp=locks[file]) && !(tmp=temp_locks[file]))
    return "File is not locked\n";

  name=this_player()->query_real_name();
  if(name!=tmp[NAME])
    return "Lock was stolen by "+capitalize(tmp[NAME])+" "+
      ctime(tmp[TIME])+"\nreason: "+tmp[REASON]+"\n";

  if(locks[file])
  {
    locks=m_delete(locks,file);
    save(0);
  }else{
    temp_locks=m_delete(temp_locks,file);
  }

  tell_room(environment(),"Lock daemon says: "+capitalize(name)+" just unlocked "+file+".\n");

  return 0; /* success */
}

mixed has_access(string file)
{
  string name;
  mixed tmp;

  if(file[0]!='/') file="/"+file;

  if(!interactive(this_player()))
    return "Only players can release locks";

  if(!(tmp=locks[file]) && !(tmp=temp_locks[file]))
    return 0;

  name=this_player()->query_real_name();
  if(name==tmp[NAME]) return 0;

  return "File is locked by"+capitalize(tmp[NAME])+" "+
    ctime(tmp[TIME])+"\nreason: "+tmp[REASON]+"\n";
}

void reset(int arg)
{
  if(!locks)
    catch(restore_object(SAVEFILE));
	  
  if(!locks)
    locks=([]);

  move_object(this_object(),"/obj/daemon/daemonroom");
}

string short() { return "a lock daemon"; }
int id(string s) { return s=="lockd" || s=="lock daemon" || s==short(); }
void long()
{
  write("The lock daemon is a grim fellow who takes cares of all locks\n");
  write("down here, he's got a whole sack full of them.\n");
}

int query_prevent_shadow() { return 1; }
