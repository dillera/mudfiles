/* The forest */

inherit "/std/room";
reset(arg) {
  ::reset(arg);
  if (arg) return;

  add_property("outdoors");
  add_property("forest");   /* This room has the terrain type forest. */
  add_property("road");     /* There is a road going in this room. */
  set_light(1);
  add_exit("south","room7");

  set_short("Forest");
  set_long(
    "You are on a small path in an old forest. The trees are mostly oak trees.\n" +
    "There are other sorts too but they are not as common.\n"+
    "In this room you can see the use of a terrain property.\n"+
    "Terrain properties are only used outdoors, and they are:\n"+
"Forest, Plain, Swamp, Desert, Water, Urban, Mountain, Hills\n");

  add_item(({"road","ground"}),
    "You are walking on a small path.\n");
  add_item("path",
    "The path looks as old as the forest.\n");
  add_item("forest",
    "The forest is quite old.\n");
  add_item(({"tree","trees"}),
    "The trees are mostly oaktrees.\n");
  add_item(({"oak","oaks","oak tree","oak trees"}),
    "The oaks look very old.\n");
} /* reset */




