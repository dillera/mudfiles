inherit "room/room";

init() {
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
    short_desc = "The inner room";
    long_desc = "This is a room for discussion of LPC.\n" +
                "Only wizards can access this room.\n";

    dest_dir =({"room/inner2", "east",
		"room/inner4", "west",
		"room/inner8", "south",
		"/doc", "down",
		});

  set_light(1);
  board=clone_object("obj/newboard");
  board->set_name("LPC","The LPC board",
		  "This is the board for discussion of the LPC"+
		  "programming language.\n"+
		  "Add a note here if you want help, or have a question.\n",
		  "Milamber",772482354,21);
  move_object(board,this_object());
}

int inorout=1;

query_prevent_follow(ob)
{
  return (!ob || ob->query_level() < 20);
}
