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
  add_action("south","south");   
}

reset(arg) 
{
  object board;
  if(arg) return;
  dest_dir = ({"room/adv_guild", "north",
/*    	       "room/election","down",  */
	       "room/inner2","west",
	       "room/inner3","south",
	       "room/inner4", "east",
	       "room/project_board", "up"});
  short_desc = "The inner room of the Adventurers' Guild";
  long_desc = "This is the inner room of the Adventurers' Guild.\n" +
    "To the east you can see a bulletin board with a header\n" +
      "saying 'Old but interesting notes'. And to the west you\n" +
	"you can see a bulletin board with a header saying\n" +
	  "'RULES discussion'. To the south is the Archwizards' room.\n" +
	    "A staircase is leading up to the project room.\n" +
	      "Only wizards can access this room.\n";
  set_light(1);
#if 1
  board=clone_object("obj/newboard");
  board->set_name("wizboard");
#else
  board=clone_object("obj/bboard");
  board->set_name("/bboard/wizboard",50);
  board->set_write_level(21);
  board->set_read_level(20);
#endif
  move_object(board,this_object());
}

/*
 * this is to prevent the _easy_ way for the wizards 
 * to read the 23+ board  /Anvil
 */

south() {
  if (this_player()->query_level() < 22) {
    write("A strong magical force prevents you from entering the Archwizards' room.\n");
    return 1;
  }
  this_player()->move_player("south#room/inner3");
  return 1;
}
int inorout=1;

query_prevent_follow(ob)
{
  return (!ob || ob->query_level() < 20);
}
