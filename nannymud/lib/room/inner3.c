inherit "room/room";

init() {
  if (!interactive(this_player()) ||
      this_player()->query_level_sec() < 22) {
    write("A strong magical force prevents you from entering "+
	  "the Archwizards' room.\n");
    this_player()->move_player("X#room/adv_inner");
    return; 
  }
  ::init();
}

int query_prevent_shadow()
{
  return 1;
}

reset(arg) { 
  if (!arg) {
    object board;
    short_desc = "The inner room";
    long_desc = "This is a room for discussion of LPC.\n" +
                "Only wizards can access this room.\n"+
	        "To the metawest, you see an evaluationcopy of\n"+
		"Milambers new bulletin-board system.\n";

    dest_dir =({"room/inner2", "east",
		"room/inner4", "west",
		"room/inner42", ({"metawest","mw"})});
    dest_dir = ({"room/adv_inner", "north"});
    short_desc ="The Archwizards' Room";
    long_desc= "Only the God, Elder wizards, Arch Sourcerors, Arch wizards\n" +
               "and High wizards can access this room.\n";
    set_light(1); 
    board=clone_object("obj/newboard");
    board->set_name("arch","The Arch- and Highwizards own board",
		  "Go ahead, make my day, write a note.\n",
		  "Milamber",772482354,23);
    move_object(board,this_object());

  }
}

int inorout=1;
