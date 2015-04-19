/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;

  set_long("You are walking on a large plain. You hear birds singing and the \n"+
	   "humming of insects flying around. The plain continues to the \n"+
	   "south and to the east. There is a forest to the west.\n");
  add_item("forest","A forest is a bunch of trees, living together. \n"+
	            "If you are interested in how it looks go to the west");
  add_item(({"tree","trees"}),"The things that grow in a forest..");

  add_exit("west", "deep_forest1");
  add_exit("east", "plane11");
  add_exit("south","plane10");
}
