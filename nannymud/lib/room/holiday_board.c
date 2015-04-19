/* A board for general discussions about MUD's.  /Slater */


inherit "/std/room";

object board;

reset(arg) 
{
  if (arg) return; 

  set_light(1);
  add_property("indoors");
  add_property("no_magic");

  add_property("no_fight");

  set_short("The Holiday board room");
  set_long(("This room is for greetings and farewells to your friends in Nannymud.\n"));
  add_exit("east","/room/adv_guild");

  board = clone_object("obj/newboard"); 
  board->set_name("holidayboard", "The holiday board",
		  "This board is for greetings and farwells to your friends in NannyMUD\n",
		  "Slater", 770000000, 2); 
  move_object(board, this_object()); 
}


