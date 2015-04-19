/* Rewritten using add and set functions in room.c. Banshe 950217 */

inherit "room/room";

reset(arg) {
  if(arg) return;
  set_long("You are standing outside the place know as the\n"+
	   "duel place. That is where fights are being fought\n"+
	   "without laws and conscience. It's so frightening that\n"+
	   "even the guardian angels avoid it. There is a large \n"+
	   "sign standing here.\n");
  add_item(({"sign","large sign"}),"It's a large wooden sign. Something is "+
	   "written on it");
  add_item("place","It's a small open space. Once you entered, you cannot leave");
  
  set_short("Entrance to duel place");
  set_light(1);
  set_outdoors();
  set_no_castle_flag(1);
  add_exit("room/dark_place", "east");
  add_exit("room/monitor_room", "northwest");
} /* reset */

init() {
  ::init();
  add_action("duel_fun", "duel");
  add_action("read_fun", "read");
}

duel_fun(string who) {
  object o,inv;
  if(!stringp(who))
    return notify_fail("Duel who?\n");
  if(load_object("/room/dark_duel")->query_duel())
    return notify_fail("There is already a duel goin on.\n"),0;
  who=lower_case(who);
  if(who == this_player()->query_real_name())
    return notify_fail("You cannot duel yourself.\n"),0;
  if((o = present(who,this_object())) && interactive(o)) {
    if(this_player()->query_ghost())
      return notify_fail("You cannot fight in your immaterial state.\n"), 0;
    if(o->query_ghost())
      return notify_fail("You cannot hardly see "+capitalize(who)+" in "+
			 o->query_possessive()+" immaterial state.\n"+
			 "It's impossible to fight with a ghost!\n"), 0;
    if(this_player()->query_level()<20 && o->query_level()<20) {
      if(present("dueller query", o))
	return notify_fail(capitalize(who)+" is already asked to duel.\n"),0;
      write("You ask "+capitalize(who)+" to duel you.\n");
      inv = clone_object("/obj/duel_object");
      inv -> set_dueller(this_player());
      move_object(inv,o);
      return 1; } 
    return notify_fail("You or/and your opponent has archived wizardlevel.\n"+
		       "Therefore you cannot duel here.\n"),0; }
  return write("I see no player with that name here.\n"),1;
} /* duel_fun */

read_fun(string what) {
  if(what == "sign" || what == "large sign") {
    write("The sign says: You can 'duel' someone here.\n"+
	  "               Remember that a duel is a fight till\n"+
	  "               one of the duellers dies. You cannot \n"+
	  "               quit or teleport away from the fight.\n"+
	  "               You will loose experience and equipment\n"+
	  "               as usual.\n"+
	  "               If you are interested in watching a fight,\n"+
	  "               the best place to be is northwest of here.\n");
    return say(this_player()->query_name()+" reads the large sign.\n"),1; }
  return notify_fail("Read what?\n"),0;
}
