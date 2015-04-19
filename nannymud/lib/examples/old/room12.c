inherit "room/room";


reset(arg) {

  set_light(1);
  set_outdoors();
  add_exit("players/anna/examples/room11", "south");

  set_short("Example room 12");
  set_long("This is example room number 12.\n" +
    "You can come here through the door from example room number 11.\n");

/*  What? Why on earth do we do this call to "short" in room11 here?
 *  Answer: If someone got into this room, either by teleporting or
 *  by going the back way (if there had been one), and room11 hadn't
 *  been loaded yet, then there wouldn't be a door here, since the
 *  door is created, and put here, by room11!
 *  Therefore we call "short" in room11, just to be sure that that
 *  room is loaded, and that the door exists!
 * 
 *  And why do we do this as the last thing in reset?
 *  Answer: If there is an error in the other room (room12)
 *  and we try to load it first we will fail and not get
 *  any light, and exits, any short and any long in this room.
 */

  call_other("players/anna/examples/room11", "short");

} /* reset */


