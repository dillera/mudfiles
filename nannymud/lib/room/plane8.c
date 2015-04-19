/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;

  set_long("You are standing on a large plain. You hear birds singing and\n"+
	   "the humming of insects flying around. The plain continues to the\n"+
	   "west and to the north. South you see a ruin and to the east there\n"+
	   "is a ford.\n");
  add_item("ruin","You have to get closer to check it out");
  add_item("ford","It looks normal");

  add_exit("north","plane13");
  add_exit("west", "plane6");
  add_exit("south","ruin");
  add_exit("east", "ford");
}
