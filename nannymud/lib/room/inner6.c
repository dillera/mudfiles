inherit "room/room";

object board;

reset(arg) 
{
  if (!arg) {
    set_light(1);
    short_desc = "The inner room";
    long_desc = "This is a room for approvals.\n" +
      "Things you want to have approved should be\n"+
	"noted here. Other wizards may comment on it.\n"+
	  "Only wizards can access this room.\n";
    dest_dir =({"room/inner2", "down",
		"room/project_board","east"});
    board = clone_object("obj/newboard"); 
    board->set_name("approval","The approval board",
		    "Post a note here if you want someting approved.\n"+
		    "Other wizards may comment on it.",
		    "Milamber",
		    773051023,21); 
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
