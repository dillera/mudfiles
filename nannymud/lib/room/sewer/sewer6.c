#include <std_mudlib.h>
inherit I_ROOM;

status done;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("author", "brom");
    add_exit("east", "sewer7");
    add_exit("down", "/room/sea_bottom");
    set_short("In the sewers");
    set_long
      ("The water here is up to your waist and streams quite violently towards a\n"+
       "hole in the floor. Along the north wall is a dais with a throne on it.\n");
    add_item("floor", "It is covered with streamin water.\n");
    add_item("roof", "Old, murky bricks.\n");
    add_item("wall",
	     "There are walls to the east, south, west and north.Which one did you\n"+
	     "want to have a closer look at?\n");
    add_item("north wall",
	     "Along the north wall is a dais with a throne on.\n");
    add_item( ({ "east wall", "west wall", "south wall", "walls" }),
	     "The walls are made from stone and bricks. The bricks are old and worn.\n");
    add_item("water",
	     "The water flow freely towards a hole in the floor. The water is far\n"+
	     "from fresh; various substances are transported along with it.\n");
    add_item( ({ "substance", "substances" }),
	     "In short: shit.\n");
    add_item("hole",
	     "It is about 2 yards across; big enough to allow you to go down it.\n");
    add_item("dais",
	     "It clears the sewer water with about a foot. On the dais is a throne.\n");
    add_item("throne",
	     "It is a crude stone throne. It is more-than-human in size, but you\n"+
	     "could surely sit in it, like a baby sitting in a grown-up's chair.\n"+
	     "But do you dare?\n");
  }
  done = 0;
} /* reset */

init() {
  :: init();
  add_action("cmd_sit", "sit");
} /* init */

cmd_sit(str) 
{
  
  notify_fail("But what do you wish to sit in?\n");
  if (!str || sscanf(str, "%*sthrone%*s")!=2) return 0;

  write("You sit on the throne. It is big. You have an uncanny feeling.\n");
  say(this_player() -> query_name() +
      " sits on the throne, looking out of place.\n");

  if (done || random(5)) return 1;

  tell_room(this_object(),
	    "The water troll rises like a behemoth out of the deep!\n"+
	    "The water troll bellows!\n");
  make_neptic();
  return 1;
}

make_neptic() {
  object n, o;

  n = clone_object("obj/smartmonster");
  n -> set_name("neptic");
  n -> add_alias("troll");
  n -> add_alias("water troll");
  n -> set_short("neptic the water troll");
  n -> set_long
    ("Neptic is a water troll of great size. He imitates Neptune, but thinks it\n"+
     "enough to rule over the local sewers. He is ugly, even as trolls go.\n");
  n -> set_level(13);
  n -> set_hp(379);
  n -> set_ac(6);
  n -> set_al(-47);
  n -> set_aggressivity(20);

  o = clone_object(I_WEAPON);
  o -> set_name("trident");
  o -> set_short("a rusty trident");
  o -> set_long
    ("A rusty old trident. It smells of the sewers.\n");
  o -> set_type("pierce");
  o -> set_class(9);
  o -> set_weight(2);
  o -> set_value(43);
  move_object(o, n);
  o = clone_object(I_ARMOUR);
  o -> set_name("crown");
  o -> set_short("a rusty iron crown");
  o -> set_long
    ("A rusty old crown. It smells bad. Real bad.\n");
  o -> set_type("helmet");
  o -> set_class(1);
  o -> set_weight(3);
  o -> set_value(38);
  move_object(o, n);
  move_object(n, this_object());
} /* make_neptic */
