/** Support functions **/

string *my_explode(string a, string b)
{
  string *data;
  data=explode("-"+a,b) || ({});
  data[0]=extract(data[0],1);
  return data;
}


/** Functions to build 'parse tree' **/
mixed *stack;

#define VAR(X) stack[X]
#define IS_READY VAR(0)
#define PREVIOUS VAR(1)
#define DATA     VAR(2)
#define NUM_VARS     3

void push()
{
  mixed tmp;
  tmp=stack;
  stack=allocate(NUM_VARS);
  PREVIOUS=tmp;
  DATA=({});
}

void start()
{
  stack=0;
  push();
}

int test()
{
  if(IS_READY)
  {
    stack=PREVIOUS;
    return 0;
  }else{
    return 1;
  }
}

void pop() { IS_READY=1; }

string fix_match(string m)
{
  string tmp;
  
  tmp="$0";
  sscanf(m,"%s\b%s",m,tmp);
  return (sizeof(my_explode(m,"%")) -1)+"\b"+m+"\b"+tmp;
}

string get_data() { return DATA; }

void add_call(mixed obj, string fun, mixed arg1, mixed arg2)
{
  DATA+=({ obj, fun, arg1, arg2 });
}

void add_call2(mixed obj, string fun, mixed arg1, mixed arg2)
{
  mixed tmp;
  tmp=stack;
  stack=PREVIOUS;
  add_call(obj,fun,arg1,arg2);
  stack=tmp;
}

/** Functions to parse 'parse tree' **/

#define INPUT data[0][0]
#define NAME data[0][1]
#define CURR data[0][2]
#define TP data[0][3]

string treat(string input,
	     string *data)
{
  int e;
  string *segments;

  segments=my_explode(input,"$");

  for(e=1;e<sizeof(segments);e++)
  {
    string tmp;
    int num,num2;
    int do_cap;

    tmp=segments[e];
    do_cap=sscanf(tmp,"c%s",tmp);

    if(sscanf(tmp,"%d%s",num,tmp))
    {
      num2=-1;
      sscanf(tmp,".%d%s",num2,tmp);

      tmp=data[num2][num]+tmp;

      if(do_cap) tmp=capitalize(tmp);
    }else if(!strlen(tmp)){
      tmp="$"+tmp;
    }

    segments[e]=tmp;
  }

  return implode(segments,"");
}

mixed do_block(mixed *data, mixed *rules)
{
  int e;

  while(e<sizeof(rules))
    if(call_other(rules[e++],rules[e++],data,rules[e++],rules[e++]))
      return 1;
}

mixed do_block2(mixed *data) { do_block(data[0], data[1]); }

int do_delay(mixed *data, int delay, mixed *rules)
{
  return call_out("do_block2",delay,({ data, rules }));
}

#define PROF find_player("profezzorn")
#define DEBUG(X) if(PROF) tell_object(PROF,X);

mixed do_match(mixed *data, string arg, mixed *rules)
{
  string val;
  string a,b,c, d,e,f;
  int matches;

  sscanf(arg,"%d\b%s\b%s",matches,arg,val);
  arg=treat(arg,data); /* We allow $ syntax in match strings! */

#if 0
  DEBUG("matches="+matches+".\n");
  DEBUG("data[-1][0]="+data[-1][0]+".\n");
  DEBUG("arg="+arg+".\n");
#endif
  if(matches ?
     (sscanf(data[-1][0],arg,a,b,c,d,e,f)==matches) :
     (data[-1][0]==arg) )
  {
    /* We have a winner */

    data+=({ ({0, a,b,c,d,e,f}) });
    data[-1][0]=treat(val,data);

    return do_block(data, rules);
  }

  return 0;
}

mixed do_random(mixed *data, int chance, mixed *rules)
{
  if(random(100) < chance) return do_block(data, rules);
  return 0;
}

int do_local_call(mixed *data, string fun, string extra)
{
  string ob;
  if(!extra) extra="";
  sscanf(fun,"%s\b%s",ob,fun);
  return call_other(ob, fun, treat(extra, data));
}

int do_call_back(mixed *data, string fun, string extra)
{
  if(!CURR) return 0;

  if(!extra) extra="";
  return call_other(CURR, fun, treat(extra, data));
}

int do_break() { return 1; }

int do_kill(mixed *data, string who)
{
  object o;
  if(!CURR) return 0;

  if(o=present(treat(who, data),environment(CURR)))
    if(CURR->query_attack()!=o)
      CURR->attack_object(o);

  return 0;
}

int do_msg(mixed *data, string message)
{
  if(!CURR) return 0;
  /* This might need catching or something */
  CURR->msg(treat(message,data), CURR, TP);
  return 0;
}

