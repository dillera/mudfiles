inherit "room/room";

object the_monster;

setup_room(arg) {
  if (arg)
    return;

  set_light(1);
  set_indoors();
  add_exit("players/anna/examples/room11", "west");
  add_exit("players/anna/examples/room14", "east",0,"check_east");

  set_short("Example room 13");
  set_long(
    "This is room number 13 in a series of examples of LPC code.\n" +
    "It is intended to show you how to write a monster that blocks an exit.\n"+
    "As long as the small but stubborn monster is here,\n" +
    "you will not be able to go through the eastern door to room number 14.\n");

} /* setup_room */

make_the_monster() {
  the_monster = clone_object("obj/smartmonster");
  the_monster->set_name("small but stubborn monster");
  the_monster->add_alias("monster");
  the_monster->add_alias("small monster");
  the_monster->add_alias("stubborn monster");
  the_monster->add_alias("small stubborn monster");
  the_monster->set_short("a small but stubborn monster");
  the_monster->set_long("This is a small monster.\n" +
    "It looks very stubborn, though, and not likely to let you pass.\n");
  the_monster->set_level(1);
  the_monster->set_al(-10);
  the_monster->set_neuter();
  the_monster->set_aggressivity(0);
  the_monster->set_ac(1);
  the_monster->set_wc(1);

  move_object(the_monster, this_object());
} /* make_the_monster */


/*  We could of course have put all the code directly in the "reset" function,
 *  but we have split it up into two functions, "setup_room" and
 *  "make_the_monster", just to make it clearer.
 */
reset(arg) {
  if (!arg)
    setup_room();           /* Done in the first reset -- always do this first! */
  if (!the_monster)
    make_the_monster();     /* Done if the monster doesn't exist */
} /* reset */

check_east() {
  if (the_monster && present(the_monster, this_object())) {
    write("The monster stands in your way and won't let you pass.\n");
    say("The monster stands " + this_player()->query_name() +
      "'s way won't let " + this_player()->query_objective() + " pass.\n");
    return 1;
  }
  else
    return 0;
} /* check_east */
