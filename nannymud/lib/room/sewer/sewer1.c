#include <std_mudlib.h>
inherit I_ROOM;

status open;

reset(arg)
{
  if (!arg)
  {
    set_light(1);
    add_property("indoors");
    add_property("author", "brom");
    add_exit("east", "sewer2");
    add_exit("south", "sewer5");
    set_short("In the sewers");
    set_long
      ("You stand in the sewers under Lars Town. The water reaches halfawys up\n"+
       "your calves. Light filters in from above you, while to the east and\n"+
       "south all is dark.\n");
    add_item("water",
	     "Well, perhaps this was water once... it stinks.\n");
    add_item("roof",
	     "There is a hole in the roof from where light falls in through a grating.\n"+
	     "Rusty rungs climbs the north wall up to the light.\n");
    add_item( ({ "rung", "rungs", "rusty rung", "rusty rungs" }),
	     "You might climb up the rungs. If you're lucky, they will hold all the\n"+
	     "way up instead of ditching you here.\n");
    add_item( ({ "wall", "walls" }),
	     "Crumbling mortar and murky bricks; the water is not good for walls.\n");
  }

  if (open) open = 0;
  add_item("hole",
	   "It is a hole leading up from the sewers. However, there's a\n"+
	   "grating blocking your access to the world of sanity and light.\n");
  add_item("grating",
	   "A sturdy, rusty grating through which the light falls. It is closed.\n");
} /* reset */

/* make a rat and return the object.
   * Used by the sewers as needed.
   */
make_rat() 
{
  object r;

  r = clone_object("obj/smartmonster");
  r -> set_name("rat");
  r -> set_short("a rat");
  r -> set_long
    ("A dirty sewer rat. It is black, and it glares at you with baleful red\n"+
     "eyes. It twitches its whiskers and bares its teeth.\n");
  r -> add_alias("monotreme");
  r -> set_level(2);
  r -> set_aggressive(2);
  r -> set_chat_chance(10);
  r -> add_chat("The rat looks like a nasty little bugger.\n");
  r -> add_chat("The rat looks menacingly at you!\n");
  r -> add_chat("The rat looks like it has a hundred diseases.\n");
  r -> add_attack("The rat bites $OTHER!\n", "The rat bites you!\n", 5, 3);
  return r;
}

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

  if (random(this_player() -> query_str()) < 12) 
  {
    write("You try to open the grating, but it does not budge. If you are strong\n"+
	  "enough, perhaps it will give up after several tries?\n");
    say(this_player() -> query_name() +
	"tries to open the grating over the hole, but fails.\n");
    return 1;
  }
  write("The grating moves.\n");
  say(this_player() -> query_name() +
      "opens the grating blocking passage up.\n");
  set_open(); /* Possible to call from sewers, too */
  "room/malkuth/malkuth_street3" -> set_open();
  return 1;
} /* cmd_open */

set_open() 
{
  open = 1;
  add_item( ({ "grating", "iron grating" }),
	   "A sturdy, rusty grating through which the light falls. It is open.\n");
  add_item("hole",
	   "It is a hole leading up from the sewers.\n");
  add_exit("up", "/room/malkuth/malkuth_street3");
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

  write("You close the grating over the hole, blocking the passage.\n");
  say(this_player() -> query_name() +
      "closes the grating over the hole, blocking the passage.\n");
  set_closed(); /* Possible to call from sewers, too */
  "room/malkuth/malkuth_street3" -> set_closed();
  return 1;
} /* cmd_close */

set_closed()
{
  remove_exit("down");
  add_item( ({ "grating", "iron grating" }),
	   "A sturdy, rusty grating through which the light falls. It is closed.\n");
  add_item("hole",
	   "It is a hole leading up from the sewers. However, there's a\n"+
	   "grating blocking your access to the world of sanity and light.\n");
  open = 0;
} /* set_closed */
  
