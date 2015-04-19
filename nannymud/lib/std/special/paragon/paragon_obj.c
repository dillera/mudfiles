/* This object is the Paragon aura.
 |
 | Changelog
 |------------------------
 | 951120    First Version By Taren.
 |
 */

#include "/std/special/paragon/send.h";
#include "/obj/line/line.c";
inherit "/std/basic_thing";

#define tell_club(str) tell_room(load_object("/std/special/paragon/send_room"),str)
#define TP this_player()
#define TO this_object()

int is_ld=0;

reset(arg)
{
  if(arg) return;
  set_name("the_aura_of_paragon");
  add_alias("aura");
  add_alias("paragon");
  add_property("non-gettable");
}

fix()
{
  object e,r;
  string name;
  
  e=environment(TO);
  if(e)
    name=e->query_name();
  if(!name) name="The Unknown";
  add_property("__extra_look",name+" bears the aura of Paragon");
  if(e->query_level()<19)
    {
      if(e->query_paragon())
	{
	  r=load_object("/room/paragon_room");
	  if(r&&objectp(r))
	    r->add_to_list(e->query_real_name(),e->query_paragon_level());
	}
    }
}

self_dest()
{
  transfer(TO,load_object("/room/void"));
  return destruct(TO);
}

check()
{
  object e;  
  e=environment(TO);
  if(!e)
    return;
  if(!interactive(e))
    return call_out("self_dest",2),1;
  if(e->query_level()>22) return;
  if(!e->query_paragon())
    return call_out("self_dest",2),1;
}

greet()
{
  if (!objectp(TP)) return;
  if(TP->query_invis())
    return;
  tell_club(TP->query_name()+" enters Nannymud.\n");
}

quit()
{
  if(TP->query_invis()) return;
  tell_club(TP->query_name()+" leaves Nannymud.\n");
}

init()
{
  if(check()) return;
  init_line();
  add_action("quit","quit");
  call_out("_signal_regained_connection",2);
}

_signal_regained_connection()
{
  while(find_call_out("_signal_regained_connection")>-1)
    remove_call_out("_signal_regained_connection");
  if(is_ld)
  {
    is_ld=0;
    if(TP->query_invis()) return;
    tell_club(TP->query_name()+" regained the connection to NannyMud.\n");
  }
}

_signal_lost_connection()
{
  string name;
  if(!is_ld)
  {
    is_ld=1;
    if(!environment(TO)->short())
      return;
    name=environment(TO)->query_name();
    if(!name) return;
    tell_club(name+" lost the connection to NannyMud.\n");
  }
}

init_arg(string st)
{
  call_out("greet",2);
  call_out("fix",2);
}

query_auto_load()
{
  return "/std/special/paragon/paragon_obj:666#";
}
