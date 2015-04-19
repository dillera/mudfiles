/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* Rewritten using /std/basic_room.c, Mats 960225 */

inherit "room/plane_def";

reset(arg) {
  ::reset(arg);
  if(arg) return;
  set_long("You are standing on a wide plains. To the north the grass is\n"+
	   "greener and healthier and it sways in the wind. To the west \n"+
	   "the plain gets drier and drier, ending in the sandy desert.\n");
  add_item("grass","Rather short and dry here, but greener to the north");
  add_item("desert","It seems very dry and very hot. If you are planning\n"+
	   "to go there, you should bring something to drink");
  add_exit("south","clearing");
  add_exit("north","plane2");
  add_exit("west", "desert");
}
