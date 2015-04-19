inherit "room/room";
#include <door.h>

object obj_1, obj_2;    /* The MAKE_DOORS macro needs these two variables! */
object key;

reset(arg) {
  if(!arg) {
   /* First reset: */
  /* There are some code in the other room (12) look at that code too. */


  MAKE_DOORS("players/anna/examples/room11", "north",
    "players/anna/examples/room12", "south",
    "iron", "key_code_1",
    "This is the door between example room 11 and example room 12.\n" +
    "It is a thick and solid wooden door, and it has a sign saying 'EXAMPLE'.\n",
    1, 1, 1);
  }
  if (arg) {
    /* All resets except the first one: */
    obj_1->set_closed(1);
    obj_1->set_locked(1);
    obj_1->set_both_status(); 
  }

  if (!key || !present(key, this_object())) {
    MAKE_KEY(key, "iron", "key_code_1")
    move_object(key, this_object());
  }

  if (arg)
    return;

  set_indoors();
  set_light(1);

/*  Note: We have three exits here.
 *  The third exit is not really needed,
 *  since the door will both define the exit command ("north")
 *  and say that there is "a door to the north ( closed )".
 *  But since everyone can see the door,
 *  it looks better if the room description says
 *  "There are three obvious exits: east, west and north"
 *  than "There are two obvious exits: east and west"!
 */

  add_exit("players/anna/examples/room10", "west");
  add_exit("players/anna/examples/room12", "north");
  add_exit("players/anna/examples/room13", "east");

  set_short("Example room 11");
  set_long("This is example room number 11.\n" +
    "It has been created to show you how to use the standard door object.\n" +
    "From this room you can go west to example room number 10,\n" +
    "or east to example room number 13,\n" +
    "but there is also a door leading north to example room number 12.\n");

} /* reset */









