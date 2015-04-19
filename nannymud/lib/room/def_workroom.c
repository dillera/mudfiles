/*  This is the default workroom.
 *  It has been created so the new wizard will have a room of his (her, its)
 *  own even if he (she, it) doesn't know much about coding yet,
 *  but also to demonstrate the use of NannyMUD's standard room object.
 *  A line #define NAME "wizardsname" will be (or has been) prepended.
 *	This is one way of creating a room object. Look at other
 *	rooms in the mud, and at the standard room object ("/std/room.c")
 *	for other ways.
 *  Latest change to this file: September 20, 1995, by Rohan
 */
inherit "/std/room";

reset(arg) {
  if (arg) return;

  add_property("no_teleport"); /* Don't let mortals teleport here */
  add_property("indoors");
  set_light(1);

  add_exit("church","/room/church");
  add_exit("guild","/room/adv_guild");
  add_exit("wizroom","/room/adv_inner");
  add_exit("post","/room/post");
  add_exit("examples","/examples/room1");

  set_short(capitalize(NAME) + "'s workroom");
  set_long(
    "You are standing inside " + capitalize(NAME) + "'s workroom.\n" +
    "So far, it's just an empty standard workroom, of the kind\n" +
    "given to new wizards on NannyMUD: bare stone walls, a rough wodden floor,\n" +
    "and some dust and spiderwebs.\n" +
    "There are some magic exits, though: to the Village Church,\n" +
    "to the Adventurers' Guild, to the Wizards' Room, to the Post Office,\n" +
    "and to a suite of example rooms that can be used for learning LPC.\n");

  add_item(({ "walls", "stone walls", "wall", "stone wall" }),
    "The walls are made of stone and are bare.");
  add_item(({ "floor", "wooden floor" }),
    "The floor is made of rough wooden planks.");
  add_item("dust",
    "There is some dust in the corners.");
  add_item(({ "spiderweb", "spider web", "spiderwebs", "spider webs", "web", "webs", "ceiling" }),
    "There are some spiderwebs in the ceiling.");
  add_item(({ "exits", "magic exits", "exit", "magic exit", "doors", "door", "magic doors", "magic door" }),
    "The exits look like ordinary doors, but they are magical one-way exits.");
} /* reset */

