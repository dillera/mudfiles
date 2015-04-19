inherit "std/room";

/*  This function, "reset", is called with argument 0 when the room is created,
 *  and with argument 1 each hour or so. It is used both to set up the room
 *  initially, for example its description and the exits, and to re-stock
 *  the room if contains monsters or treasure.
 *
 * Now below there will be a short discussion about some useful terms.
 * global variable : the global variable can be used in _all_ functions
 *                   in the object.
 * local variable  : the variable can only be used within the function
 *                   declaring it.
 */

int i; /* This is a GLOBAL variable */

reset(arg) {
  object monster; /* This is a local variable.*/
  if (arg)
    return;
  add_property("indoors");
  set_light(1);
  add_exit("east","/examples/room2");
  set_short("Example room 1");
  set_long(
	   "This is the first room in a series of examples of LPC code.\n" +
	   "It is a very simple room and it has no features at all.\n" +
	   "You can leave east, to the next example room.\n"+
	   "Type 'more' to the see the code of this room,\n"+
	   "you can always do that if you may read the code.\n");
} /* reset */


