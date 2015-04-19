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
    "This is a room for discussion of the GUILDS.\n" +
    "Only wizards can access this room.\n");

  add_exit("room/inner2", "south");
  set_light(1);
  set_indoors();
  board=clone_object("obj/bboard");
  board->set_name("/bboard/guild_board");
  board->set_max_notes(100);
  move_object(board,this_object());

}

query_prevent_follow(ob)
{
  return (!ob || ob->query_level() < 20);
}
