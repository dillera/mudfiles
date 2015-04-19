inherit "/players/wing/winglib";
inherit "/room/room";

object board;

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
    
reset( flag )
{
    if( flag )
	return;
    row_length = 79; 
    dest_dir = ({ "/room/adv_inner", "down",
		  "/players/wing/project_comments", "west" });
    short_desc = "The room of the project board";
    long_desc = wing_word_wrap("This is the room with the project board.\n" +
			       "On the project board you can tell other " +
			       "wizards what you're coding currently. " +
			       "There should not be any discussions on " +
			       "this board as the space is limited, use " +
			       "the comments board instead (to the west)"
			       , row_length);
    set_light(1);
    board = clone_object("obj/bboard");
    board -> set_name("bboard/project");
    move_object( board, this_object() );
}

int inorout=1;

