inherit "std/room";

reset(arg) {
  if (arg)
    return;

  add_property("indoors");
  set_light(1);

 /* The exit to a room can be defined in two ways. Either by using an 
    absolute path as in the first case, or with a relative path as 
    in the second case. */
  add_exit("west","/examples/room3");
  add_exit("east","room5");     

  set_short("Example room 4");
  set_long(
    "This is the fourth room in a series of examples of LPC code.\n" +
    "It is similar too room three, since it has a floor, walls,\n" +
    "and a ceiling, but in this room you can also do some things:\n" +
    "you can sweep the floor ('sweep floor'),\n" +
    "you can dust the walls ('dust walls'),\n"+
    "and you can freak out ('freak out').\n" +
    "To define the things that can be done you can use either add_item_cmd\n"+
    "or add_command. Use 'more to see the explanation on how to use them.\n");

  add_item(({ "walls", "stone walls", "wall", "stone wall" }),
    "The walls are made of stone and are bare.");
  add_item(({ "floor", "wooden floor" }),
    "The floor is made of rough wooden planks.");
  add_item("ceiling","There are some spiderwebs in the ceiling.");
  add_item(({ "spiderweb", "spiderwebs", "web", "webs" }),
    "The spiderwebs in the ceiling are too high up for you to reach.");

  /* These three actions has been defined in three different ways.
     The first and second uses the command 'add_item_cmd'.
     The first argument in 'add_item_cmd' is the verb to use.
     The second is which of the above defined items who should react.
     This means that you can both 'sweep floor' and 'sweep wooden floor'
     with the same result.
     The third argument is the reaction.
     It the 'sweep floor' case the reaction is just a text to the player.
     In the 'dust walls' case we want a little more complicated resonse
     and call the function 'dust_walls'.
     
     The third command 'freak out' is defined differently. We have no item
     to react on, and thus we use the command 'add_command' instead.
     The first argument is what to react on, and the second is the reaction.
     The reaction can be just as in add_item_cmd: just a string or a 
     function call. */
  add_item_cmd("sweep","floor","You sweep the floor.\n");
  add_item_cmd("dust","walls","@dust_walls()");
  add_command("freak out","@freak_out()");
} /* reset */

dust_walls() {
  write("You dust the walls a little.\n");
  say(this_player()->query_name() + " dusts the walls.\n");
  return 1;
} /* dust_webs */

freak_out() {
  write("You freak out.\n");
  say(this_player()->query_name() + " freaks out.\n");
  return 1;
} /* freak_out */






