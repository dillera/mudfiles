inherit "/std/msg";

#define THIS "/std/rfc/roomlib"

string do_linebreak(string s, mixed arg)
{
  return line_break(s);
}

int cmd_moveto(string where)
{
  this_player()->move_player(where);
  return 1;
}

/* Hmm, I need to make automagic so you can't find 10000 objects in 
 * one reset...
 */

string item_find(string file,mixed arg)
{
  string desc;

  desc="You see nothing special.";
  
  sscanf(file,"%s\b%s",file,desc);
  
  if(arg!="query")
  {
    if(!previous_object()->query_property("temp_found_item_"+file))
    {
      object o;
      
      previous_object()->add_property("temp_found_item_"+file);
      if(sscanf(file,"a %s",file) || sscanf(file,"an %s",file))
      {
	o="/std/lib"->make(file);
      }else{
	o=clone_object(file);
      }
      if(transfer(o,this_player()))
	transfer(o,environment(this_player()));
      
      msg("\bPRON find\b$ "+(o->short() || "something") +".\n");
      
      return "";
    }
  }

  return desc;
}

int block_if_present(string who,mixed arg)
{
  if (present(who,previous_object()))
  {
    write("The "+who+" bars the way!\n");
    return 1;
  }
  return 0;
}

mixed null(mixed arg) { return arg; }

#define R replace
string powereval(string s, string argument)
{
  string *tmp,foo,arg,rest;
  int e,not;
  mixed yes;
#if 0
  write("powereval\n");
  write(s);
  write("----------\n");
#endif

  tmp=explode("-"+s+"\b","\b");
  tmp[0]=tmp[0][1..100000];
  for(e=1;e<sizeof(tmp);e++)
  {
    if(sscanf(tmp[e],"%s %s %s",foo,arg,rest)==3)
    {
      not=sscanf(arg,"not%s",arg);
      switch(foo)
      {
      case "isprop": yes=previous_object()->query_property(arg); break;
      case "ispresent": yes=present(arg,previous_object()); break;
      case "isid": yes=previous_object()->id(arg); break;
      case "isarg": yes=argument==arg; break;
      case "": yes=1; break;
      default: 
	rest=tmp[e];
	not=0;
	yes=1;
      }
      tmp[e]=not^!!yes?
	powereval(R(R(rest,"\\\\","\\"),"\\b","\b"),argument):
	"";
    }
  }
  return R(R(implode(tmp,""),"&lt;","<"),"&gt",">");
}

static string low_powerpre(string foo)
{
  string ret,*tmp;
  int e;

#if 0
  write("low_powerpre:\n");
  write(foo);
  write("------------------\n");
#endif

  tmp=explode("-"+foo+"<","<");
  ret=tmp[0][1..1000000];
  
  for(e=1;e<sizeof(tmp);e++)
  {
    string cond,arg,rest,t1,t2;
    int par,d;
    sscanf(tmp[e],"%s>%s",cond,rest);
    arg="";
    sscanf(cond,"%s %s",cond,arg);
    par=1;
    for(d=e+1;d<sizeof(tmp);d++)
    {
      if(tmp[d][0]=='/')
      {
	if(!--par) break;
      }else{
	par++;
      }
    }

    t2="";
    if(d<sizeof(tmp))
    {
      sscanf(tmp[d],"/%s>%s",t1,t2);
    }

    rest=low_powerpre(rest+implode(tmp[e+1..d-1],"<"));
    rest=R(R(rest,"\\","\\\\"),"\b","\\b");
    t2=low_powerpre(t2);
    t2=R(R(t2,"\\","\\\\"),"\b","\\b");
    ret+="\b"+cond+" "+arg+" "+rest+"\b  "+t2;
    e=d;
  }

  return ret;
}

string powerpre(string s)
{
  if(sscanf(s,"%*s<"))
  {
    return "@"+THIS+"->powereval("+low_powerpre(s)+")";
  }else{
    return R(R(s,"&lt;","<"),"&gt",">");
  }
}


string random_file(string tmp)
{
  string *t;
  t=explode(tmp," ");
  return t[random(sizeof(t))];
}
