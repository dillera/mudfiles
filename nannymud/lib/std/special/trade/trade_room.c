inherit "/std/room";

reset(arg)
{
  if(arg) return;
  set_short("Inside the Traders Tent");
  set_long("You stand within the Traders Tent, the floor is worn, "+
	   "a small trashpile lies in the corner. The walls are made "+
	   "of cloth. Here you may apply for a membership in Traders "+
	   "Club.\n"+
	   "The commands available to a Club Member is:\n"+
	   "Trade <str>: Talk on the Traders line.\n"+
	   "Trade: See all the Traders currently on.\n");
  add_exit("out","/room/forest1");
  add_item("floor","It is just black earth.\n");
  add_item(({"wall","walls"}),"Sturdy cloth, made by Dwarfish craftmen.\n");
  add_item(({"pile","trashpile"}),"In the pile lies old, broken items.\n");
  	   
  set_light(1);
}

init()
{
  ::init();
  add_action("apply","apply");
}

apply(str)
{
  object ob;
  if(!present("the_traders_mark",this_player()))
    {
      ob=clone_object("/std/special/trade/trade");
      move_object(ob,this_player());
      ob->join();
      write("You have joined the Traders Club.\n");
      say(this_player()->query_name()+" has joined the Club.\n",
	  ({this_player()}) );
      return 1;
    }
  write("You already belong to the Traders Club.\n");
  return 1;
}



