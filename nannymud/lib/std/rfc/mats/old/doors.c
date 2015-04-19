/* doors.c
   Written by Mats   */

inherit "/std/room";


/*
 * Should be in basic.c
 *
 */

private string *extra_longs;

varargs string my_query_long(string s)
{
  /*  if(items)
  {
    if(items[s])
      return eval(items[s],s);
  } 
  s = eval(long_desc); */
  s = "In this version it is not possible to set_long, because\n"+
    "basic.c is not updated with extra_longs.\n";
  if(extra_longs) {
    int i;
    for(i=0;i<sizeof(extra_longs);i++)
      s += eval(extra_longs[i]);
  }
  return s;
}

void add_extra_long(string l)
{ 
  if (!extra_longs) extra_longs = ({});
  if (member_array(l,extra_longs) == -1)
    extra_longs += ({l});
}

void remove_extra_long(string l)
{
  extra_longs -= ({l});
}

query_extra_longs() { return extra_longs; }

/*
 * Add a dependant room , could be in basic_room.c
 *
 */

private string *depend_rooms = ({});

void add_depend(string room)
{
  room = expand_file_name(room);
  load_object(room);
  depend_rooms += ({room});
}

void remove_depend(string room)
{
  int i;
  i=member_array(room,depend_rooms);
  if (i == -1) return;
  depend_rooms = depend_rooms[0..i-1]+depend_rooms[i+1..0x7ffffff];
}

string *query_depend_rooms() { return depend_rooms; }
 
int clean_up(int refs)
{
  int e;
  for(e=0;e<sizeof(depend_rooms);e++) {
    if (find_object(depend_rooms[e])) return 1;
  }
  ::clean_up(refs);
}

/**  end of additions **/


#define ERROR(X) tell_object(find_player(creator(this_object())||"")||this_player()||this_object(),"ERROR "+(X))

mapping doors=([]);
mapping locks=([]);
int nr_doors=0;
int *opened=({});
string *doordir=({});
mapping reversedir=([
  "north": "south",
  "south": "north",
  "east": "east",
  "west": "west",
  "up": "down",
  "down": "up",
  "out": "in",
  "in": "out",
  "southeast": "northwest",
  "southwest": "northeast",
  "northeast": "southwest",
  "northwest": "southeast"]);

void make_door_here(string name,string arg, int w)
{
  doors[name]=w?-1:nr_doors;
  add_item(name,w?"Which door?":("@door_desc("+arg+")"));
  add_item_cmd(({"open","close"}),name,w?"Which door?\n":("@open_door("+arg+")"));
}

void make_door_there(string name, string arg, string where, string from,int w)
{
  mixed p;
  where->add_item(name,w?"Which door?":("@/"+from+"->door_desc("+arg+")"));
  where->add_item_cmd(({"open","close"}),name,w?"Which door?\n":("@/"+from+"->open_door("+arg+")"));
  if (w)
    p = -1;
  else
    p = this_object();
  where->add_property(name,p);
}

void make_door(string name,string *arg)
{
  if (id(name)) {
    ERROR("Door \""+name+"\" already defined!\n");
  }
  make_door_here(name,arg[0],0);
  make_door_there(name,arg[0],arg[1],arg[2],0);
}

/*
unmake_door(string name,string arg)
{
  _m_delete(doors,name);
  remove_item(name);
  remove_item_cmd(({"open","close"}),name);
  arg->remove_item(name);
  arg->remove_item_cmd(({"open","close"}),name);
} */

varargs void add_door(string dir, mixed where, mixed names)
{
  string from;

  where = objectp(where) ? "/"+file_name(where) : expand_file_name(where);
  where = where[1..strlen(where)];
  from = file_name(this_object());

  if (pointerp(names))
    names -= ({"door",dir+" door"});
  else
    names = ({names});

  map_array(names,"make_door",this_object(),({names[0],where,from}));
  names = names[0];
  make_door_here(dir+" door",names,0);
  make_door_there(reversedir[dir]+" door",names,where,from,0);
  make_door_here("door",names,id("door"));
  make_door_there("door",names,where,from,where->id("door"));

  add_exit(dir,"@check_door("+where+"#"+names+")",dir+", through the "+names);
  add_extra_long("@door_long("+names+")");
  where->add_exit(reversedir[dir],
	  "@/"+from+"->check_door("+from+"#"+names+")",
	  dir+", through the "+names);
  where->add_extra_long("@/"+from+"->door_long("+names+")");

  nr_doors++;
  opened += ({0});
  doordir += ({dir});
}

varargs void set_lockdoor(string name,string code,string desc)
{
  if (!desc) desc = "lock";
  locks[doors[name]]=({1,code,desc});
  add_item("lock","##");
}

/* what==0 try unlock door, what==1 try lock door */
int lockunlock(string name, string code, int what)
{
  if (doors[name]<0) return 4;
  if (!locks[doors[name]])
    return 0;
  if (locks[doors[name]][0]==what)
    return 2;
  if (locks[doors[name]][1]==code) {
    locks[doors[name]][0]=what;
    return 1;
  }
  return 3;
}

remove_door()
{
}

string door_long(string name) {
  string r;
  r = previous_object()==this_object()?
    doordir[doors[name]]:reversedir[doordir[doors[name]]];
  return "There is a "+
    (opened[doors[name]]?"open "+name:"closed "+name)+
    " to the "+r+".\n";
}

string door_desc(string name) {
  string tmp;
  
  tmp = "It is a "+(opened[doors[name]]?"open "+name:"closed "+name);
  if (locks[doors[name]]) tmp += " with a "+locks[doors[name]][2];
  return tmp+".\n";
}

int open_door(string name,string what) {
  string verb;
  verb = query_verb();

  if (locks[doors[name]] && locks[doors[name]][0]) {
    write("You can't do that. The "+name+" is locked.\n");
    return 1;
  }
  if (opened[doors[name]] == (verb == "open")) {
    write("The "+name+" is already "+(verb == "open"?"open":"closed")+".\n");
  } else {
    opened[doors[name]]=(verb == "open");
    "/std/msg"->msg("\bPRON "+verb+"\b$ the "+name+".\n");
  }
  add_extra_long("@door_long("+name+")");

  return 1;
}

mixed check_door(string exitdoor,mixed arg)
{ 
  string *ed;
  ed = explode(exitdoor,"#");

  if (opened[doors[ed[1]]] || arg=="query")
    return ed[0];
  write("The "+ed[1]+" is closed.\n");
  return 1;
}

query_doors(string s)
{
  if (s) return doors[s];
  return doors;
}

query_doordir() { return doordir;}
query_locks() { return locks;}
