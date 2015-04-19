inherit "room/room";

object the_monster;

setup_room() {
    set_light(1);
    add_exit("players/anna/examples/room9", "west");
    add_exit("players/anna/examples/room11", "east");

    set_indoors();
    set_short("Example room 10");
    set_long(
        "This is example room number 10.\n" +
        "The reason for this room is to show you the correct way to put\n" +
        "a monster in a room from the reset function.\n" +
        "The trick is to FIRST define all exits and commands,\n" +
        "and THEN put the monster in the room.\n" +
        "If you do it the other way around, the first monster that is\n" +
        "created (at the first reset), will not be able to use the commands\n" +
        "in the room, so it will not be able to leave.\n" +
        "So, if you want a monster that can do things that are defined as\n" +
        "commands in the room, or that can leave the room (for example if it\n" +
        "is wimpy), then FIRST define the room, and THEN put the monster in it!\n");
    add_item( ({ "floor", "wooden floor" }),
             "The floor is made of rough wooden planks",
             "sweep_floor", "sweep");
} /* setup_room */

/* This function creates the monster */
make_the_monster() {
    the_monster = clone_object("obj/smartmonster");
    the_monster->set_name("coma");
    the_monster->add_alias("sanitary technician");
    the_monster->add_alias("cleaning woman");
    the_monster->set_short("Coma the sanitary technician");
    the_monster->set_long("This is Coma.\n" +
                          "She is a member of the cleaning staff.\n" +
                          "She used to be a cleaning woman,\n" +
                          "but she is now a sanitary technician.\n" +
                          "She will sweep the floor for you, if you tell her to.\n");
    the_monster->set_level(2);
    the_monster->set_al(50);
    the_monster->set_female();
    the_monster->set_aggressivity(0);
    the_monster->set_ac(4);
    the_monster->set_wc(10);

    move_object(the_monster, this_object());

    the_monster->set_soul();

    the_monster->add_response("arrives",
                              ({ "Coma says: Hi there, $OTHER!\n" }));
    the_monster->add_say_response(({"clean", "sweep"}),
                                  ({"Coma says: Ok, $OTHER, I'll sweep the floor!\n", "!sweep floor"}));
} /* make_the_monster */


/*  We could of course have put all the code directly in the "reset" function,
 *  but we have split it up into two functions, "setup_room" and
 *  "make_the_monster", just to make it clearer.
 */
reset(arg) {

    if (!arg)
        setup_room();   /* Done in the first reset */
    if (!the_monster)
        make_the_monster();     /* Done if the monster doesn't exist */
} /* reset */

sweep_floor(str) {
    /* str will match the floor, since this function is called by room/room */
    write("You sweep the floor. Well done!\n");
    say(this_player()->query_name() + " sweeps the floor.\n");
    return 1;
} /* sweep_floor */
