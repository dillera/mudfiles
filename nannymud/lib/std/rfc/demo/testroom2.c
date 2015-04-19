inherit "/std/room";
#include </std/rfc/room.h>

void reset(int arg)
{
  object o;
  ::reset(arg);

  if(!arg)
  {
    set_light(1);
    set_short("A testroom");
    set_long(LONG_HTML("<ispresent profezzorn>You are in the peresence of Profezzorn.\n</ispresent>You are in the testroom.\nThere is a trashcan here.\n"));
    
    add_exit("south",this_object());    
    add_exit("west",this_object(),0,BLOCK_IF_PRESENT("guard"));
    ADD_HIDDEN_EXIT("east","testroom");

    add_item("trashcan",ITEM_FIND("a knife","It's full of trash.\n"));
    add_item_cmd("search","trashcan",CMD_FIND("a knife","\bPRON search\b{|es} the trashcan without finding anything.\n");
    add_object("a guard");
  }
}
