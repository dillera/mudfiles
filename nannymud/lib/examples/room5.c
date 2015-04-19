inherit "std/room";

reset(arg) {
  if (arg)
    return;

  add_property("indoors");
  set_light(1);
/* add_exit(destination,command to go there,exit message if not the usual,
     check function) */
  add_exit("west","room4");
  add_exit("east","room6");
  add_exit("church","/room/church","through the magic door to the church");
  add_exit("guild","/room/adv_guild", 
    "through the magic door to the adventurers' guild");
  add_exit("post","/room/post","through the magic door to the post office");

  add_exit("wizroom","/room/adv_inner",
    "through the magic door to the wizards' room","@check_wiz()");

  set_short("Example room 5");
  set_long(
    "This is the fifth room in a series of examples of LPC code.\n" +
    "It's similar to room 4, but uses some more of the features\n" +
    "in NannyMUD's room object.\n" +
    "There is a big red button on the wall that you can press.\n" +
    "It has two normal exits: one leading west, to example room 4,\n" +
    "and one leading east, to example room 6.\n" +
    "There are also two magic exits leading to the Village Church\n" +
    "and to the Adventurers' Guild.\n");



  add_item(({ "walls", "stone walls", "wall", "stone wall" }),
    "The walls are made of stone and are bare, except for a big red button.");
  add_item("ceiling","There are some spiderwebs in the ceiling.");
  add_item(({ "spiderweb", "spiderwebs", "web", "webs" }),
    "The spiderwebs in the ceiling are too high up for you to reach.");
  add_item(({ "exit", "exits", "door", "doors" }),
    "The two doors leading east and west seem ordinary enough,\n" +
    "but those magic exits are really weird-looking.");
  add_item(({ "magic exit", "magic exits", "magic door", "magic doors" }),
    "The magic exits look almost like ordinary doors, but they are one-way magic portals.");
  add_item(({ "floor", "wooden floor" }),
    "The floor is made of rough wooden planks.");
  add_item(({ "button", "red button", "big button", "big red button" }),
    "A big red button on the wall. It looks as if it can be pressed.");

  add_item_cmd("sweep","floor","You sweep the floor.\n");
  add_item_cmd("dust","walls","@dust_walls()");

  add_item_cmd(({"press","push"}),"button","@press_button()");
  add_command("freak %s","@freak_out()");

} /* reset */

check_wiz() {
  if (this_player()->query_level() < 20) {
    write("You bang your head against a sign saying 'Wizards only. Sorry.'\n");
    return 1;
  }
  else
    return 0;
} /* check_wiz */

dust_walls() {
  write("You dust the walls a little.\n");
  say(this_player()->query_name() + " dusts the walls.\n");
  return 1;
} /* dust_webs */

press_button() {
  write("You pressed the big red button on the wall. Nothing happens.\n");
  say(this_player()->query_name() +
    " pressed the big red button on the wall. Nothing happens.\n");
  return 1;
} /* press_button */

freak_out(arg,str) {
/* arg contains what is between the () in the 
   add_command("freak %s","@freak_out()");. 
                                     ^^
   In this case it is nothing and arg is "". (An empty string.)

   str is everything that the player wrote after 'freak'.
   example: The player writes 'freak foo', then str is "foo". */

  if (str != "out") {
    /* notify_fail is used to give error messages. You use it when something 
       went wrong. If you instead 'write' the text and take the command 
       (by return ing a 1) no other object that could have take the command 
       gets a chance. 
       When you use notify_fail the text will only be written to the player 
       if no other object takes the command. */

    notify_fail("You can only freak out in this room. No other freaking is possible.\n");
    return 0;
  }
  else {
    write("You freak out.\n");
    say(this_player()->query_name() + " freaks out.\n");
    return 1;
  }
} /* freak_out */

