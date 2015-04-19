inherit "std/room";
reset(arg) {

  if (arg)
    return;

  add_property("indoors");

  set_light(1);
  add_exit("west","/examples/room2");
  add_exit("east","/examples/room4");
  set_short("Example room 3");
  set_long(
    "This is the third room in a series of examples of LPC code.\n" +
    "This room has some features - a floor, walls, and a ceiling.\n" +
    "You can look at them, with commands like\n" +
    "'look at floor', 'examine walls' etc.\n" +
    "These items are added by the add_item command.\n");

  add_item(({ "walls", "stone walls", "wall", "stone wall" }),
    "The walls are made of stone and are bare.");
  add_item(({ "floor", "wooden floor" }),
    "The floor is made of rough wooden planks.");
  add_item("ceiling","There are some spiderwebs in the ceiling.");
  add_item(({ "spiderweb", "spiderwebs", "web", "webs" }),
    "The spiderwebs in the ceiling are too high up for you to reach.");


} /* reset */


