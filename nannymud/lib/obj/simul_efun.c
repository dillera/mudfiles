#pragma strict_types
#pragma save_types

mapping profiled = ([ ]);

#define PROFILING /* Milamber */

#ifdef PROFILING
#define PROFILE(X) profiled["X"]++
mapping query_simulefun_profile_info()
{
  PROFILE(query_simulefun_profile_info);
  return profiled + ([ ]);
}
#else
#define PROFILE(X)
#endif

int query_prevent_shadow()  { return 1;} /* Lpd 920120 */

varargs string line_break(string s,string pre,int width)
{
  int e;
  string done,a,b;

  PROFILE(line_break);

  if(!stringp(pre)) pre="";

  if(!width)
  {
    if(this_player())
      width=this_player()->query_cols();

    if(!width)
      width=80;

    width-=strlen(pre)+2;
  }

  done="";
  while(strlen(s))
  {
    e=5000;
    if(strlen(s)>=5000)
    {
      e=5000;
      while(e>=0 && s[e]!=' ') e--;
      if(e==-1) return done+"   "+s+"\n";
    }
    done+=sprintf("%s%-=*s",pre,width,s[0..e-1]);
    pre="                                                                                                               "[1..strlen(pre)];
    s=s[e+1..strlen(s)-1];
    if(!strlen(s))
    {
      if(done[-1] != '\n') done+="\n";
      return done;
    }
    for(e=strlen(done)-1;e>=0 && done[e]!='\n';e--);

    a=0;
    b="";
    sscanf(s,"%s %s",a,b);
    while(a && strlen(a)+strlen(done)-e<=width+2)
    {
      done+=" "+a;
      s=b;
      a=0;
      b="";
      sscanf(s,"%s %s",a,b);
    }
    done+="\n";
  }
  return done;
}

string big_linebreak(string s,int width)
{
  log_file("FIX", "big_linebreak: "+file_name(previous_object())+"\n");
  PROFILE(big_linebreak);
  if(!width) width=79;
  return line_break(s,"",width);
}

/*
 * filter_objects() has been renamed to filter_array().
 */
mixed *filter_objects(mixed *list, string str, object ob, mixed extra)
{ 
  log_file("FIX", "filter_objects: "+file_name(previous_object())+"\n");
  PROFILE(filter_objects);
  return filter_array(list, str, ob, extra);
}

/*
 * The old 'slice_array' is no longer needed. Use range argument inside
 * a pair of brackets instead.
 */
mixed *slice_array(mixed *arr, int from, int to)
{
  PROFILE(slice_array);
  log_file("FIX", "slice_array: "+file_name(previous_object())+"\n");
  return arr[from..to];
}

string substitute_string(string str, string from, string to, int all)
{
  string tmp1, tmp2, ret;
  log_file("FIX", "substitute_string: "+file_name(previous_object())+"\n");

  PROFILE(substitute_string);
  if(all) return replace(str,from,to);


  if(sscanf(str, "%s" + from + "%s", tmp1, tmp2) == 2)
    str = tmp1 + to + tmp2;
  return str;
}

string itoa(int i)  
{ 
  PROFILE(itoa);
  return ""+i; 
}

object load_object(mixed s)
{
  PROFILE(load_object);
  if(objectp(s)) return s;
  call_other(s,"?");
  return find_object(s);
}

string query_ip_name(mixed arg)
{
  string ip_num;

  PROFILE(query_ip_name);

  if(stringp(arg))
    ip_num = arg;
  else if(objectp(arg))
    ip_num = query_ip_number(arg);
  else 
    ip_num = query_ip_number(previous_object());
  
  return (string)"/obj/daemon/dnsd"->query_hostname(ip_num);
}

string _query_ident(object ob)
{
  PROFILE(_query_ident);
  if(!ob) ob=previous_object();
  return (string) "/obj/daemon/identd"->query_ident(ob);
}


string implode_nicely(string *dum,string del)
{
  int s;
  PROFILE(implode_nicely);
  if(!del) del="and";
  switch(s=sizeof(dum))
  {
   default:
    return implode(dum[0..s-2],", ")+" "+del+" "+dum[s-1];
   case 2:
    return dum[0]+" "+del+" "+dum[1];
   case 1:
    return dum[0];
   case 0:
    return "";
  }
} /* implode_nicely */


#define to_m(X) ((X)[0]*1000+(X)[1]/1000000)


mapping mapping_rusage()
{
  mapping u;
  mixed *s;

  PROFILE(mapping_rusage);
/*  log_file("FIX", "mapping_rusage: "+file_name(previous_object())+"\n"); */

  u=([]);
  s=_rusage();
  u["minflt"] = s[0];
  u["majflt"] = s[1];
  u["nswap"] = s[2];
  u["inblk"] = s[3];
  u["outblk"] = s[4];
  u["msnd"] = s[5];
  u["srcv"] = s[6];
  u["sigs"] = s[7];
  u["vctx"] = s[8];
  u["ictx"] = s[9];
  u["sysc"] = s[10];
  u["ioch"] = s[11];
  u["rtime"] = to_m( s[12] );
  u["utime"] = to_m( s[13] );
  u["stime"] = to_m( s[14] );
  u["ttime"] = to_m( s[15] );
  u["tftime"] = to_m( s[16] );
  u["dftime"] = to_m( s[17] );
  u["ktime"] = to_m( s[18] );
  u["ltime"] = to_m( s[19] );
  u["slptime"] = to_m( s[20] );
  u["wtime"] = to_m( s[21] );
  u["stoptime"] = to_m( s[22] );
  u["brksize"] = s[23];
  u["stksize"] = s[24];
  return u;
}

string expand_file_name(string s)
{
  mixed tmp,i;

  PROFILE(expand_file_name);

  if(sscanf(s,"an %*s") || sscanf(s,"a %*s"))
    return s;
  
  switch(s[0])
  {
  case '@': return s;
  case '/': 
    tmp=explode(s,"/");
    break;

  case '~':
    if(s[1]=='/')
      s="/players/"+creator(previous_object())+s[1..strlen(s)];
    else
      s="/players/"+s[1..strlen(s)];
    tmp=explode(s,"/");
    break;

  default:
    tmp=explode(file_name(previous_object()),"/");
    tmp[-1]=".";
    tmp+=explode(s,"/");
  }

  tmp-=({".",""});
  while(-1!= (i=member_array("..",tmp)))
  {
    tmp=tmp[0..i-2]+tmp[i+1..100000];
  }
 
  return "/"+implode(tmp,"/");
}

int set_light(int l)
{
   mixed val;
   object obj;

   PROFILE(set_light);

   l = efun::set_light(l, previous_object());
   if(this_player())
   {
#if 0
      if(environment(this_player()) &&
	 (environment(this_player()) == environment(previous_object()) ||
	  environment(this_player()) == previous_object()) &&
	 environment(this_player())->query_property("own_light_code"))
      {
	 l = (int) environment(this_player())->query_light(l);
      }
#else
      for(obj = previous_object(); obj; obj = environment(obj))
      {
	/* A value of 0 will be treated as no value, tough luck */
	 if(!(val = (mixed) obj->query_property("own_light_code")))
	    continue;
	 if(intp(val))
	    l = val;
	 else if(stringp(val))
	    l = (int) call_other(obj, val, l);
	 else
	 {
	    throw("Bad type of 'own_light_code' property value for \"" +
		  file_name(obj) + "\".\n");
	 }
	 break;
      }
#endif
      if(l<1)
      {
	 if(this_player()->query_property("night_vision"))
	    l = 1;
      }
      else
      {
	 if(this_player()->query_property("blinded"))
	    l = 0;
      }
   }
   return l;
}

object *_get_all_obj()
{
   PROFILE(_get_all_obj);
   return _get_spec_obj(0, 0);
}

object _next_object(object obj)
{
   object *all;
   int i;
   PROFILE(_next_object);
   log_file("FIX", "_next_object: "+file_name(previous_object())+"\n");

   if((i = member_array(obj, all = _get_all_obj())) == -1)
      return 0;
   else if(++i >= sizeof(all))
      return all[0];
   else
      return all[i];
}

object _prev_object(object obj)
{
   object *all;
   int i;
   PROFILE(_prev_object);
   log_file("FIX", "_prev_object: "+file_name(previous_object())+"\n");
   if((i = member_array(obj, all = _get_all_obj())) == -1)
      return 0;
   else if(--i < 0)
      return all[-1];
   else
      return all[i];
}


varargs int *frnd(int max, int min, int nr, int seed)
{
  /* A fast and dirty random generator for integers.
   * Based on FORTRAN-77.
   * Period is only about 2**30.
   *
   * BEWARE: There are no sanity checks on things here,
   * as that would slow it down and LPC is slow enough as
   * it is. It is YOUR responsibility to give valid values.
   *
   * max is the max numberr the generator will give,
   * min is the smallest,
   * nr is the size of the returned array and
   * seed is the starting seed. seed should be in between
   * min and max.
   */

  int i;
  int rr, *tmp;

  tmp = ({ });
  rr = seed;
  for (i=0; i<nr; i++)
  {
    rr = (rr*8121 + 28411) % 134456;
    tmp += ({ min + (max - min +1)*rr/134456 });
  }
  return tmp;
} /* frnd */
