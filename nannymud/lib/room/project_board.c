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
  dest_dir = ({ "room/adv_inner", "down",
		"room/inner6", "west" });
  short_desc = "The room of the project board";
  long_desc = line_break("This is the room with the project board.\n" +
			 "On the project board you can tell other " +
			 "wizards what you're coding currently.\n"
			 , 79) + "\n";
  set_light(1);
  board = clone_object("obj/newboard");
  board -> set_name("projects","The Projects board",
		    "On this board you can tell other wizards what you"+
		    "\nare currently coding.\n","Wing",731314211,21);
  move_object( board, this_object() );
}

int inorout=1;

