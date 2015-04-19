/* A board for general discussions about MUD's.  /Slater */
/* Changed a bit by Rohan 950904 */
inherit "/std/room";

reset(arg) 
{
object board;
  if (arg) return;

  set_light(1);
  add_property("indoors");
  add_property("no_fight");
  add_property("no_magic");
  set_short("The MUD board room");
  set_long("This is a room for discussions about MUD's in\n"+
	    "general. You can write anything you want here, \n"+
	    "like if you start a new MUD.\n");
  add_exit("down","/room/adv_guild");

  board = clone_object("obj/newboard"); 
  board->set_name("MUDboard","The MUD discussion board",
	    "This is a room for discussions about MUD's in\n"+
	    "general. You can write anything you want here,\n"+
	    "like if you start a new MUD.\n","Slater",-10000000,2); 
  move_object(board, this_object()); 
}
