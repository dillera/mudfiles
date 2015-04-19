#include <std_mudlib.h>
inherit I_ROOM;

status search_done;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("waterfilled");
    add_property("author", "brom");
    add_exit("up", "sewer3");
    set_short("In the sewers");
    set_long
      ("Things fall to the bottom, or what counts as bottom, here; the liquid is\n"+
       "soft to the touch and brown. Who knows what could be found here by the\n"+
       "bold explorer?\n");
    add_item("liquid",
	     "It is surely based on water; what else it contains is better\n"+
	     "left undescribed.\n");
    add_item("bottom",
	     "It is soft and yielding. When you touch it, small silvery bubbles\n"+
	     "rise towards the surface.\n");
  }
  search_done = 0;
} /* reset */

init() {
  :: init();
  add_action("cmd_search", "search");
  add_action("cmd_search", "dig");
} /* init */

cmd_search(str) {
  
  notify_fail("Search what?\n");
  if (!str || sscanf(str, "%*sbottom%*s")!=2) return 0;

  write("You search the bottom.\n");
  say(this_player() -> query_name() +
      " searches the bottom.\n");

  if (search_done)
  {
    write("You find nothing.\n");
    return 1;
  }

  if (random(2)) search_done = 1;

  write("You find some lost gold coins.\n");
  this_player() -> add_money(5+random(8));
  return 1;
} /* cmd_search */
