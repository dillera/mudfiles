/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */
 
inherit "/std/basic_room";
 
reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("plain");

  set_long("You are walking on a large plain. You hear birds singing and the \n"+
	   "humming of insects flying around.");
  set_short("On a large plain");
  add_item(({"bird","birds"}),"You don't see the birds, but you can here them sing.");
  add_item(({"insect","insects"}),"You hear them but you just see some bees.");
  add_item(({"bee","bees"}),"They look as they should. Yellow with black stripes.");
  add_item(({"tree","trees"}),"It looks that a forest begins to the south.");
  add_item("plain","It's a normal plain with grass and flowers growing on it.");
  add_item("grass","The grass is, to your big surprise, green.");
  add_item(({"flower","flowers"}),"They are wonderful.");

  add_item_cmd("kill","bee","You try to kill a bee, but it ends up with it "+
	       "stinging you.\n\b|\bPRON screams in pain as \bpron fails to "+
	       "kill a bee.\n");
  add_item_cmd("pick","flower","You consider picking some flowers, but "+
	       "decides that they look better here.\n\b|\bPRON wonders if "+
	       "\bpron should pick some flowers, but decides not to.\n");
}

extra() {
  /* this empty function is needed otherwise the program is thrown away
     and set_light(1) doesn't work */
  return;
}
