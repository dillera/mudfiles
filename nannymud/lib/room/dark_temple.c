/* Rewritten using add and set functions in room.c. Banshee 950217 */

inherit "room/room";

reset(arg) {
  if (arg)
    return;
  set_light(1);
  set_short("Inside the Dark Temple");
  set_no_castle_flag(1);
  set_long("You are inside the Dark Temple, where you can abolish your \n"+
	   "guardian angel and enter the ranks of Those Who Kill Each Other.\n" +
	   "Use the command 'abolish guardian angel'.\n" +
	   "You can get more information with the command 'help playerkilling'.\n" +
	   "There is black stone altar here.\n");
  add_exit("room/dark_place", "west");
  add_item("altar","It's just an altar made of black stone");
  set_indoors();
} /* reset */

init() {
  ::init();

 /* Throw out newbies. */
  if(this_player()->query_level()<6) {
    call_out("throw_out_newbies",4,this_player()); } 
  else {
    /* Throw out druids, they must not abolish their guardian angel; /Helenette */
    if (present("druid_guild_cloak",this_player()) && this_player()->query_level()<20)
      call_out ("throw_out",5,this_player());
    else {
      add_action("cmd_abolish", "abolish");
      add_action("cmd_really", "really"); }
  }
} /* init */

throw_out_newbies(ob) {
  if (!objectp(ob) || environment(ob)!=this_object()) return;
  tell_object(ob,"This place is much to scary for a young adventurer like you!\n");
  ob->run_away(); 
}

throw_out (ob) {
  if (!objectp(ob) || environment(ob)!=this_object()) return;
  tell_object(ob,"The smell of human blood lingers in the air; you fancy to hear the "
	      + "whispering\nvoices of bodyless souls. Your pure druidic soul is hit by an "
	 + "incredible force\nof darkness and you sense how demons try to overtake "
	 + "you and possess you.\n\n");
  ob->run_away();
} /* throw out druids */

cmd_abolish(str) {
  string my_name;
  
  if (!str) {
    notify_fail("But what do you want to abolish?\n");
    return 0; }
  str = lower_case(str);
  if (str != "angel" && str != "guardian angel" && str != "my guardian angel") {
    notify_fail("You can only abolish your guardian angel here.\n");
    return 0; }
  write("Are you really sure you want to do this?\n");
  write("If you haven't already done so, type 'help playerkilling',\n" +
	"and when you have read the information,\n" +
	"you can type 'really abolish guardian angel'!\n");
  my_name = this_player()->query_name();
  say(my_name + " just tried to perform an old and ominious ritual at the altar,\n" +
      "but didn't find the right words this time.\n");
  return 1;
} /* cmd_abolish */

cmd_really(str) {
  string my_name;
  
  if (!str) {
    notify_fail("Really what?\n");
    return 0; }
  str = lower_case(str);
  if (sscanf(str, "abolish %s", str) != 1) {
    notify_fail("The only thing you can really do here, is to abolish your guardian angel.\n" +
		"Type 'really abolish guardian angel'!\n");
    return 0; }
  if (str != "angel" && str != "guardian angel" && str != "my guardian angel") {
    notify_fail("You can only abolish your guardian angel here.\n");
    return 0; }
  if(this_player()->query_real_name() == "guest") {
    write("Guest can't abolish " + (this_player()->query_possessive()) + " guardian angel.\n");
    return 1; }
  write("You perform the old and ominious ritual at the altar.\n");
  my_name = this_player()->query_name();
  say(my_name + " performs an old and ominious ritual at the altar.\n");
  if (this_player()->query_playerkilling()) {
    write("Nothing happens. Haven't you done this before?\n"); }
  else {
    write("You have the feeling of something leaving you for ever,\n" +
	  "and you feel free, unprotected and alone.\n");
    this_player()->set_playerkilling(1); }
  return 1;
} /* cmd_really */

