/*
  A small daemon preforming some gathering of statistics...
  IT IS A CLUDGEY OBJECT!!!
 */

#define FILE "etc/statistics"

mapping full=([]);
mapping denied=([]);

int current_hour;
int current_quarter;
string current_day;

/*
  ([
  <weekday>:([<hour>:([<quarter>:({<mortal>,<wizard>}) ]) ])
  ])
 */

save()
{
  save_object(FILE);
}

restore()
{
  restore_object(FILE);
}

clean()
{
  string weekd,new;
  mapping current;

  while(remove_call_out("clean")>-1);

  call_out("clean",3600-time()%3600);

  new=atoi(ctime(time())[11..12]);
  
  if(new==current_hour)
    {
      return;
    }

  this_object()->query_quarter();
  
  weekd=query_week_day();
  current=full[weekd];
  if(!current||!mappingp(current))    
    current=([]);
  current[new]=([]);
  full[weekd]=current;
  current_hour=new;
  call_out("_clean",10);
}

query_week_day()
{
  string day;
  day=ctime(time())[0..2];
  if(day!=current_day)
    {
      this_object()->collect();
      current_day=day;
    }
  return current_day;
}

query_hour()
{
  if(!current_hour||!stringp(current_hour))
    current_hour = ctime(time())[11..12];
  return atoi(current_hour);
}

query_quarter()
{
  int q;
  q = (1+(atoi(ctime(time())[14..15]) / 15));
  if(q!=current_quarter)
    {
      this_object()->collect();
      current_quarter=q;
    }
  return q;
}

collect()
{
  string weekd;
  mapping current,tmp;
  mixed old,ind,tmpp;
  int wiz,mortals,i,hour,q;

  
  wiz=0;
  mortals=0;
  ind=m_indices(denied);
  for(i=0;i<sizeof(ind);i++)
    {
      tmpp=denied[ind[i]];
      if(tmpp&&pointerp(tmpp))
	{
	  if(tmpp[1]>19)
	    wiz++;
	  else
	    mortals++;
	}
    }


  if(mortals!=0||wiz!=0)
    {
      
      weekd=current_day;
      current=full[weekd];
      if(!current||!mappingp(current))    
	current=([]);
      
      hour=current_hour;
      tmp=current[hour];
      if(!tmp||!mappingp(tmp))
	current[hour]=([]);
        
      q=current_quarter;
      tmp=current[hour][q];
      current[hour][q]=({mortals,wiz});
      full[weekd]=current;
    }
  denied=([]); 
  save();
  this_object()->make_www_page();
}


add_new_to_full(string who, int level)
{
  mixed old;
  int q;

  q=query_quarter();
  old=denied[who];
  if(!old||!pointerp(old))
    old=({0,0,0});

  if((time()-old[2])>60)
    {
      tell_room(environment(this_object()),
		capitalize(who)+" tried to logon but failed ("+level+").\n");
      old[0]=old[0]+1;
      old[1]=level;
      old[2]=time();
    }
  else
    tell_room(environment(this_object()),
	      capitalize(who)+" polled ("+level+").\n");
  denied[who]=old;
    
  return old;
}

query_denied()
{
  return denied;
}

query_full()
{
  return full;
}

rm_full()
{
  full=([]);
}

postfix(i)
{
  switch(i)
    {
    case 1:
      return "1st";
    case 2:
      return "2nd";
    case 3:
      return "3rd";
    case 4:
      return "4th";
    default:
      return i;
    }
}

get_day_array()
{
  int p,i;
  mixed tmp,week;
  tmp = ({"Mon","Tue","Wed","Thu","Fri","Sat","Sun"});
  switch(current_day)
    {
    case "Mon":
      i=0;
      break;
    case "Tue":
      i=1;
      break;
    case "Wed":
      i=2;
      break;
    case "Thu":
      i=3;
      break;
    case "Fri":
      i=4;
      break;
    case "Sat":
      i=5;
      break;
    default:
    case "Sun":
      i=6;
      break;
    }
  week=allocate(7);
  p=0;
  while(p<7)
    {
      week[p]=tmp[i];
      p++;
      i++;
      if(i>6) i=0;
    }
  return week;
}

show()
{
  mapping quarters;
  mixed ind,day,hours,q,ply,arr;
  int i,o,p;

  write("Statistics over failed login attempts:\n");
  write("Current: "+current_day+", "+current_hour+" "+postfix(current_quarter)+
	" quarter. Size of denied: "+sizeof(m_indices(denied))+"\n");
  ind=get_day_array();
  for(i=0;i<sizeof(ind);i++)
    {
      day=full[ind[i]];
      if(day&&mappingp(day))
	{
	  write(sprintf("%s:\n",ind[i]));
	  hours=m_indices(day);
	  for(o=0;o<sizeof(hours);o++)
	    {
	      write(sprintf("%2d: ",hours[o]));
	      quarters=day[hours[o]];
	      q=m_indices(quarters);
	      for(p=0;p<sizeof(q);p++)
		{
		  arr=quarters[q[p]];
		  write(sprintf("%s qtr: %2d /%2d ",
				postfix(q[p]),arr[0],arr[1]));
		  if(p==sizeof(q)-1)
		    write(".");
		  else
		    write(", ");
		}
	      write("\n");
	    }      	
	  write("\n");      
	}
    }
}


make_www_page()
{
  mapping quarters;
  mixed ind,day,hours,q,ply,arr;
  int i,o,p;

  string file;

  
  file="";
  file+="Current time is: "+current_day+" "+current_hour+"\n";

  ind=get_day_array();
  
  for(i=0;i<sizeof(ind);i++)
    {
      day=full[ind[i]];
      if(day&&mappingp(day))
	{
	  file+=sprintf("%s:\n",ind[i]);
	  hours=m_indices(day);
	  for(o=0;o<sizeof(hours);o++)
	    {
	      file+=sprintf("%2d: ",hours[o]);
	      quarters=day[hours[o]];
	      q=m_indices(quarters);
	      for(p=0;p<sizeof(q);p++)
		{
		  arr=quarters[q[p]];
		  file+=sprintf("%s qtr: %2d /%2d",
				postfix(q[p]),arr[0],arr[1]);

		  if(p==sizeof(q)-1)
		    file+=".";
		  else
		    file+=", ";
		}
	      file+="\n";
	    }      	
	  file+="\n";      
	}
    }
  rm("/www/FULL");
  write_file("/www/FULL",file);
  "obj/master"->do_chmod("/www/FULL");
  tell_room(environment(this_object()),"Statisticsd says: I just made a WWW page!\n");
}



mapping grr=([]);

add(day,hour,q,arr)
{
  mapping current,tmp;

  current=grr[day];
  if(!current||!mappingp(current))    
    current=([]);

  tmp=current[hour];
  if(!tmp||!mappingp(tmp))
    current[hour]=([]);
  
  tmp=current[hour][q];
  current[hour][q]=arr;
  grr[day]=current;
}

_clean(int output)
{
  mapping quarters;
  mixed ind,day,hours,q,ply,arr;
  int i,o,p;

  grr=([]);
  
  ind=m_indices(full);
  for(i=0;i<sizeof(ind);i++)
    {
      day=full[ind[i]];
      hours=m_indices(day);
      for(o=0;o<sizeof(hours);o++)
	{
	  quarters=day[hours[o]];
	  q=m_indices(quarters);
	  for(p=0;p<sizeof(q);p++)
	    {
	      arr=quarters[q[p]];
	      if(arr[0]!=0||arr[1]!=0)
		{
		  add(ind[i],hours[o],q[p],arr);
		  if(output)
		    write("Include: "+ind[i]+":"+hours[o]+" "+q[p]+
			  " ("+arr[0]+"/"+arr[1]+")\n");
		}
	    }
	}      	
    }
  full=grr;
  grr=([]);
  call_out("make_www_page",10);
  return full;
}


/*
  Basic stuff below...
 */

void reset(int arg)
{
  if(_isclone(this_object())) 
    return destruct(this_object());
  move_object(this_object(),"/obj/daemon/daemonroom");  
  restore();
  clean(); 
}

string short()
{
  return "The Statistics daemon";
}

int id(string s)
{
  return s=="stat" || s=="daemon" || s=="statistics" || s==short();
}

void long() 
{
  write("This is a very pointless litle daemon.\n");
}

int query_prevent_shadow()
{
  return 1;
}

