# include <std_mudlib.h>
inherit I_ROOM;

int open;

reset(arg) 
{
  if (!arg)
  {
    set_light(1);
    add_property("outdoors");
    add_property("road");
    add_property("author", "brom");
    add_exit("north", "malkuth_street4");
    add_exit("east", "malkuth_street2");
    set_short("narrow street corner");
    set_long
      ("Malkuth street goes north and east from here. To the west a large heap of\n"+
       "refuse rests against the wall of the house.\n");
    add_item( ({ "refuse", "heap", "pile", "refuse heap", "refuse pile" }),
	     "It contains nothing useful, not even things a goat would eat.\n");
    add_item( ({ "ground", "road" }),
	     "It is not very clean. There's an old iron grating set into the\n"+
	     "ground close to the refuse pile.\n");
  }
  
  if (open) 
  {
    tell_room(this_object(),
	      "The rusty grating falls, closing the hole in the ground.\n");
    remove_exit("down");
  }
  add_item("hole",
	   "It is a hole leading down into the sewers. However, there's a\n"+
	   "grating blocking your access to this wounderful world of darkness,\n"+
	   "rats and unmentionable things.\n");
  add_item( ({ "grating", "iron grating" }),
	   "An old, rusty grating blocking a hole in the ground. Judging from\n"+
	   "the stench, the hole probably leads down onto the sewers. The grating\n" +
	   "is closed, and probably not easy to open.\n");
  open = 0;
} /* reset */

init() 
{
  :: init();
  add_action("cmd_open", "open");
  add_action("cmd_close", "close");
} /* init */

cmd_open(str)
{
  notify_fail("What do you want to open?\n");
  if (!str || sscanf(str, "%*sgrating%*s")!=2) return 0;

  if (open)
  {
    write("But the grating over the hole is already open?\n");
    return 1;
  }

  if (random(this_player() -> query_str()) < 8) 
  {
    write("You try to open the grating, but it does not budge. If you are strong\n"+
	  "enough, perhaps it will give up after several tries?\n");
    say(this_player() -> query_name() +
	"tries to open the grating over the hole in the ground, but fails.\n");
    return 1;
  }
  write("The grating moves.\n");
  say(this_player() -> query_name() +
      "opens the grating over the hole in the ground.\n");
  set_open(); /* Possible to call from sewers, too */
  "room/sewer/sewer1" -> set_open();
  return 1;
} /* cmd_open */

set_open() 
{
  open = 1;
  add_item( ({ "grating", "iron grating" }),
	   "An old, rusty grating blocking a hole in the ground. Judging from\n"+
	   "the stench, the hole probably leads down onto the sewers. The grating\n" +
	   "is open, revealing a passage down.\n");
  add_item("hole",
	   "It is a hole leading down into the sewers.\n");
  add_exit("down", "/room/sewer/sewer1");
} /* set_open */

cmd_close(str) {
  notify_fail("What did you want to close?\n");
  if (!str || 
      (sscanf(str, "%*sgrating%*s") !=2 &&
       sscanf(str, "%*shole%*s") !=2)) return 0;
  if (!open)
  {
    write("The grating over the hole is already closed.\n");
    return 1;
  }

  write("You close the grating over the hole in the ground.\n");
  say(this_player() -> query_name() +
      "closes the grating over the hole in the ground.\n");
  set_closed(); /* Possible to call from sewers, too */
  "room/sewer/sewer1" -> set_closed();
  return 1;
} /* cmd_close */

set_closed()
{
  remove_exit("down");
  add_item( ({ "grating", "iron grating" }),
	   "An old, rusty grating blocking a hole in the ground. Judging from\n"+
	   "the stench, the hole probably leads down onto the sewers. The grating\n" +
	   "is closed, and probably not easy to open.\n");
  add_item("hole",
	   "It is a hole leading down into the sewers. However, there's a\n"+
	   "grating blocking your access to this wounderful world of darkness,\n"+
	   "rats and unmentionable things.\n");
  open = 0;
} /* set_closed */
