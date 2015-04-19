inherit "room/room";

init() 
{
  ::init();
  if (call_other(this_player(), "query_level", 0) < 20) {
    write("Hey! You! Puny mortal! " +
	  "What are you trying to do in here? Get out!\n");
    call_other(this_player(),"move_player", "X#room/adv_guild");
    return;
  }
}

reset(arg) 
{
  object board;
  if(arg) return;
  set_short("The inner room");
  set_long(
    "This is a room for discussion of the RULES.\n" +
    "Only wizards can access this room.\n");

  add_exit("room/adv_inner", "east");
  add_exit("room/inner5", "west");
  add_exit("room/inner6","up");
  add_exit("room/inner7","north");
  add_exit("room/prop_room","south");
  set_light(1);
  set_indoors();
  board=clone_object("obj/newboard");
  board->set_name("RULES","The RULES wizardboard",
		  "This is the board for discussion of the RULES in Nanny.\n"+
		  "Add a note here if you want help, or have a question.\n"+
		  "The RULES can be read in /doc/build/RULES.\n",
		  "Milamber",772482354,21);
  move_object(board,this_object());
}

query_prevent_follow(ob)
{
  return (!ob || ob->query_level() < 20);
}
