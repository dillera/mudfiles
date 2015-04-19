inherit "room/room";

reset(arg) {

  if (arg)
    return;

  set_indoors();
  set_light(1);
  add_exit("players/anna/examples/room13", "west");

  set_short("Example room 14");
  set_long(
    "This is the last of the example rooms.\n" +
    "It is a simple, empty room.\n" +
    "Try to make your own rooms more interesting than this one!\n");

} /* reset */


