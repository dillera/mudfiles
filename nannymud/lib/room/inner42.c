inherit "room/room";

object board;

reset(arg) {
  if (!arg) {
    set_light(1);
    short_desc = "The inner room, take 12";
    long_desc = "This is an evaluationcopy of\n"+
		"Milambers new bulletin-board system.\n";

    dest_dir =({ "room/inner5", "away"});
    board = load_object("players/milamber/board_obj"); 
    move_object(board, this_object()); 
  }
}

init() {
    ::init();
    if (call_other(this_player(), "query_level", 0) < 20) {
        write("Hey! You! Puny mortal! " +
	      "What are you trying to do in here? Get out!\n");
	call_other(this_player(),"move_player", "X#room/adv_guild");
	return;
    }
}

int inorout=1;
