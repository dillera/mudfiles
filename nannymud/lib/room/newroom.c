/* A _generic_ room
 * written by Profezzorn
 * documenation soon to be added.
 */

private mixed short_description;
private mixed long_description;
private string short_dirs="< >";
private string long_dirs="  There are no obvious exits\n";
private mapping cmds=([]);
private mapping properties=([]);
private mapping items=([]);

mixed eval(string s, string arg)
{
  mixed a,b,c;
  if(sscanf(s,"@%s->%s",a,b))
  {
    if(sscanf(b,"%s(%s",b,c)) c=c[0..strlen(c)-2];
    return (mixed)call_other(a,b,arg,c);
  }
  return s;
}

mixed query_property(string str)
{
  if(str)
  {
    return eval(properties[str],str);
  }else{
    return m_indices(properties);
  }
}

varargs mixed add_property(string str,int arg) { properties[str]=arg || 1; }
void remove_property(string s) {  properties=m_delete(properties,s); }

string short()
{
  return set_light(0) ? short_description + short_dirs : "In a dark room";
}

void init() { add_action("cmd","",1); }

int cmd(string arg)
{
  mixed q;
  if(q=cmds[query_verb()])
    return eval(q,arg);
}

/* DO NOT REDEFINE PLEASE */
void set_exit_strings(string long, string short)
{
  long_dirs=long;
  short_dirs=short;
}

nomask string *get_dirs()
{
  return map_array((explode(replace(short_dirs[1..strlen(short_dirs)-2]," ",""),",")||({})),"expand_dir",load_object("/room/newroomdaemon"));
}

void add_cmd(string cmd,mixed action) { cmds[cmd]=action; }
void remove_cmd(string cmd) { cmds=m_delete(cmds,cmd); }

void add_item(mixed it,string desc)
{
  int e;
  if(pointerp(it))
  {
    for(e=0;e<sizeof(it);e++)
      items[it[e]]=desc;
  }else{
    items[it]=desc;
  }
}

void remove_item(mixed it)
{
  int e;
  if(pointerp(it))
  {
    for(e=0;e<sizeof(it);e++)
      items=m_delete(items,it[e]);
  }else{
    items=m_delete(items,it);
  }
}

string query_long(string s)
{
  if(s)
    s=eval(items[s],s);
  else
    s=eval(long_description,0)+long_dirs;
  if(!s) return 0;
  return sprintf("%*-=s\n",this_player()->query_cols()-3,s);
}

nomask int id(string s) { return !!query_long(s); }
nomask void long(string s)
{
  if(s && this_player()->short())
    write(this_player()->query_name()+" looks at "+s+".\n");
  if(s=query_long(s)) write(s);
}

int inorout() { return query_property("inorout"); }
string realm() { return query_property("realm"); }

int can_put_and_get() { return 1; }
