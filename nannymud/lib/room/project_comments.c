inherit "room/room";

object board;

void init()
{
    ::init();
    if (call_other(this_player(), "query_level", 0) < 20) {
        write("Hey! You! Puny mortal! " +
	      "What are you trying to do in here? Get out!\n");
	call_other(this_player(),"move_player", "X#room/adv_guild");
	return;
    }
}
    
void reset( int flag )
{
    if( flag )
	return;
    dest_dir = ({ "room/project_board", "east",
                 "room/inner6", "west" });
    short_desc = "The room of the comments to projects board.";
    long_desc = line_break("This is the room with the comments to " +
			       "projects board\n" +
			       "On the comments board you can discuss the " +
			       "various projects described on the project " +
			       "board.", 79) + "\n";
    set_light(1);
    board = clone_object("obj/bboard");
    board -> set_name("bboard/comments");
    move_object( board, this_object() );
}

int inorout=1;

