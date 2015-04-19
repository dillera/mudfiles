inherit "std/room";
reset(arg) {

  if (arg)
    return;

  add_property("indoors");
  set_light(1);
  add_exit("west","/examples/room1");
  add_exit("east","/examples/room3");
  set_short("Example room 2");
  set_long(          
    "This is the second room in a series of examples of LPC code.\n" +
    "It is a very simple room and it has no features at all.\n" +
    "It has two exits: one leading west, to the first example room,\n" +
    "and one leading east, to the next example room.\n" +
    "Notice that for a door between two rooms to work both ways,\n" +
    "you have to define it as an exit in both rooms.\n");

} /* reset */


