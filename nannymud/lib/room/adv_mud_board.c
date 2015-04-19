/* A board for general discussions about MUD's.  /Slater */

inherit "room/room";

reset(arg) 
{
  object board;
  if (!arg) 
  {
    set_light(1);
    inorout=1;
    short_desc = "The MUD board room";
    long_desc = ("This is a room for discussions about MUD's in\n"+
		 "general. You can write anything you want here, \n"+
		 "like if you start a new MUD.\n");
    dest_dir =({"room/adv_guild", "down"});
    board = clone_object("obj/newboard"); 
    board->set_name("MUDboard","The MUD discussion board",
		    "This is a room for discussions about MUD's in\n"+
		    "general. You can write anything you want here,\n"+
		    "like if you start a new MUD.\n","Slater",-10000000,2); 
    move_object(board, this_object()); 
  }
}

query_haven() { return 1; }	/* Padrone, May 11, 1992 */
query_property(str) { return str == "no_magic"; }  /* Celeborn, Feb 1994 */
