inherit "room/room";

object board;

reset(arg) {
  if (!arg) {
    short_desc ="The inner room";
    long_desc = "This is a room for old but interesting notes.\n" +
                "Only wizards can access this room and only arch wizards\n" +
		"can write on this board.\n";
    dest_dir = ({ "room/adv_inner", "west",
		  "room/inner5", "east"});
    board = clone_object("obj/bboard"); 
    board->set_name("board/archives");
    board->set_write_level(25);
    move_object(board, this_object()); 
    set_light(1);
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
