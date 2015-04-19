mixed pairs;
static string file_used;
static mapping cache;
static int error;

#define DEBUG 
#define SAVEFILE "etc/marriages"

int equal(mixed a,mixed b)
{
  int e;
  if(a==b) return 1;

  if(pointerp(a))
  {
    if(!pointerp(b)) return 0;
    if(sizeof(a) != sizeof(b)) return 0;
    e=sizeof(a)-1;
    do {
      if(!equal(a[e],b[e])) return 0;
    } while(e--);
  }

  if(mappingp(a))
  {
    if(!mappingp(b)) return 0;
    return
      equal(m_indices(a), m_values(b)) &&
      equal(m_indices(a), m_values(b));
  }

  return 0;
}

static int safe_save_object(string s)
{
  mixed p,foo;

  catch(save_object(s)); 
  return 1;
  p=pairs;
  pairs=-17;
  catch(restore_object(s));
  foo=equal(p, pairs);
  pairs=p;
  if(!foo) rm(s+".o");
  return foo;
}

static void restore()
{
  if(!pointerp(pairs))
  {
    file_used=SAVEFILE;
    if(catch(restore_object(file_used)))
    {
      file_used=SAVEFILE+"_backup";
      if(catch(restore_object(file_used)))
      {
	write("Error restoring marriage database.\n");
	error=1;
	pairs=({});
      }else{
	if(safe_save_object(SAVEFILE))
	  file_used=SAVEFILE;
      }
    }
  }
}

static void do_save()
{
  if(!error)
  {
    if(file_used == SAVEFILE)
    {
      if(safe_save_object(SAVEFILE+"_backup"))
      {
	file_used=SAVEFILE+"_backup";
	if(safe_save_object(SAVEFILE))
	{
	  file_used=SAVEFILE;
	}
      }
    }else{
      if(safe_save_object(SAVEFILE))
      {
	file_used=SAVEFILE;
	safe_save_object(SAVEFILE+"_backup");
      }
    }
  }
}

void marry(string s1, string s2, string ini)
{
  restore();
  
  s1=lower_case(s1);
  s2=lower_case(s2);
  pairs+=({ ({s1, s2, ini, ctime(time()) }) });

  do_save();
  cache=0;
}

void divorce(string s)
{
  int e;
  restore();
  s=lower_case(s);
  for(e=0;e<sizeof(pairs);e++)
  {
    if(pairs[e][0]==s || pairs[e][1]==s)
    {
      pairs=pairs[0..e-1]+pairs[e+1..sizeof(pairs)];
      e--;
    }
  }
  do_save();
  cache=0;
}

#ifdef DEBUG
void read_spouses(string str)
{
  string f;
  string s1,s2,ini,when;
  int q;
  mapping wed;

  wed=([]);
  pairs=({});
  f=read_file("/log/marriages") + "\n";
  while(sscanf(f,"%s %*swith %s %*sThis wedding was initiated by %s.%*s(%s)\n%s",
	       s1,
	       s2,
	       ini,
	       when,
	       f)==8)
  {
    s1=lower_case(s1);
    s2=lower_case(s2);
    pairs+=({ ({s1,s2,lower_case(ini),when}) });
    if(wed[s1]) write(s1+" is married twice.\n");
    wed[s1]=1;
    if(wed[s2]) write(s2+" is married twice.\n");
    wed[s2]=1;
  }
  error=0;
  do_save();
  cache=0;
}

mixed query_cache() { return cache; }
#endif

mixed *query_spouse(string who)
{
  if(!cache)
  {
    int e;
    restore();
    cache=([]);
    for(e=0;e<sizeof(pairs);e++)
    {
      cache[pairs[e][0]]=({pairs[e][1],pairs[e][2],pairs[e][3]});
      cache[pairs[e][1]]=({pairs[e][0],pairs[e][2],pairs[e][3]});
    }
  }
  return cache[lower_case(who)];
}

void reset()
{
  cache=0;
}

void clean_up() { destruct(this_object()); }

mixed copy_array(mixed a) { return a+({}); }
mixed query_pairs()
{
  restore();
  return map_array(pairs, "copy_array", this_object());
}


int query_prevent_shadow() { return 1; }
