/*
   Changelog:
   By     When                                 What
--------------------------------------------------------------------------
|  Taren    Sat Apr 15 03:02:37 1995     First version
|  Anna     Sat Apr 15 14:11:51 1995     Second version
|  Taren    Mon Nov 20 12:42:38 1995     Third version
|  Taren    Mon Nov 27 02:59:33 1995     Started changelog, made flashier
|                                        'join' code
*/


#include <tune.h>

#define FILE "etc/paragons"
#define COST 1000000
#define BASE 1000000
#define PERCENT_PER_SL 5

#define FINGERD "/obj/daemon/fingerd"

static inherit "/std/room";

mapping paragons=([]);


reset(arg)
{
  ::reset(arg);
  if(arg) return;
  set_short("The Room of Paragons");
  set_long("The air is full of dignity, this chamber is for the Paragons\n"+
	   "in NannyMud. They gather in this room and discuss the future\n"+
	   "of NannyMud and advance to new levels.\n"+
	   "The curious can do cost to gain some information.\n");
  
  add_item(({"wall","walls"}),"The walls are made of marble.\n");
  add_item("floor","The floor is made of finest mahogny.\n");
  add_item("ceiling","The ceiling is painted white.\n");
  add_exit("west","paragon_entry_room"); 
  add_property("indoors");
  set_light(1);
  call_out("fix_board",2);
  call_out("check_for_wiz",2);
  restore();
}

restore()
{
  restore_object(FILE);
}

save()
{
  save_object(FILE);
}

fix_board()
{
  object board;
  if(!present("board",this_object())) {
    board=clone_object("obj/bboard");
    board->set_name("/bboard/paragon");
    board->set_max_num(50);
    move_object(board,this_object());
  }
}

init()
{
  if(this_player()->query_level()<19&&!this_player()->query_paragon())
  {
    tell_room(this_object(),capitalize(this_player()->query_real_name())+" is tossed out!\n");
    transfer(this_player(),load_object("/room/paragon_entry_room"));
    return ;
  }
  ::init();
  add_action("advance","advance");
  add_action("cost","cost");
  add_action("list","list");
  add_action("press","press");
  add_action("set_al_title", "alignment");
  add_action("set_al_title", "al");
  if(this_player()->query_paragon())
    if(!present("the_aura_of_paragon",this_player()))
      press("foo");
}

press(string s)
{
  object ob;  
  if(present("the_aura_of_paragon",this_player()))
    return write("You already have the aura of Paragon.\n"),1;
  ob=clone_object("/std/special/paragon/paragon_obj");
  move_object(ob,this_player());
  ob->fix();
  write("You received your true aura of Paragon.\n");
  return 1;
}

join()
{
  object room,ob;

   if(present("the_aura_of_paragon",this_player()))
    return write("You already have the aura of Paragon.\n"),1;
    
  ob=clone_object("/std/special/paragon/paragon_obj");
  move_object(ob,this_player());
  ob->fix();

  write("You received your true aura of Paragon.\n");
  room=load_object("/std/special/paragon/send_room");
  if(room)
    tell_room(room,capitalize(this_player()->query_real_name())+" has become Paragon!\n");
    
}

list()
{
  string out;
  mixed all;
  int i;

  all=m_indices(paragons);
  for(i=0;i<sizeof(all);i++)
    all[i]=capitalize(all[i]); 
  out="These are the Paragons:\n";
  out+=sprintf("%-*#s",78,implode(all,"\n"));
  write(out+"\n");
  return 1;
}

query_cost(sl)
{
  return sl*COST;
}

ok_to_advance(object ob)
{
  int qp,qpmax,qp_percent,sl,must;
  
  sl=this_player()->query_paragon_level(); 
  qp=ob->query_quest_points();
  qpmax=ob->query_my_quest_max();
  qp_percent=100*qp/qpmax;

  if(qp_percent < QUEST_POINTS_PERCENT_TO_WIZ)
    return -2;

  must=(QUEST_POINTS_PERCENT_TO_WIZ+(PERCENT_PER_SL*sl));
  if(must>95) must=95;

  if(must < QUEST_POINTS_PERCENT_TO_WIZ) return -1;

  if(qp_percent < must)
    return -1;

  return 1;
}

add_to_list(string who,int pl)
{
  paragons[who]=(pl+1);
}

query_paragons()
{
  return paragons;
}

advance(string str)
{
  int exp,level,res,cost;

  notify_fail("Advance what?\n");
  if(str=="paragon level"||str=="sl"||str=="level") {

    if(!this_player()->query_paragon()) 
      {
	write("You are no Paragon, so you cannot advance level.\n");
	return 1;
      }

    if(this_player()->query_level()<19) 
      {
	write("You were unable to advance paragon level.\n");
	return 1;
      }

    res=ok_to_advance(this_player());
    switch(res)
    {
    case -2:
      write("You must have enough QP to become wizard.\n");
      return 1;
    case -1:
      write("You must have more QP to advance Paragon Level.\n");
      return 1;
    default: break;
    }
    
    level=this_player()->query_paragon_level();
    level++;
   
    exp=this_player()->query_exp()-BASE;
    cost=query_cost(level);
    if(exp<cost)
    {
      write("You don't have enough experience points to advance level\n");
      return 1;
    }
    this_player()->add_exp(-cost);
    this_player()->set_paragon_level(level); 
    write("You now have Paragon level : "+level+"\n");
    add_to_list(this_player()->query_real_name(),level);
    save();
    return 1;
  }
}

cost()
{
  int level,exp,cost;
  
  level=this_player()->query_paragon_level(); 
  level++;
  exp=this_player()->query_exp()-BASE;
  cost=query_cost(level);
  if(exp<cost)
    write("You need "+(cost-exp)+" more expericence points.\n");
  else 
    write("You have enough experience points.\n");
  if(ok_to_advance(this_player())<0)
    write("You need to have more quest points to advance.\n");
  else
    write("You have enough quest points to advance.\n");
  return 1;
}

/* This is was added to remomve players that has wizzed.*/
check_for_wiz()
{
  mixed ind,res;
  int i;
  
  ind=m_indices(paragons);
  for(i=0;i<sizeof(ind);i++)
  {
    res = FINGERD->get_player_stats(ind[i]);
    if (res && pointerp(res) && res[5]>19) _m_delete(paragons,ind[i]);
  }
  save();
}

set_al_title(str)
{
  if (!objectp(this_player()) || !this_player() -> query_paragon()) return 0;

  if (!stringp(str))
  {
    notify_fail("Perhaps a description would do?\n");
    return 0;
  }
  
  if (!ok_desc(str)) return 1;

  write("You change the description of your alignment.\n");
  say(this_player() -> query_name() +
      " changed " +
      this_player() -> query_possessive() +
      " alignment description to " + str + ".\n");
  
  this_player() -> set_alignment(str);
  return 1;
}

ok_desc(str)
{
  if (strlen(str) > 15)
  {
    write("Use at most 15 characters!\n");
    return;
  }

  if (sscanf(str, "%*s %*s") == 2 ||
      sscanf(str, "%*s+%*s") == 2 ||
      sscanf(str, "%*s=%*s") == 2 ||
      sscanf(str, "%*s-%*s") == 2 ||
      sscanf(str, "%*s_%*s") == 2)
  {
    write("Use normal letters, only!\n");
    return;
  }
  
  write_file("log/SECURE/PARAGON",
	     "al_title: " +
	     this_player() -> query_real_name() +
	     " to " + str + "\n");
  return 1;
} 
