/* This object is the Trade Clubs Mark
 |
 | Changelog
 |------------------------
 | Date: Sat Dec  9 23:47:58 1995    First Version By Taren and Brom.
 |
 */


#include "/std/special/trade/send.h";
#include "/obj/line/line.c";
inherit "/std/basic_thing";

#define tell_club(str)   tell_room(load_object("/std/special/trade/send_room"),str)
#define TP this_player()
#define TO this_object()

int is_ld=0;

reset(arg)
{
  if(arg) return;
  set_short("trade mark");
  set_long("This is your Trade Mark.\n"); 
  set_name("the_traders_mark");
  add_alias("club_mark");
  add_alias("trade mark");
  add_alias("mark");
  add_alias("trade");
  add_property("non-gettable");
}


self_dest()
{
  transfer(TO,load_object("/room/void"));
  return destruct(TO);
}

greet()
{
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

query_auto_load()
{
  return "/std/special/trade/trade:888#";
}

join()
{
  tell_room(SENDROOM,this_player()->query_name()+" just joined the Traders.\n");
}

drop(){ return 1;}
