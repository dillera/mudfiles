#define BACKLOG 4096
#define SAVEFILE "etc/questd_save"

#define PERCENT_DYNAMIC 66
#define PERCENT_AUGMENTED 50

#define LOG_INTERVAL 2 * 24 * 60 * 60

static int ok;
string *array = allocate(BACKLOG);
mapping solved = ([]);
mapping quest_samples = ([]);
int ptr=0;

int last_log;

int do_log()
{
  int e;
  mixed *data;
  string s;

  if(last_log + LOG_INTERVAL < time())
  {
    s="";
    data="/room/quest_room"->query_quest_data();

    s="** Quest values on "+ctime(time())+" ("+time()+")\n";
    for(e=0;e<sizeof(data[0]);e++)
    {
      s+=data[0][e]+"#"+data[1][e]+"\n";
    }
    write_file("/log/QUEST_VALUES",s);

    last_log += LOG_INTERVAL;
    return 1;
  }
  return 0;
}

void safe_save_object()
{
  mixed foo;

  if(!ok) return;

  do_log();

  if(!catch(foo=save_object(SAVEFILE)))
  {
    save_object(SAVEFILE+".bak");
  }
}

/*
 * This function 'samples' all quests that are active right now, this is so
 * we can calculate the percentage correctly
 */

sample()
{
  object o;
  string *names;
  string *not_there;
  int e;

  names=all_inventory(load_object("/room/quest_room"))->query_name() - ({0});
  not_there = m_indices(quest_samples) - names;

  for(e=0;e<sizeof(names);e++)
  {
    if(quest_samples[names[e]]<BACKLOG)
      quest_samples[names[e]]++;
    else
      quest_samples[names[e]]=BACKLOG;
  }

  for(e=0;e<sizeof(not_there);e++)
  {
    if(quest_samples[not_there[e]]>1)
      quest_samples[not_there[e]]--;
    else
      quest_samples=m_delete(quest_samples,not_there);
  }
}

/*
 * This function is called whenever someone solves a quest
 * it remembers it in both a history list and a mapping.
 */
void solved(string who,string name)
{
  object w;

  tell_room(environment(),
	    "The quest-daemon says: "+capitalize(who)+" just solved "+name+".\n");

  w=find_player(lower_case(who));
  if(!w || w->query_level() > 19 || w->query_testchar())
    return;

  if(array[ptr])
  {
    if(--solved[array[ptr]] < 1)
      solved=m_delete(solved,array[ptr]);
  }
  solved[array[ptr++]=name]++;
  if(ptr >= sizeof(array)) ptr=0;

  sample();

  if(-1==find_call_out("safe_save_object"))
  {
    remove_call_out("safe_save_object");
    call_out("safe_save_object",2);
  }

  "/room/quest_room"->recalc_qp_later();
  users()->clear_cached_quest_points();
}

void reset(int a)
{
  int len,tmp;
  string f,*rows,_;

  if(a) return;

  if(_isclone(this_object())) destruct(this_object());

  move_object(this_object(),"/obj/daemon/daemonroom");

  if(!restore_object(SAVEFILE))
  {
    tell_room(environment(),"The questdeamon says: Oh my, I seem to have lost my notes.\n");
    if(!restore_object(SAVEFILE+".bak"))
    {
      tell_room(environment(),"The questdeamon says: Ack! I can't find them anywyere!\n");
      array = allocate(BACKLOG);
      solved = ([]);
      quest_samples = ([]);
      ptr=0;
      ok=0;
    }else{
      tell_room(environment(),"The questdeamon says: Yes, I found a backup!\n");
      safe_save_object();
      ok=1;
    }
  }else{
    ok=1;
  }
  if(sizeof(array) < BACKLOG)
    array += allocate(BACKLOG - sizeof(array));

  call_out("safe_save_object",2);
}

int id(string s)
{
  return s=="quest-daemon" ||
    s=="questdaemon" ||
      s=="questd" ||
	s=="quest daemon";
}

string short() { return "a quest daemon"; }

static int in_long;

int augment(string name,int done, int samples, int base, int old_qp)
{
  if(in_long == time())
    return done - samples - old_qp / 10;
  else
    return done - samples - old_qp;
}

#define SORT(X,Y) m_values(mkmapping((Y),(X)))


/*
 * This function returns an array that maps backwards, from # solved
 * to # qp.
 */
int *get_backwards(int *done, int *qp)
{
  int *ret, p;
  int delta_qp, delta_done,e;
  
  if(!sizeof(qp) || !sizeof(done)) return ({});
  
  qp=SORT(qp,qp);
  done=SORT(done,done);

  ret=allocate(qp[-1]+1);

  p=0;
  e=qp[0];
  while(e<qp[-1])
  {
/*    write("> e="+e+" done[-1]="+done[-1]+"\n"); */
    delta_qp=qp[p+1]-qp[p];
    if(delta_qp)
    {
      delta_done=done[~(p+1)]-done[~p];

#if 0
      write("qp[p] = "+qp[p]+"  qp[p+1]="+qp[p+1]+"\n");
      write("done[~p] = "+done[~p]+"  done[~(p+1)]="+done[~(p+1)]+"\n");
      write("delta_qp="+delta_qp+"  delta_done="+delta_done+"\n");
#endif
      for(;e<qp[p+1];e++)
      {
	ret[e]=done[~p]+ (e-qp[p]) * delta_done / delta_qp;
/*	write("ret["+e+"]="+ret[e]+".\n"); */
      }
    }
    p++;
  }
  ret[-1]=done[0];

  return ret;
}

int *get_quest_values(string *quest_names, int *old_quest_values)
{
  int e;
  int *done,*augmented_done;
  int *stat,*dynamic,*augmented;
  int *new, *backwards;
  mapping aug,dyn;
  int samps,total_samples, percent_dynamic;

  done=allocate(sizeof(quest_names));
  stat=allocate(sizeof(quest_names));
  dynamic=allocate(sizeof(quest_names));
  augmented=allocate(sizeof(quest_names));
  augmented_done=allocate(sizeof(quest_names));
  new=allocate(sizeof(quest_names));

  for(e=0;e<sizeof(quest_names);e++)
  {
    total_samples+=quest_samples[quest_names[e]];
    done[e]=solved[quest_names[e]];
  }
  if(total_samples)
    total_samples /= sizeof(quest_names);

  backwards=get_backwards(done, old_quest_values);

/*  write(sprintf("backwards=%O\n",backwards)); */
  for(e=0;e<sizeof(stat);e++)
  {
/*    write("e="+e+"\n"); */
    samps=quest_samples[quest_names[e]];
    dynamic[e]=old_quest_values[e]*PERCENT_DYNAMIC*total_samples/100/BACKLOG;

    stat[e]=old_quest_values[e]-dynamic[e];
    augmented[e]=dynamic[e]*PERCENT_AUGMENTED/100;
    dynamic[e]-=augmented[e];

/*    write("Done["+e+"]="+done[e]+"  old["+e+"]="+old_quest_values[e]+"  back="+backwards[old_quest_values[e]]+"\n"); */
    done[e]=(done[e]*samps+ backwards[old_quest_values[e]]*
	     (BACKLOG-samps));

    augmented_done[e]=-augment(quest_names[e],done[e] / BACKLOG ,quest_samples[quest_names[e]],BACKLOG,old_quest_values[e]);
    done[e]=-done[e]; 
  }
#if 0
  if(this_player() && this_player()->query_real_name() == "profezzorn")
    write(sprintf("1 = %O\n",SORT(dynamic,dynamic)));

  if(this_player() && this_player()->query_real_name() == "profezzorn")
    write(sprintf("2 = %O\n",SORT(quest_names,done)));

  if(this_player() && this_player()->query_real_name() == "profezzorn")
    write(sprintf("done = %O\n",done));

  if(this_player() && this_player()->query_real_name() == "profezzorn")
    write(sprintf("done = %O\n",quest_names));
#endif

  dyn=mkmapping(SORT(quest_names,done),SORT(dynamic,dynamic));
/*
  if(this_player() && this_player()->query_real_name() == "profezzorn")
    write(sprintf("dyn = %O\n",dyn));
*/
  aug=mkmapping(SORT(quest_names,augmented_done),SORT(augmented,augmented));

  for(e=0;e<sizeof(quest_names);e++)
  {
    new[e]=stat[e]+dyn[quest_names[e]]+aug[quest_names[e]];
#if 0
    if(this_player() && this_player()->query_real_name() == "profezzorn")
      write(sprintf("%2d: (%4d, %4d) %4d +%4d +%4d =%4d %s\n",e,done[e],augmented_done[e],stat[e],dyn[quest_names[e]],aug[quest_names[e]],new[e],quest_names[e]));
#endif
  }

  return new;
}

long()
{
  int sum, old_sum;
  int new_qp, *done, e;
  object o;
  mixed *data;
  string *quest_names, *old_quest_values;
 
  in_long=time();
  o=load_object("/room/quest_room");
  data=o->query_old_quest_data();
  quest_names=data[0]+({});
  old_quest_values=data[1];
  for(e=0;e<sizeof(old_quest_values);e++)
    old_quest_values[e]*=10;

  done=allocate(sizeof(quest_names));
  for(e=0;e<sizeof(quest_names);e++) done[e]=solved[quest_names[e]];

  new_qp=get_quest_values(quest_names,old_quest_values);

  quest_names=SORT(quest_names,done);
  old_quest_values=SORT(old_quest_values,done);
  new_qp=SORT(new_qp,done);
  done=SORT(done,done);

  write("Solved Old Qp New qp quest\n");
  for(e=0;e<sizeof(quest_names);e++)
  {
    sum+=new_qp[e];
    old_sum+=old_quest_values[e];
    write(sprintf("%-6d %4d %4d.%1d %s\n",
		  done[e],
		  old_quest_values[e]/10,
		  new_qp[e]/10,new_qp[e]%10,
		  quest_names[e]));
  }
  write(sprintf("Old Sum: %d, New Sum: %d.%d\n",old_sum/10,sum/10,sum%10));
  write(sprintf("Slots: %d, Filled slots: %d, ptr=%d\n",
		sizeof(array),
		sizeof(array-({0})),
		ptr));
  in_long=0;
}

void set_ok() { ok=1; }
query_prevent_shadow() { return 1; }
