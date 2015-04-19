/* Written by Profezzorn */

#pragma strict_types
#pragma save_types

inherit "/std/basic_monster";
inherit "/std/msg";

private int walk_pace;   /* Seconds / Pace */
private int last_seen;   /* Seconds since we last saw an interactive */
private string *chats=({});
static private string *output_queue=({});
private mixed *attacks=({});
private int chat_chance;

/*** setup functions ***/

void add_chat(mixed data)
{
  if(!pointerp(data)) data=({data});
  chats+=data;
}

void remove_chat(mixed data)
{
  if(!pointerp(data)) data=({data});
  chats-=data;
}

void set_chat_chance(int chance) { chat_chance=chance; }
void start_walking(int pace) { walk_pace = pace || 200; }
void stop_walking() { walk_pace=0; }

varargs void add_attack(string msg,
			int chance,
			mixed dam,
			mixed busy,
			mixed sp)
{
  mixed att;
  int e;

  if(!msg) return;
  if(intp(chance)<1) return;
  att=({msg,chance,dam,busy,sp});
  for(e=sizeof(att)-1;!att[e];e--) att=att[0..e];
  attacks+=({ att });
}

varargs void remove_attack(string msg,
			   int chance,
			   mixed dam,
			   mixed busy,
			   mixed sp)
{
  int e,d;
  mixed *tmp;
  tmp=({msg, chance, dam, busy, sp });
  for(e=0;e<sizeof(attacks);e++)
  {
    for(d=0;d<sizeof(attacks[e]);d++)
      if(attacks[e][d]!=tmp[d])
	break;
    if(d==sizeof(attacks[e]))
    {
      attacks=attacks[0..e-1]+attacks[e+1..0];
      return;
    }
  }
}

/*** internal functions ***/

#define WARRAY ({ "east", "west", "north", "south", "out", "in", "northeast",\
		    "southeast", "northwest", "southwest", "up", "down" })

#define MAY_ENTER(X) ( \
  ( r && function_exists("remote_prevent_enter",r) ) ? \
  ( ! r->remote_prevent_enter(this_object(), X) ) : \
  ( creator(this_object()) == creator(load_object(X)) || \
    query_property("walk_all_over")))

#define DEBUG(X) tell_object(find_player("mats"),X+"\n")

void walk()
{
  object attack;

  if(!walk_pace) return;

  test_if_any_here(); /* maybe update last_seen */

  if(!((attack=query_attack()) && environment(attack)==environment()))
  {
    int approved,e,i,nr;
    mapping known_exits;
    mixed r;
    object back;
    string *exits;

    back=environment();
    enable_commands();
    
    known_exits=back->query_exits();
    if(!known_exits)
    {
      mixed tmp;
      known_exits=([]);
      
      if(tmp=back->query_dest_dir())
      {
	for(e=1;e<sizeof(tmp);e+=2)
	{
	  if(pointerp(tmp[e]))
	  {
	    for(i=0;i<sizeof(tmp[e]);e++)
	      known_exits[tmp[e][i]]=tmp[e-1];
	  }else{
	    known_exits[tmp[e]]=tmp[e-1];
	  }
	}
      }
    }
    
    r=query_property("__remote");
    if(r)
    {
      r=load_object(r);
      exits=r->remote_handle_move_monster(this_object());
      if(stringp(exits)) exits=({exits});
    }
    
    if(!exits)
      exits = (WARRAY - m_indices(known_exits)) + m_indices(known_exits);
    
    nr=sizeof(exits);
    do{
      string dir,dest;
      dir=exits[i=random(nr)]; 
      if(dest=known_exits[dir])
      {
	if(MAY_ENTER(dest))
	{
	  approved=1;
	  command(dir);
	}
      }else{
	approved=0;
	command(dir);
      }
      
      exits[i]=exits[--nr];
    }while(nr && environment() == back);
    
    if(environment() != back)
    {
      if(!approved && !MAY_ENTER("/"+file_name(environment())))
      {
	call_out("move_player",0,"in a hurry#"+file_name(back));
      }
      else if(r)
      {
	r->remote_handle_moved(this_object());
      }
    }
  }
  
  remove_call_out("walk");
  if(time() - last_seen < walk_pace*6)
    call_out("walk", random(walk_pace)+walk_pace/2);
}

private static int busy;

void do_extra_attacks()
{
  mixed *att;
  object who;
  int e;

  if(busy)
  {
    busy--;
    return;
  }

  who=query_attack();
  for(e=0;e<sizeof(attacks);e++)
  {
    att=attacks[e];
    if(random(100)< eval(att[1],who))
    {
      int tmp;

      switch(sizeof(att))
      {
      default: 
	tmp=eval(att[4],who);
	if(tmp > spell_points) continue;
	spell_points-=tmp;
      case 4:  busy+=eval(att[3],who);
      case 3:  spell_dam+=eval(att[2],who);
      case 2:  /* do nothing */
      case 1:  msg(eval(att[0],who),this_player(),who);
      }
      if(busy) return;
    }
  }
}

/*** Functions called by mudlib and driver ***/

int heart_beat()
{
  string tmp;
  tmp=query_property("__remote");
  if(!query_attack())
  {
    if(tmp) tmp->remote_peace_beat(this_object());

    if(!sizeof(output_queue) && random(100) < chat_chance)
    {
      if(tmp=eval(chats[random(sizeof(chats))]))
	output_queue+=explode(sprintf("%-=74s",tmp),"\n");
    }
  } else {
    if(tmp) tmp->remote_fight_beat(this_object());
    do_extra_attacks();
  }

  if(sizeof(output_queue))
  {
    say(output_queue[0]+"\n");
    output_queue=output_queue[1..1000000];
  }

  ::heart_beat();
}

int test_if_any_here()
{
  if(::test_if_any_here())
  {
    last_seen=time();
    return 1;
  }
  return 0;
}

int heart_beat_needed()
{
  return ::heart_beat_needed() ||
    (chat_chance && test_if_any_here()) ||
      sizeof(output_queue);
}

void init()
{
  ::init();
  if(interactive(this_player()))
  {
    last_seen=time();

    if(walk_pace && -1==find_call_out("walk"))
    {
      remove_call_out("walk");
      call_out("walk", random(walk_pace));
    }
  }

  set_heart_beat(1);
}
