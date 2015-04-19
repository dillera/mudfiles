inherit "room/room";
object the_monster;

reset(arg) {
  if (!the_monster)
    make_the_monster();


  if (arg)
    return;

  set_indoors();
  set_light(1);
  add_exit("players/anna/examples/room8", "west");
  add_exit("players/anna/examples/room10", "east");
  set_short("Example room 9");
  set_long(
    "In this example room, number 9, there is a more advanced\n" +
    "monster. The room is rudimentary, but the monster, which is a clone of\n"+
    "'obj/smartmonster', can do a few simple things.\n");

} /* reset */



/* This function creates the monster */
make_the_monster() {
  the_monster = clone_object("obj/smartmonster");
  the_monster->set_name("monster");
  the_monster->add_alias("big monster");
  the_monster->set_short("a big monster");
  the_monster->set_long("This is a big, beautiful monster with nine legs and a long tail.\n" +
    "It doesn't seem aggressive, but it would probably be dangerous\n" +
    "to attack it.\n" +
    "It looks fairly intelligent.\n");
  the_monster->set_level(10);
  the_monster->set_al(100);
  the_monster->set_neuter();
  the_monster->set_aggressivity(0);
  the_monster->set_ac(4);
  the_monster->set_wc(15);

  move_object(the_monster, this_object());

  the_monster->set_soul();

  the_monster->set_chat_chance(5);
  the_monster->set_chats(({
    "The monster waves its tail.\n",
    "The monster looks at you.\n"
  }));

  the_monster->add_attack(
    "The monster kicks $OTHER hard with one if its nine feet.\n",
    "The monster kicks you hard with one if its nine feet.\n", 8, 10);
  the_monster->add_attack(
    "The monster hits $OTHER with a sweep of its tail.\n",
    "The monster hits you with a sweep of its tail.\n", 5, 10);

  the_monster->set_say_handler(this_object(), "handle_say");
  the_monster->set_tell_handler(this_object(), "handle_say");

  the_monster->add_response("arrives",
    ({ "The monster says: Hi there, $OTHER. I'm a smartmonster!\n", "!strut" }));
  the_monster->add_response("leaves", "The monster says: Well, good riddance of that one!\n");
  the_monster->add_response("sweeps you across the dance floor", "!smile");
  the_monster->add_response("kicks you", "The monster doesn't even notice the kick.\n");
  the_monster->add_response("hugs you", "!kiss $LOWOTHER");
  the_monster->add_response("smiles", "The monster seems to like $OTHER.\n",
                              "The monster seems to like you.\n");
  the_monster->add_response("pukes", ({ "!kick $LOWOTHER", "!glare $LOWOTHER" }));

  the_monster->set_real_dead_ob(this_object());       /* Special for NannyMUD! */
} /* make_the_monster */

/*  Actually, these responses to "say" and "tell" could have been coded
 *  more easily with just these two calls:
 *      the_monster->add_say_response( {( "example", "examples" }),
 *              "The monster says: This is an example room, and I am an example monster!\n");
 *      the_monster->add_say_response("smartmonster",
 *                                    "The monster says: Yes, I'm a smartmonster!\n");
 *  But we have done it with "set_say_handler" and "set_tell_handler"
 *  just to show you have you can do more advanced things.
 */

handle_say(talker_obj, talker_name, phrase) {
string a, b;

  phrase = lower_case(phrase);
  if (sscanf(phrase, "%sexample%s", a, b) == 2) {
    say("The monster says: This is an example room, and I am an example monster!\n");
  }
  else if (sscanf(phrase, "%ssmartmonster%s", a, b) == 2) {
    say("The monster says: Yes, I'm a smartmonster!\n");
  }
} /* handle_say */

/* Called when the monster is about to die */
monster_died(poor_monster) {
object murderer;
    
  murderer = the_monster->query_attack();

  /*  When this function is called, this_player() is the player attacking the monster,
  *  so a simple "say" will not work!
  */
  poor_monster->heal_self(1000);
  say("Just as the monster is about to die, it miraculously recovers!\n", poor_monster);
  write("Just as the monster is about to die, it miraculously recovers!\n");
  say("The monster says: Ha ha! I'm invincible!\n", poor_monster);
  write("The monster says: Ha ha! I'm invincible!\n");
  return 1;
} /* monster_died */











