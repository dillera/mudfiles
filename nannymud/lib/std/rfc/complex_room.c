/* Written by Profezzorn */
/* This code is hazardous to your health */

/*
 * Todo:
 * Make query functions so you can find out everything about the doors
 * Make set functions so you can close/open/lock doors from other objects
 * Comment code
 * Write documentation
 */
#pragma strict_types
#pragma save_types

inherit "/std/room";
inherit "/std/msg";

private mixed *doors=({});

/* BEWARE, THESE DEFINES MIGHT CHANGE VALUES AT ANY TIME */

/* These are OR:ed in door[DOOR_STATUS] */
#define DOOR_IS_CLOSED 1
#define DOOR_IS_LOCKED 2
#define DOOR_IS_BROKEN 4
#define DOOR_WILL_CLOSE 8
#define DOOR_WILL_LOCK 16
#define DOOR_WILL_HEAL 32
#define DOOR_IS_UNPICKABLE 64

/*
 * these are the data stored for each door, each define is an index in an
 * array, default values are assumed for entries not present in the array
 * (if the array is too small)
 */

/* needed */
#define DOOR_STATUS 0
#define DOOR_DIR 1
#define DOOR_DEST 2

/* optional */
#define DOOR_ADJ 3
#define DOOR_NAME 4
#define DOOR_DESC 5

#define DOOR_KEY_ID 6

#define DOOR_HP 7
#define DOOR_MAX_HP 8

#define DOOR_OPEN_MSG 9
#define DOOR_CLOSE_MSG 10
#define DOOR_UNLOCK_MSG 11
#define DOOR_LOCK_MSG 12
#define DOOR_KNOCK_MSG 13

#define DOOR_PREVENT_ENTER 14
#define DOOR_PREVENT_OPEN 15
#define DOOR_PREVENT_CLOSE 16
#define DOOR_PREVENT_LOCK 17
#define DOOR_PREVENT_UNLOCK 18
#define DOOR_PREVENT_KNOCK 19
#define DOOR_PREVENT_ATTACK 20

#define DOOR_AUTO_CLOSE_MSG 21
#define DOOR_AUTO_HEAL_MSG 22
#define DOOR_AUTO_LOCK_MSG 23

#define DOOR_MAX_SIZE 24

/* These defines are arguments to describe_door to modify it's behaviour */
#define DESCRIBE_SHORT 0
#define DESCRIBE_IN_LONG 1
#define DESCRIBE_EXPLICIT 2
#define DESCRIBE_LONG 3

string describe_door(mixed *door, int length);
int door_cmd(string cmd, mixed *door, object with);

string reverse_dir(string dir)
{
  switch(dir)
  {
  case "north": return "south";
  case "south": return "north";
  case "east": return "west";
  case "west": return "east";
  case "up": return "down";
  case "down": return "up";
  case "out": return "in";
  case "in": return "out";
  case "southeast": return "northwest";
  case "southwest": return "northeast";
  case "northeast": return "southwest";
  case "northwest": return "southeast";
  }
}

#define GET_OTHER_DOOR(X) (\
X[DOOR_DEST]->query_door("/"+file_name(this_object()),reverse_dir(X[DOOR_DIR])) || \
X[DOOR_DEST]->query_door("/"+file_name(this_object())))

/* Add door */
varargs mixed * add_door(string dir, 
			 mixed whereto,
			 string adjectives,
			 string long_desc,
			 mapping extra)
{
  object o;
  mixed *data;
  int e;

  if(objectp(whereto))
  {
    o=whereto;
    whereto="/"+file_name(o);
  }else{
    whereto=expand_file_name(whereto); 
    o=find_object(whereto);
  }

  if(!o && !adjectives && !long_desc && !extra)
    o=load_object(whereto);

  if(o)
  {
    data=o->query_door("/"+file_name(this_object()),reverse_dir(dir));
    if(data) data+=({});
  }

  if(!extra) extra=([]);

  if(!data)
  {
    data=allocate(DOOR_MAX_SIZE);
    data[DOOR_STATUS]=
      (extra["open"]?0:DOOR_IS_CLOSED | DOOR_WILL_CLOSE) |
	(extra["locked"]?DOOR_IS_LOCKED | DOOR_WILL_LOCK:0) |
	  (extra["broken"]?DOOR_IS_BROKEN:DOOR_WILL_HEAL);

    data[DOOR_NAME]=extra["name"];
    data[DOOR_ADJ]=adjectives;
    data[DOOR_DESC]=long_desc;
    data[DOOR_MAX_HP]=data[DOOR_HP]=extra["hp"];
    if(data[DOOR_KEY_ID]=extra["key_id"])
      data[DOOR_STATUS]|=DOOR_IS_LOCKED | DOOR_WILL_LOCK;

    data[DOOR_PREVENT_ENTER]=extra["prevent_enter"];
    data[DOOR_PREVENT_OPEN]=extra["prevent_open"];
    data[DOOR_PREVENT_CLOSE]=extra["prevent_close"];
    data[DOOR_PREVENT_LOCK]=extra["prevent_lock"];
    data[DOOR_PREVENT_UNLOCK]=extra["prevent_unlock"];
    data[DOOR_PREVENT_ATTACK]=extra["prevent_attack"];
    data[DOOR_PREVENT_KNOCK]=extra["prevent_knock"];

    data[DOOR_OPEN_MSG]=extra["open_msg"];
    data[DOOR_CLOSE_MSG]=extra["close_msg"];
    data[DOOR_LOCK_MSG]=extra["lock_msg"];
    data[DOOR_UNLOCK_MSG]=extra["unlock_msg"];
    data[DOOR_KNOCK_MSG]=extra["knock_msg"];
    
    data[DOOR_AUTO_CLOSE_MSG]=extra["auto_close_msg"];
    data[DOOR_AUTO_HEAL_MSG]=extra["auto_heal_msg"];
    data[DOOR_AUTO_LOCK_MSG]=extra["auto_lock_msg"];

    if(extra["unlocked"])
      data[DOOR_STATUS]&=~(DOOR_IS_LOCKED | DOOR_WILL_LOCK);

    if(extra["no_auto_close"]) data[DOOR_STATUS]&=~DOOR_WILL_CLOSE;
    if(extra["no_auto_lock"]) data[DOOR_STATUS]&=~DOOR_WILL_LOCK;
    if(extra["no_auto_heal"]) data[DOOR_STATUS]&=~DOOR_WILL_HEAL;
    if(extra["unpickable"]) data[DOOR_STATUS]|=DOOR_IS_UNPICKABLE;
  }

  data[DOOR_DIR]=dir;
  data[DOOR_DEST]=whereto;

  for(e=sizeof(data)-1;!data[e];e--);
  data=data[0..e];

  for(e=0;e<sizeof(doors);e++)
  {
    if(doors[e][DOOR_DIR]==dir)
    {
      doors[e]=data;
      break;
    }
  }
  if(e==sizeof(doors)) doors+=({data});
  
  add_exit(data[DOOR_DIR],data[DOOR_DEST],0,
	   "@check_door("+data[DOOR_DIR]+"\b"+data[DOOR_DEST]+")");
  ::compute_extra_long(); /* recompute later */
}

void remove_door(mixed whereto, string dir)
{
  int e;
  if(objectp(whereto))
    whereto="/"+file_name(whereto);
  else if(stringp(whereto))
    whereto=expand_file_name(whereto);
    
  for(e=0;e<sizeof(doors);e++)
  {
    if(doors[e][DOOR_DEST]==whereto)
    {
      if(!dir || dir==doors[e][DOOR_DIR])
      {
	remove_exit(doors[e][DOOR_DIR]);
	doors=doors[0..e-1]+doors[e+1..0x7fffffff];
	whereto->remove_door(this_object(),reverse_dir(dir));
	break;
      }
    }
  }
}


/*** Internal ***/
mixed *query_door(string whereto, string dir)
{
  int e;
  if(!sizeof(doors)) return 0;
  if(!whereto && !dir) return doors;

  for(e=0;e<sizeof(doors);e++)
    if(doors[e][DOOR_DEST]==whereto)
      if(!dir || dir==doors[e][DOOR_DIR])
	return doors[e];
}

int check_door(string data, string arg)
{
  int e;
  string dir, dest;
  mixed door;
  if(sscanf(data,"%s\b%s",dir,dest) && (door=query_door(dest,dir)))
  {
    if(door[DOOR_STATUS] & DOOR_IS_BROKEN) return 0;
    
    if((door[DOOR_STATUS] & DOOR_IS_CLOSED))
    {
      door_cmd("open",door,0);
      if(door[DOOR_STATUS] & DOOR_IS_CLOSED) return 1;
    }
    return 0;
  }

  /* This is really a bug */
  write("You aim for a door but miss and walk into a wall.\n");
  return 1;
}

string add_a(string a)
{
  switch(a[0])
  {
  case 'a': case 'o': case 'u': case 'e': case 'i':
  case 'A': case 'O': case 'U': case 'E': case 'I':
    return "an "+a;
  default: return "a "+a;
  }
}

/* Instead of indexing a door array directly we use
 * this routine to get default values for nonexistant
 * indexes. The first three values are required though,
 * so for those we can index directly
 */
varargs mixed get_door_datum(mixed *door, int type)
{
  mixed ret,tmp;
  if(sizeof(door)>type) ret=door[type];

  switch(type)
  {
  case DOOR_ADJ:
    if(!pointerp(ret)) ret=({ret})-({0});
    break;

  case DOOR_NAME:
    if(!ret)
    {
      if(door[DOOR_DIR]=="up" || door[DOOR_DIR]=="down")
	ret=({"hatch"});
      else
	ret=({"door"});
    }else{
      if(!pointerp(ret)) ret=({ret});
    }

  case DOOR_DESC:
    if(!ret)
    {
      ret=describe_door(door, DESCRIBE_IN_LONG);
      sscanf(ret,"the %s",ret);
      ret="It's "+ret+".\n";
    }
    break;

  case DOOR_OPEN_MSG: if(!ret) ret="\bPRON open\b$ \bDOOR.\n"; break;
  case DOOR_CLOSE_MSG: if(!ret) ret="\bPRON close\b$ \bDOOR.\n"; break;
  case DOOR_UNLOCK_MSG: if(!ret) ret="\bPRON unlock\b$ \bDOOR.\n"; break;
  case DOOR_LOCK_MSG: if(!ret) ret="\bPRON lock\b$ \bDOOR.\n"; break;
  case DOOR_KNOCK_MSG: if(!ret) ret="\bPRON knock\b$ on \bDOOR.\n"; break;

  case DOOR_AUTO_CLOSE_MSG:
    if(!ret) ret="Suddenly a breeze sweeps by and \bDOOR slams shut.\n";
    break;

  case DOOR_AUTO_LOCK_MSG:
    if(!ret) ret="You hear a click from \bDOOR.\n";
    break;

  case DOOR_AUTO_HEAL_MSG:
    if(!ret) ret="Suddenly, \bDOOR is restored to it's original shape, must be magic!\n";
    break;
  }

  return ret;
}

static int can_see;
int see_names(object who,object active) { return can_see; }

varargs void door_room_msg(string message, mixed *door, int notp)
{
  object o;
  mixed other;
  int light,other_can_see;
  string tmp,first,here,rest;
  mixed involved;

  o=load_object(get_door_datum(door, DOOR_DEST));

  if(sscanf(message,"%s\b|%s",first,here))
  {
    sscanf(here,"%s\b|%s",here,rest);
  }else{
    first=here=message;
  }

  tmp=describe_door(door,DESCRIBE_SHORT);
  if(notp)
  {
    involved=({});
  }else{
    involved=({({this_player()})});

    can_see=1;
    first=replace(first,"\bDOOR",tmp);
    write(capitalize(fix_msg(first,involved,this_player())));
  }

  light=set_light(0);
  
  if((door[DOOR_STATUS] & DOOR_IS_BROKEN) || !(door[DOOR_STATUS] & DOOR_IS_CLOSED))
  {
    if(set_light(0,o)>0) light++;
    if(light>0) other_can_see=1;
  }
  can_see=light>0;
  if(notp)
  {
    tell_room(this_object(),
	      capitalize(fix_msg(replace(here,"\bDOOR",tmp),involved,0)));
  }else{
    tell_room(this_object(),
	      capitalize(fix_msg(replace(here,"\bDOOR",tmp),involved,0)),
	      ({this_player()}));
  }

  if(o==this_object()) return;
  can_see=other_can_see;
  /* Default: give message to other room if it has a door back */
  if(other=GET_OTHER_DOOR(door))
  {
    rest=replace(rest||here,"\bDOOR",o->describe_door(other,DESCRIBE_SHORT));
    tell_room(o,capitalize(fix_msg(rest,involved,0)));
  }
  else if(rest) /* or if there was a specific message */
  {
    tell_room(o,capitalize(fix_msg(rest,involved,0)));
  }
}

/* Describe a door, with four different formats */
string describe_door(mixed *door, int length)
{
  string desc,*tmp,xtra;

  switch(length)
  {
  case DESCRIBE_EXPLICIT:
  case DESCRIBE_IN_LONG:

    if(door[DOOR_STATUS] & DOOR_IS_BROKEN)
    {
      xtra="broken";
    }else{
      if(length!=DESCRIBE_IN_LONG)
      {
	if(door[DOOR_STATUS] & DOOR_IS_CLOSED)
	{
	  if(door[DOOR_STATUS] & DOOR_IS_LOCKED)
	  {
	    xtra="locked";
	  }else{
	    xtra="closed";
	  }
	}else{
	  xtra="open";
	}
      }
    }

  case DESCRIBE_SHORT:
    tmp=get_door_datum(door,DOOR_ADJ);

    if(xtra) tmp=({xtra})+tmp;

    tmp+=({get_door_datum(door,DOOR_NAME)[0]});

    desc=implode(tmp," ");

    switch(tmp=get_door_datum(door,DOOR_DIR))
    {
    case "north":
    case "south":
    case "east":
    case "west":
    case "northeast":
    case "southeast":
    case "northwest":
    case "southwest":
      desc+=" to the "+tmp;
      break;
    default:
      desc+=" leading "+tmp;
      break;
    }
    if(length==DESCRIBE_IN_LONG)
    {
      return add_a(desc);
    }else{
      return "the "+desc;
    }
    /* NOTREACHED */

  case DESCRIBE_LONG:
    desc=get_door_datum(door, DOOR_DESC);
    if(get_door_datum(door,DOOR_STATUS) & DOOR_IS_BROKEN)
    {
      desc+="Or at least it used to be, someone has broken it down.\n";
    }else{
      switch(get_door_datum(door,DOOR_STATUS) & (DOOR_IS_CLOSED | DOOR_IS_LOCKED))
      {
      case DOOR_IS_CLOSED:
	desc+="It is closed.\n";
	break;
	
      case DOOR_IS_CLOSED | DOOR_IS_LOCKED:
	desc+="It is closed and locked.\n";
	break;
	
      case DOOR_IS_LOCKED:
	desc+="It is locked open.\n";
	break;
	
      case 0:
	desc+="It is open.\n";
      }
    }
    return desc;
  }
}

/* Identify door, does basically the reverse of describe door
 * returns an array of all doors fitting the description
 */
mixed *identify_door(string door)
{
  mixed *tmp, *words, dir, name;
  int e, nr;
  /* things to recognize:
   * the open door, 
   * the magnificient door, 
   * the closed door,
   * the northern door, 
   * the door leading north, 
   * the door to the north
   * and maybe:
   * door #
   * remember, having 'door' as a 'name' isn't good enough, several 'names'
   * must be possible.
   */

  if(!doors || !sizeof(doors)) return ({});

  sscanf(door,"the %s",door);
  tmp=doors+({});

  if(sscanf(door,"%s leading %s",door,dir) || 
     sscanf(door,"%s to the %s",door,dir))
  {
    for(e=0;e<sizeof(tmp);e++)
      if(tmp[e][DOOR_DIR]!=dir)
        tmp[e]=0;
    tmp-=({0});

    if(!sizeof(tmp)) return tmp;
  }

  words=explode(door," ") || ({door});

  if(words[-1][0]>='0' && words[-1][0]<='9')
  {
    sscanf(words[-1],"%d",nr);
    words=words[0..sizeof(words)-2];
  }else{
    nr=-1;
  }

  name=words[-1];
  words=words[0..sizeof(words)-2];

  for(e=0;e<sizeof(tmp);e++)
    if(member_array(name,get_door_datum(tmp[e],DOOR_NAME))==-1)
      tmp[e]=0;

  tmp-=({0});
  if(!sizeof(tmp)) return tmp;

  if(sizeof(words))
  {
    for(e=0;e<sizeof(tmp);e++)
    {
      mixed *foo;
      foo=get_door_datum(tmp[e],DOOR_ADJ);
      foo+=({ 
	tmp[e][DOOR_STATUS] & DOOR_IS_CLOSED ? "closed" : "open" ,
	  tmp[e][DOOR_STATUS] & DOOR_IS_LOCKED ? "locked" : "unlocked",
	  tmp[e][DOOR_STATUS] & DOOR_IS_BROKEN ? "broken" : "unbroken",
      });
      
      switch(tmp[e][DOOR_DIR])
      {
      case "north":
      case "south":
      case "east":
      case "west":
      case "northwest":
      case "southwest":
      case "northeast":
      case "southeast":
	foo+=({tmp[e][DOOR_DIR], tmp[e][DOOR_DIR]+"ern"});
      }

      if(sizeof(foo & words) < sizeof(words))
	tmp[e]=0;
    }
  }

  tmp-=({0});
  if(!sizeof(tmp)) return tmp;

  if(nr>0)
  {
    if(nr<sizeof(tmp))
      return tmp[nr..nr];
    else
      return ({});
  }

  return tmp;
}

/* Simple identify door, return the door fitting the description
 * do notify_fail and return zero if the number of doors matching
 * the description != 1
 */
mixed simple_identify_door(string s)
{
  mixed *d;
  notify_fail(capitalize(query_verb())+" what?\n");
  if(!s) return 0;

  if(!doors || !sizeof(doors))
  {
    if(id(s) || 
       present(s,this_object()) || 
       present(s,this_player()))
    {
      notify_fail("You cannot "+query_verb()+" that.\n");
    }else{
      sscanf(s,"the %s",s);
      notify_fail("There is no "+s+" here.\n");
    }

    return 0;
  }

  d=identify_door(s);
  switch(sizeof(d))
  {
  case 0:
    sscanf(s,"the %s",s);
    notify_fail("There is no "+s+" here.\n");
    return 0;
  default:
    write("Which one?\n"+
	  capitalize(implode_nicely(map_array(d,"describe_door",this_object(),DESCRIBE_EXPLICIT),"or")+"?\n"));
    return 1;

  case 1:
    return d[0];
  }
}

/* Make it possible to examine doors */
string query_mentioned_in_long(string s)
{
  string tmp;
  mixed *doors;

  switch(sizeof(doors=identify_door(s)))
  {
  case 0: return 0;
  case 1: return describe_door(doors[0],DESCRIBE_LONG);
  default:
    return "Which one?\n"+
      capitalize(implode_nicely(map_array(doors,"describe_door",this_object(),DESCRIBE_EXPLICIT),"or")+"?\n");
  }

  return ::query_mentioned_in_long(s);
}

/* Make doors show up in long() */
varargs string compute_extra_long(int recompute)
{
  string xlong;
  if(!recompute) return ::compute_extra_long(recompute);

  if(sizeof(doors))
  {
    xlong=sprintf("%-=77s\n","There is "+
		  implode_nicely(map_array(doors,"describe_door",this_object(),DESCRIBE_IN_LONG))+".");
  }else{
    xlong="";
  }
  xlong+=::compute_extra_long(recompute);
  return xlong;
}

mixed *translate(string cmd)
{
  int bit, value, strength, prevent_type;
  string state;
  mixed msg_type;

  switch(cmd)
  {
  default: return 0;

  case "knock":
    bit=0;
    value=0;
    state=0;
    msg_type=DOOR_KNOCK_MSG;
    prevent_type=DOOR_PREVENT_KNOCK;
    break;
    
  case "break":
    value=bit=DOOR_IS_BROKEN;
    msg_type="\b{try,tries} to break";
    state="broken";
    prevent_type=DOOR_PREVENT_ATTACK;
    strength=10+random(10);
    break;
    
  case "hit":
    value=bit=DOOR_IS_BROKEN;
    msg_type="hit\b$";
    state="broken";
    prevent_type=DOOR_PREVENT_ATTACK;
    strength=5+random(5);
    break;
    
  case "kick":
    value=bit=DOOR_IS_BROKEN;
    msg_type="kick\b$";
    state="broken";
    prevent_type=DOOR_PREVENT_ATTACK;
    strength=10+random(20);
    break;
    
  case "smash":
    value=bit=DOOR_IS_BROKEN;
    msg_type="\b{smash,smashes} violently";
    strength=random(50);
    state="broken";
    prevent_type=DOOR_PREVENT_ATTACK;
    break;
    
  case "tackle":
    value=bit=DOOR_IS_BROKEN;
    msg_type="tackle\b$";
    state="broken";
    prevent_type=DOOR_PREVENT_ATTACK;
    strength=20;
    break;
    
  case "open":
    bit=DOOR_IS_CLOSED;
    value=0;
    state="open";
    prevent_type=DOOR_PREVENT_OPEN;
    msg_type=DOOR_OPEN_MSG;
    break;
    
  case "close":
    value=bit=DOOR_IS_CLOSED;
    state="closed";
    prevent_type=DOOR_PREVENT_CLOSE;
    msg_type=DOOR_CLOSE_MSG;
    break;
    
  case "lock":
    value=bit=DOOR_IS_LOCKED;
    state="locked";
    prevent_type=DOOR_PREVENT_LOCK;
    msg_type=DOOR_LOCK_MSG;
    break;
    
  case "unlock":
    bit=DOOR_IS_LOCKED;
    value=0;
    state="unlocked";
    prevent_type=DOOR_PREVENT_UNLOCK;
    msg_type=DOOR_UNLOCK_MSG;
    break;
    
  }
  return ({ bit, value, state, prevent_type, msg_type, strength });
}

void affect_door(mixed *door, string how)
{
  int bit, value;
  mixed foo, *other;
  foo=translate(how);

  other=GET_OTHER_DOOR(door) || allocate(DOOR_STATUS+1);
  other[DOOR_STATUS]=door[DOOR_STATUS]=(door[DOOR_STATUS] & ~bit) | value;
}

varargs int door_cmd(string cmd, mixed *door, object with)
{
  mixed other, tmp;
  int bit, value,strength;
  string state;
  mixed prevent_type, msg_type;

  if(door[DOOR_STATUS] & DOOR_IS_BROKEN)
  {
    write("It's broken.\n");
    return 0;
  }

  tmp=translate(cmd);
  if(!tmp)
  {
    write("Do what?\n");
    return 0;
  }

  bit=tmp[0];
  value=tmp[1];
  state=tmp[2];
  prevent_type=tmp[3];
  msg_type=tmp[4];
  strength=tmp[5];
  
  if(state && (door[DOOR_STATUS] & bit)==value)
  {
    write("It is already "+state+".\n");
    return 0;
  }
  
  if(tmp=eval(get_door_datum(door,prevent_type),door[DOOR_DIR]))
  {
    if(stringp(tmp))
      door_room_msg(tmp,door);
    else
      write("You can't do that.\n");
    return 0;
  }
  
  other=GET_OTHER_DOOR(door) || allocate(DOOR_STATUS+1);

  switch(bit)
  {
  case DOOR_IS_BROKEN:
    if(this_player()->check_busy()) return 0;
       
    if(cmd=="hit" || cmd=="break")
    {
      if(with)
      {
	if(!with->id("axe") || !with->query_type("chop"))
	{
	  write("You can't use that to "+cmd+" things.\n");
	  return 0;
	}

	if(!with->query_wielded())
	{
	  write("You need to wield it first.\n");
	  return 0;
	}

	value=random(with->query_wc()*2);
      }
    }else{
      if(with)
      {
	write("You can't "+cmd+" anything with that.\n");
	return 0;
      }
    }
    door_room_msg("\bPRON "+msg_type+" \bDOOR.\n",door);

    /* Use strength */
    value=value*(20+this_player()->query_str()) / 40;

    /* Don't hurt players if they use an object */
    if(!with)
    {
      if(this_player()->query_hp()<value)
      {
	write("You're in no condition to do that.\n");
	return 0;
      }
      
      this_player()->hit_player(value >> 1);
    }
    
    if(get_door_datum(door,DOOR_MAX_HP))
    {
      if(door[DOOR_HP] > this_player()->query_str()*5)
      {
	value=0;
      }else{
	value-=random(door[DOOR_HP]);
	if(value < 0) value=0;
      }

      door[DOOR_HP]-=value;
      if(door[DOOR_HP]<=0)
      {
	door_room_msg("\bDOOR breaks into fragments.\n",door);
	door[DOOR_STATUS]&=~(DOOR_IS_LOCKED | DOOR_IS_CLOSED);
	door[DOOR_STATUS]|=DOOR_IS_BROKEN;
	other[DOOR_STATUS]=door[DOOR_STATUS];
	compute_extra_long(0);
	return 1;
      }
    }else{
      value=0;
    }
    
    switch(value)
    {
    default:
      door_room_msg("\bDOOR is partially obliterated.\n",door);
      break;
    case 46..135:
      door_room_msg("\bDOOR now has a small hole in it.\n",door);
      break;
    case 16..45:
      door_room_msg("\bDOOR shakes in it's frame.\n",door);
      break;
    case 6..15:
      door_room_msg("\bDOOR budges a little.\n",door);
      break;
    case 1..5:
      door_room_msg("\bDOOR shakes slightly.\n",door);
      break;
    case 0:
      door_room_msg("\bDOOR is unharmed.\n",door);
      break;
    }
    return 0;
    
  case DOOR_IS_CLOSED:
    if(door[DOOR_STATUS] & DOOR_IS_LOCKED)
    {
      door_cmd("unlock",door,with);
      with=0;
      if(door[DOOR_STATUS] & DOOR_IS_LOCKED)
	return 0;
    }

    if(with)
    {
      write("You can't "+cmd+" "+
	    describe_door(door, DESCRIBE_EXPLICIT)+" with that.\n");
      return 1;
    }
    
    break;
    
  case DOOR_IS_LOCKED:
    if(!(tmp=get_door_datum(door,DOOR_KEY_ID))) /* Lockable */
    {
      write("There is no lock on "+describe_door(door,DESCRIBE_SHORT)+".\n");
      return 1;
    }
    if(!with)
    {
      for(with=first_inventory(this_player());with;with=next_inventory(with))
	if(with->query_code(tmp) == tmp)
	  break;
      
      if(!with)
	for(with=first_inventory(this_object());with;with=next_inventory(with))
	  if(with->query_code(tmp) == tmp)
	    break;
      if(!with)
      {
	write("You don't have the key.\n");
	return 1;
      }
    }else{
      if(with->query_code(tmp) != tmp)
      {
	write("It doesn't fit "+describe_door(door,DESCRIBE_SHORT)+".\n");
	return 1;
      }
    }
  }

  other[DOOR_STATUS]=door[DOOR_STATUS]=(door[DOOR_STATUS] & ~bit) | value;
  door_room_msg(eval(get_door_datum(door,msg_type),door[DOOR_DIR]),door);
  return 1;
}

int do_cmd(string what)
{
  mixed with, tmp, door;

  if(::do_cmd(what)) return 1;

  switch(query_verb())
  {
  case "knock": if(what) sscanf(what,"on %s",what);

  case "open":
  case "close":
  case "lock":
  case "unlock":

  case "break":
  case "tackle":
  case "smash":
  case "hit":
  case "kick":

    if(what)
    {
      if(sscanf(what,"%s with %s",what,with))
      {
	tmp=present(with,this_player()) || present(with,this_object());
	if(!tmp)
	{
	  sscanf(with,"the %s",with);
	  notify_fail("There is no "+with+" here.\n");
	  return 0;
	}
	with=tmp;
      }
    }
    door=simple_identify_door(what);
    if(!pointerp(door)) return door;

    door_cmd(query_verb(), door, with);
    return 1;
  }
}

void reset(int arg)
{
  int e;
  ::reset(arg);
  if(arg)
  {
    for(e=0;e<sizeof(doors);e++)
    {
      int s;
      string msg;
      mixed door,tmp;
      
      door=doors[e];
      s=door[DOOR_STATUS];
      msg=0;

      if((s & DOOR_IS_BROKEN) && (s & DOOR_WILL_HEAL))
      {
	if(tmp=get_door_datum(door,DOOR_MAX_HP)) door[DOOR_HP]=tmp;
	msg=get_door_datum(door,DOOR_AUTO_HEAL_MSG);
	s&=~DOOR_IS_BROKEN;
      }
      if((s & DOOR_IS_CLOSED) && (s & DOOR_WILL_CLOSE))
      {
	msg=get_door_datum(door,DOOR_AUTO_CLOSE_MSG);
	s&=~DOOR_IS_CLOSED;
      }
      if((s & DOOR_IS_LOCKED) && (s & DOOR_WILL_LOCK))
      {
	msg=get_door_datum(door,DOOR_AUTO_LOCK_MSG);
	s&=~DOOR_IS_LOCKED;
      }

      if(msg)
      {
	door_room_msg(eval(msg,door[DOOR_DIR]),door,1);
	door[DOOR_STATUS]=s;
	if(tmp=GET_OTHER_DOOR(door))
	  tmp[DOOR_STATUS]=s;
      }
    }
  }
}
