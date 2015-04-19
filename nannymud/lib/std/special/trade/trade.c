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
#define MY_SENDCOST 7


reset(arg)
{
  if(arg) return;
  set_short("trade mark");
  set_long("This is your Trade Mark.\n"+
	   "Type 'leave trade' to leave the club.\n"); 
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

init()
{
  init_line();
  add_action("leave","leave");
}

leave(string what)
{
  if(id(what))
    {
      write("You leave the Traders club!\n");
      return self_dest(),1;
    }
  notify_fail("Leave what?\n");
}

query_auto_load()
{
  return "/std/special/trade/trade:888#";
}

join()
{
  tell_club(this_player()->query_name()+" just joined the Traders.\n");
}

drop(){ return 1;}


own_sendcost_function(string what)
{
  object ob;

  ob=environment(TP);
  if(ob&&objectp(ob))
    if(file_name(ob)=="std/special/trade/trade_room")
      return 1;      
  
  ob=present("the_collar_of_khorne",TP);
  if(ob&&objectp(ob))
    {
      if(TP->query_hp()<MY_SENDCOST)
	{
	  write("You're too weak to use the line!\n");
	  return 0;
	}
      this_player()->add_hp(-MY_SENDCOST);
      return 1;
    }
  else
    {
      if(TP->query_sp()<MY_SENDCOST)
	{
	  write("You're too weak to use the line!\n");
	  return 0;
	}
      this_player()->add_sp(-MY_SENDCOST);
      return 1;
    }
}

own_sendtocost_function(object who, string what)
{
  write("You can't use that command.\n");
  return 0;
}
