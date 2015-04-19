/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "room/room";

object *duellers;

init() {
  add_action("quit","quit");
}

reset(arg) {
  if (arg)
    return;

  if(!present("duel_snooper"))
    load_object("/obj/duel_snooper");
     
  set_light(1);
  set_short("Duel place");
  set_no_castle_flag(1);
  set_long("You are standing in a small open space,\n" +
	   "where duels have been fought since ancient times.\n" +
	   "In this place, players are allowed to kill each other\n" +
	   "without any interference from law, conscience or\n" +
	   "guardian angels.\n");
  set_outdoors();
  set_realm("NT");
  add_property("allow_pk");
} /* reset */

query_playerkilling() { return 2; }

string looser;
object loos;

player_died() {
  looser=previous_object()->query_name();
  loos=previous_object();
  remove_call_out("duel_end");
  remove_call_out("duel_end");
  remove_call_out("duel_end");
  call_out("duel_end",3);
}

duel_end() {
  int i;
  object o;
  tell_room(this_object(), "The fight is over and "+looser+
	    " was the loser.\n", ({loos}));
  tell_object(loos, "The fight is over and you are the loser.\n");
  for(i = 0; i<sizeof(duellers); i++) {
    duellers[i]->reset_kill_callbacks();
    tell_object(duellers[i], "You are teleported back to the entrance.\n");
    move_object(duellers[i], "/room/dark_duel_entrance");
    if(o=duellers[i]->query_duel_shadow())
      destruct(o);
    if(o=present("DUEL OBJECT", duellers[i]))
      destruct(o); }
  if(o = present("corpse")) {
    move_object(o, "/room/dark_duel_entrance");
    if(o=present("DUEL OBJECT", o))
      destruct(o);
  }
  reset_duel();
}

quit() {
  return write("You cannot quit until one of you are dead.\n"),1;
}

set_duellers(object *who) { duellers = who; }

query_duel() { return duellers; }

reset_duel() { duellers = 0; }

