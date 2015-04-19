#include <std_mudlib.h>
inherit I_ROOM;

string chats;

reset(arg) 
{
  if (!arg)
  {
    set_light(1);
    add_property("outdoors");
    add_property("authour", "brom");
    add_exit("north", "pine_wood4");
    add_exit("east", "pine_wood2");
    set_short("Squirrel valley");
    set_long
      ("Here in the squirrel valley, the trees are less tall. The ground is covered\n"+
       "in pine needles.\n");
    add_item( ({ "tree", "trees", "pine", "pines", "pine tree", "pine trees" }),
	     "Full grown pine trees grows here. They are the home of many birds and small\n"+
	     "animals.\n");
    add_item("ground", "It is covered in pine needles.\n");
    add_item( ({ "bird", "birds" }),
	     "They flitter around in almost silence on their hunt for things to eat.\n");
    add_item( ({ "animal", "animals", "small animal", "small animals" }),
	     "Small furry animals scurrying around in the wood.\n");
  }
  mk_squirrel();
} /* reset */

mk_squirrel() {
  object o;

  if (!present("squirrel"))
  {
    o = clone_object("obj/smartmonster");
    o -> set_name("squirrel");
    o -> set_short("A squirrel");
    o -> set_long
      ("A cute little squirrel. You'd love to pet this one.\n");
    o -> set_level(1);
    o -> set_chat_chance(10);
    o -> add_chat("The squirrel climbs around.\n");
    o -> add_chat("The squirrel scratches the ground, looking for something.\n");
    o -> add_chat("The squirrel gnaws at a pine cone.\n");
    o -> add_chat("The squirrel claws at a pine cone.\n");
    o -> add_chat("The squirrel trims its tidy tail.\n");
    o -> add_chat("The squirrel bushes up its tail.\n");
    o -> add_chat("The squirrel seems to listen for something.\n");
    move_object(o, this_object());
  }
} /* mk_squirrel */

