#pragma strict_types
#include "brain.h"

#define BEARER ((string)call_other(this_player(),REAL_NAME_FUNC))

#ifdef STD_OBJECT
inherit STD_OBJECT;
#endif

#ifndef BRAINSHADOW
static string *pipes;	/* Used to retrive all output from a command */
#endif
void reset()
{
#ifdef STD_OBJECT
  ::reset();
#endif
#ifndef BRAINSHADOW
  pipes=({}); 
#endif
}

#if NATIVE
void create()
{
#ifdef STD_OBJECT
  ::create();
#endif
  reset();
}
#endif

string my_creator(object obj)
{
  string a,b,c;
  a=my_file_name(obj);
  if(!sscanf(a,"/"+WIZDIR+"/%s",b)) return (string)0;
  if(!sscanf(b,"%s/%s",b,c)) return (string)0;
  return b;
}

mixed *mid_array(mixed *arr, int a, int b)
{
  mixed *d;
  return (d=arr[a..b])?d:({});
}

mixed *my_exclude_array(mixed *a,int b)
{ return mid_array(a,0,b-1)+mid_array(a,b+1,sizeof(a)-1); }

void movem(object *what,object where)
{
  int dum;
  for(;dum<sizeof(what);dum++) if(what[dum]) move_object(what[dum],where);
}

/**** here I simulate or not simulate mappings  ****/
#if mappings
mapping m_assign(mapping map,string c,mixed d) { map[c]=d; return map; }
mixed m_assoc(mapping map,string c) { return map[c]; }
mapping copy_mapping(mapping a) { return a+([]); }
mapping order_mapping(mapping m) { return m; }
#if mappings==17
mapping mkmapping(mixed *indices,mixed *values)
{
  mapping tmp;
  int e;
  tmp=allocate_mapping(sizeof(indices));
  for(e=0;e<sizeof(indices);e++)
    tmp[indices[e]]=values[e];
  return tmp;
}
mixed *m_indices(mapping m) { return keys(m); }
mixed *m_values(mapping m) { return values(m); }
mapping m_delete(mapping m, mixed elem) { map_delete(m, elem);  return m; }
int m_sizeof(mapping m) { return sizeof(m); }
#endif
#else
#if alists
mixed *order_mapping(mixed *m) { return order_alist(m[0],m[1]); }
int m_sizeof(mixed *a) { return sizeof(a[0]); }
mixed *m_assign(mixed *map,string c,mixed d) { return insert_alist(c,d,map); }
mixed m_assoc(mixed *map,string c) { return assoc(c,map,0); }
mixed *mkmapping(mixed *b,mixed *c) { return order_alist(b,c); }
mixed *m_delete(mixed *c,string a)
{
  int b;
  b=assoc(a,c[0]);
  if(b<0) return c;
  return order_alist(my_exclude_array(c[0],b),my_exclude_array(c[1],b));
}
#else
mixed *order_mapping(mixed *m) { return m; }
int m_sizeof(mixed *a) { return sizeof(a[0]); }
mixed *m_assign(mixed *map,string c,mixed d)
{
  int e;
  if(-1==(e=member_array(c,map[0])))
     return ({map[0]+({c}),map[1]+({d})});
  map[1][e]=d;
  return map;
}
mixed m_assoc(mixed *map,string c)
{
  int e;
  if(-1==(e=member_array(c,map[0]))) return 0;
  return map[1][e];
}
mixed *mkmapping(mixed *b,mixed *c) { return ({b,c}); }
mixed *m_delete(mixed *c,string a)
{
  int b;
  b=member_array(a,c[0]);
  if(b<0) return c;
  return ({my_exclude_array(c[0],b),my_exclude_array(c[1],b)});
}
#endif
int mappingp(mixed *a) { return pointerp(a); }
mixed *m_indices(mixed *a) { return a[0]; }
mixed *m_values(mixed *a) { return a[1]; }
MYMAP copy_mapping(MYMAP a) { return ({a[0]+({}),a[1]+({})}); }
#endif

MYMAP my_clean_alist(MYMAP ass)
{
  mixed *b,*c;
  int a;
  b=m_indices(ass);
  c=m_values(ass);
  while(-1!=(a=member_array(0,c)))
  {
    b=my_exclude_array(b,a);
    c=my_exclude_array(c,a);
  }
  return mkmapping(b,c);
}

string my_extract(string a,int b,int c)
{
  if(b<=c && b>-1 && c<strlen(a)) return extract(a,b,c);
  return "";
}

string left_format(string str,int i)
{
  return my_extract(str+
"                                                                     ",0,i);
}

string right_format(string str,int i)
{
  str="                                                                     "+str;
  return extract(str,strlen(str)-i);
}

int typeof(mixed dum)
{
  if(intp(dum)) return T_NUMBER | T_STATUS;
  if(stringp(dum)) return T_STRING;
  if(objectp(dum)) return T_OBJECT;
  if(pointerp(dum)) return T_POINTER;
#if mappings
  if(mappingp(dum)) return T_MAPPING;
#endif
}
#ifndef PERFECT_EXPLODE
string *my_explode(string a,string b)
{
  string *c;
#if debug>10
  write("my_explode("+a+","+b+")\n");
#endif
/*  if(a=="") return ({}); */
  if(a==b) return ({"",""}); 
  if(strlen(b) && my_extract(a,0,strlen(b)-1)==b)
    return ({ "" })+ my_explode(my_extract(a,strlen(b),strlen(a)-1),b);
  if(c=explode(a+b,b)) return c;
  return ({});
}
#else
string *my_explode(string a,string b)
{
  return explode(a,b);
}
string *explode(string a,string b)
{
  if(strlen(b))
  {
    while(a[0..strlen(b)-1]==b)
      a=a[strlen(b)..strlen(a)-1];
    if(a[strlen(a)-strlen(b)..strlen(a)-1]==b)
      a=a[0..strlen(a)-strlen(b)-1];
  }
  return efun::explode(a,b);
}
#endif

int atoi(string str)
{
  int a;
  return ( stringp(str) ? ( sscanf(str,"%d",a),a ) : 0 );
}

int instr(string a,string b,int d)
{
#if debug>4
  write(sprintf("instr(%s,%s,%d)\n",a,b,d));
#endif

  if(!sscanf(my_extract(a,d,strlen(a)-1),"%s"+b,a)) return -1;
  return d+strlen(a);
}

string *stringmake(string a)
{
  /* start AFTER a " */
  int sl,qu,e;
  string st,ch;

#if debug>3
  write(sprintf("stringmake(%s)\n",a));
#endif

  e=0;
  st="";

  while((sl=instr(a,"\\",0))<(qu=instr(a,"\"",0)) && sl!=-1)
  {
    st+=my_extract(a,e,sl-1);
    switch(ch=my_extract(a,sl+1,sl+1))
    {
      case "t": st+="\t"; break;
      case "n": st+="\n"; break;
      default: st+=ch; break;
    }
    a=my_extract(a,sl+2,strlen(a)-1);
  }
  if(qu==-1) return ({a,st});
  st+=my_extract(a,e,qu-1);
  a=my_extract(a,qu+1,strlen(a)-sl);
  return ({a,st});
}

int quotesearch(string a,int e)
{
  /* start AFTER a " */
  int sl,qu;
  string st,ch;

#if debug>3
  write(sprintf("quotesearch(%s,%d)\n",a,e));
#endif

  sl=e-2;
  do
  {
    e=sl+2;
    qu=instr(a,"\"",e);
    sl=instr(a,"\\",e);
  }while(sl<qu && sl!=-1);
  if(qu==-1) return strlen(a);
  return qu;
}

string *my_explode_on_char(string str,int c)
{
  int e,parlvl;
  string *a;

#if debug>1
  write("explode on char:\""+str+"\"\n");
#endif

  if(!c) c=' ';
  a=({});
  for(;e<strlen(str);e++)
  {
    if(str[e]==c && !parlvl)
    {
      a+=({my_extract(str,0,e-1)});
      str=my_extract(str,e+1,strlen(str)-1);
      e=-1;
    }else{
      switch (str[e])
      {
        case '"':
          e=quotesearch(str,e+1);
          break;
        case '(':
	case '[':
	case '{':
	  parlvl++;
	  break;
	case '}':
	case ')':
	case ']':
	  parlvl--;
      }
    }
  }
  if(str!="") a+=({str});
  return a;
}

string *explode_partially(string str,int c)
{
  int e,parlvl;
  string *a;

  if(!c) c=' ';
  for(;e<strlen(str);e++)
  {
    if(str[e]==c && !parlvl)
    {
      return ({my_extract(str,0,e-1),my_extract(str,e+1,strlen(str)-1)});
    }else{
      switch (str[e])
      {
        case '"':
          e=quotesearch(str,e+1);
          break;
        case '(':
	case '[':
	case '{':
	  parlvl++;
	  break;
	case '}':
	case ')':
	case ']':
	  parlvl--;
      }
    }
  }
  if(str!="") return ({str,""}); 
  return 0;
}

/* returns ({string,status,status....}); */
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

#ifndef BRAINSHADOW
void write(mixed s)
{
  if(sizeof(pipes)) pipes[0]=pipes[0]+s;
  else if(this_player()) tell_object(this_player(),s);
}

static string popen(string fun,mixed a1,mixed a2,mixed a3,mixed a4)
{
  string s;
  pipes=({""})+pipes;
  if(sizeof(pipes)==1) 
    catch(call_other(this_object(),fun,a1,a2,a3,a4));
  else
    call_other(this_object(),fun,a1,a2,a3,a4);
  s=pipes[0];
  pipes=mid_array(pipes,1,1000);
  return s;
}
#else
static string popen(string fun,mixed a1,mixed a2,mixed a3,mixed a4)
{
  int t;
  string s;
  object shad;
#ifdef TRACE
  t=trace(0);
#endif
  shad=clone_object(BRAINSHADOW);
  if(!(shad->start_shadow(this_player(),this_object())))
    return "";
  catch(call_other(this_object(),fun,a1,a2,a3,a4));
  if(!shad) return "";
  s=(string)shad->query_text();
  if(!stringp(s)) s="";
  destruct(shad);
#ifdef TRACE
  trace(t);
#endif
  return s;
}
#endif

void printarrayofstrings(string *a,int flg,int termcols)
{
  int max,e,s,b,*t,cols,ss;
  string d;
  termcols--;
  if(!pointerp(a)) return;
  s=sizeof(a);

  switch(flg)
  {
  case 2: /* Linebreak with fill */
    for(ss=e=0;e<s;e++)
    {
      a[e]+=" ";
      cols+=strlen(a[e]);
      if(cols>termcols)
      {
	cols-=strlen(a[e]);
	while(cols<=termcols)
	  for(max=e-1;max>=ss && termcols >=cols;max--)
	  {
	    a[max]+=" ";
	    cols++;
	  } 
	for(;ss<e;ss++) write(a[ss]);
	write("\n");
	cols=strlen(a[e]);
      }
    }
    for(;ss<e;ss++) write(a[ss]);
    write("\n");
    return;

 case 1: /* Linebreak without ithout fill */
    for(e=0;e<s;e++)
    {
      b=strlen(a[e]);
      if((cols+=b+1)>termcols) {  write("\n"); cols=b+1;}
      write(a[e]+" ");
    }
    write("\n");
    return;

  case 0: /* Columns */
    t=allocate(s);
    for(e=0;e<s;e++)
    {
      b=t[e]=strlen(a[e]);
      if(b>max) max=b;
    }
    max++;
    cols=termcols/max;
    if(s>2)
      max=termcols/cols;
    d=extract("                                                       ",0,max-1);
    for(e=0,b=cols;e<s;e++)
    {
      write(a[e]+extract(d,t[e]));
      if(!(--b))
      {
	write("\n");
	b=cols;
      }
    }
    if(b!=cols) write("\n");
  }
}

void printformatted(string str,int termcols)
{
  int e;
  string *q;
  q=explode(str,"\n");
  for(;e<sizeof(q);e++)
    printarrayofstrings(my_explode(q[e]," "),1,termcols);
}

int query_prevent_shadow() { return 1; }
int query_allow_shadow() { return 0; }

nomask int isshell(object o)
{
  string dum;
  if(!o) return 0;
  dum=my_file_name(o);
  sscanf(dum,"%s#",dum);
  if(BRAINFILE==dum ||
     -1!=member_array(dum,DEFAULT_WIZMODULES)
#ifdef alt_name
     || dum==alt_name
#endif
     ) return 1;
  return 0;
}

string pathname(string file)
{
  string *hat;
  hat=my_explode(file,"/");
  if(sizeof(hat)<2) return "/";
  return implode(hat[0..sizeof(hat)-2],"/");
}

string basename(string file)
{
  while(sscanf(file,"%s/%s",file,file));
  return file;
}

static int issafe;

varargs static mixed basepatch(string what,mixed a,mixed b)
{
  mixed tmp;
#if NATIVE
  string id;
  if(!isshell(this_object())) return;
  id=geteuid();
  seteuid(getuid());
  issafe=1;
  tmp=(mixed)call_other(BRAINBASE,what,a,b);
  seteuid(id);
#else
  if(!isshell(this_object())) return;
  issafe=1;
  tmp=(mixed)call_other(BRAINBASE,what,a,b);
#endif
  issafe=0;
  return tmp;
}

int query_issafe() { return issafe; }

string nice_period(int a)
{
  string *b;
  int c;
  b=({});
  if(c=a%60) b=({c+" sec"})+b;
  if(a/=60)
  {
    if(c=a%60) b=({c+" min"})+b;
    if(a/=60)
    {
      if(c=a%24) b=({c+" hour"+(1==c?"":"s")})+b;
      if(a/=24)
      {
	if(c=a%7) b=({c+" day"+(1==c?"":"s")})+b;
	if(a/=7)
	{
	  b=({a+" week"+(1==a?"":"s")})+b;
	}
      }
    }
  }
  return implode(b," ");
}

string short_time(int t)
{
  /* 6 * 30 * 24 * 60 * 60 = 15552000 */
  if(time()-t>15552000) return ctime(t)[4..10]+ctime(t)[20..23]+" ";
  if(time()<t) return "The Future  ";
  return ctime(t)[4..15];
}

varargs string implode_nicely(string *dum,string del)
{
  int s;
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
}

string find_function(string fun,object o)
{
  string tmp;
  if(!o) return 0;
  if(fun=="heart_beat") return my_file_name(o);
  if(tmp=find_function(fun,shadow(o,0))) return tmp;
  if(tmp=function_exists(fun,o)) return tmp;
  return 0;
}

string compress_file_name(string a)
{
  string b,c;
  if(!sscanf(a,"/"+WIZDIR+"/%s",b)) return a;
  if(!sscanf(b,"%s/%s",b,c)) return a;
  if(b==BEARER) return "~/"+c;
  return "~"+b+"/"+c;
}

string short_file_name(object o)
{
  return compress_file_name(my_file_name(o));
}

string complete_file_name(string f)
{
  string *p,t;
  int w;

  switch(f[0])
  {
  default:
    t=(string)this_player()->query_path();
    if(!t) t="/"+WIZDIR+"/"+BEARER+"/";
    if(!strlen(t) || t[0]!='/') t="/"+t;
    if(t[strlen(t)-1]!='/') t+="/";
    f=t+f;
    break;
  case '~':
    if(f[1]=='/')
      f="/"+WIZDIR+"/"+BEARER+"/"+my_extract(f,2,strlen(f)-1);
    else
      f="/"+WIZDIR+"/"+my_extract(f,1,strlen(f)-1);
  case '/': 
  }
  p=my_explode(f,"/");
  p -= ({"."});
  while(-1!=(w=member_array("..",p)))
    p=mid_array(p,0,w-2)+mid_array(p,w+1,sizeof(p)-1);
  return implode(p,"/");
}

object master_of(object o)
{
  string f;
  if(sscanf(f=file_name(o),"%s#",f))
    return find_object(f);
  return o;
}

string upper_case(string str)
{
  int i,len;
  mixed tmp;

  if(!stringp(str))
    return 0;
  
  tmp = "";
  len = strlen(str);

  for(i=0; i < len; i++)  tmp += capitalize(str[i..i]);
  return tmp;
}

object *get_all_clones(object o)
{
#ifdef CHILDREN
  object m;
  string f;
  m=master_of(o);
  if(m)
    return ({m})+children(m);
  else
    return children(o);
#else
#ifdef NEXT_CLONE
  object m,*ret;
  string f;
  int e;
  m=master_of(o);

  if(m)
    o=m;
  else
    m=o;
  ret=allocate(100);
  while((o=NEXT_CLONE(o)) && o!=m)
  {
    if(e==sizeof(ret)) ret+=allocate(sizeof(ret));
    ret[e++]=o;
  }
    
  return ret[0..e-1];
#else  
  return 0;
#endif
#endif
}

int binsearch(int a,int *b)
{
#if alists
  return insert_alist(a,b);
#else
  /* Ok, not a binsearch, but it works... */
  int i;
  for(i=0;i<sizeof(b) && a>b[i];i++);
  return i;
#endif
}

string read_line_cached(string fil,int i,mixed *cache)
{
  string a;
  if(!cache[0] || i<cache[0][0] || i>=cache[0][0]+sizeof(cache[0]))
  {
    i&=(-1 | 128);
    if(i<1) i=1;
    a=read_file(fil,i,128);
    if(!a || a=="\n") return 0;
    cache[0]=({i-1})+explode(a,"\n");
    if(!cache[0] || i<cache[0][0] || i>=cache[0][0]+sizeof(cache[0])) return 0;
  }
  return cache[0][i-cache[0][0]];
}

int *query_age(object obj)
{
  int i;
#ifdef OBJECT_CREATED
  i=OBJECT_CREATED(obj);
  return ({i,i});
#else
#ifdef PEOP
  string d;
  int c,cc,*top, q;
  
  if(sscanf(my_file_name(obj),"%s#%d",d,c))
  {
    top=share("timedata",wizard);
    if(sizeof(top[0]))
    {
      i=binsearch(c,top[1]);

      if(!i)
      {
	return ({-1, top[0][0] });
      }
      else if(i==sizeof(top[0]))
      {
	return ({ top[0][i-1], -1 });
      }else{
	return top[0][i-1 .. i];
      }
    }
  }else{
    cache=({0});
    my_command("dumpallobj");
    top=share("timedata",wizard);
    sscanf(d=my_file_name(obj),"/%s",d);
    d="^"+d+" ";

    /* coarse search */
    for(i=1;(b=read_file(OBJ_DUMP,i,128)) && !sizeof(regexp(my_explode(b,"\n"),d));i+=128);
	
    sscanf(d=my_file_name(obj),"/%s",d);
    d+="%s";

    /* Fine search */
    for(;(b=read_line_cached(OBJ_DUMP,i,cache)) && !sscanf(b,d,b);i++);

    if(b)
    {
      /* Search backwards for previous clone */
      for(q=i-1;q>=1 && (b=read_line_cached(OBJ_DUMP,q,cache)) && sscanf(b,"%s ",b) && !sscanf(b,"%s#%d",b,c);q--);
      i=binsearch(cc,top[1]);
      if(i<sizeof(top[0])) cc=top[0][i]; else cc=-1;

      /* Search forward for next clone */
      for(q=i+1;(b=read_line_cached(OBJ_DUMP,q,cache)) && sscanf(b,"%s ",b) && !sscanf(b,"%s#%d",b,cc);q++);
      i=binsearch(c,top[1]);
      if(i>0) c=top[0][i-1]; else c=-1;


      return ({ cc, c});
      
    }
  }
  
#else
  return ({-1,-1});
#endif
#endif
}
