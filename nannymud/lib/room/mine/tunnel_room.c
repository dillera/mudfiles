/* Rewritten using /std/room.c, Mats 960222 */

inherit "/std/room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  ::reset(arg);
  if(arg) return;

  set_light(0);
  add_property("cave");

  set_short("Deep in the Mines");
  set_long(LB("Deep in the Mines\n"+
	"You are standing in a small room deep under the mountains.  "+
	"It appears that the miners did not dig much deeper than this, "+
	"for the walls grow rough-cut and the man-made passage grows "+
	"to an end, while to the north a natural cavern opens up.  The "+
	"air is moist and water drips from the ceiling."));
  add_exit("south","tunnel5");
  add_exit("north","cavern_south");

  add_item("water", LB("Stalactites hang from the ceiling, formed as "+
	"water over the centuries has dropped down, bringing "+
	"calcium deposites with it."));
  add_item("cavern", "A large cavern lies to the north.\n");

  add_object("@make_hobgoblin()");
}

make_hobgoblin()
{
  object hob;
  object thing;

  hob = clone_object("/obj/monster");
  hob->set_name("hobgoblin");
  hob->set_alias("goblin");
  hob->set_short("a hobgoblin");
  hob->set_long("He looks mean and quite nasty!\n");
  hob->set_level(5);
  hob->set_wc(9);
  hob->set_race("goblin");
  hob->set_ac(4);

  thing = clone_object("/obj/weapon");
  thing->set_name("miner's pick");
  thing->set_alias("pick");
  thing->set_alt_name("miners pick");
  thing->set_short("a miner's pick");
  thing->set_long("This is a pick that a miner might use.\n");
  thing->set_class(9);
  thing->set_weight(3);
  thing->set_value(35);
  move_object(thing, hob);

  thing = clone_object("/std/basic_thing");
  thing->set_name("quartz");
  thing->set_short("some quartz");
  thing->set_long("Nice and shiny, if roughly cut;  however, it is "+
	"probably worthless.\n");
  thing->set_value(15);
  thing->set_weight(2);
  move_object(thing, hob);
  tell_room(this_object(), "A hobgoblin wanders into the room and "+
	"glares about angrily!\n");

  return hob;
}
