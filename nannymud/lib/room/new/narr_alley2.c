/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "/room/room";

void reset(int arg) {
  if (arg) return;
  add_exit("/room/narr_alley","north");
  add_exit("/room/turistinfo","east");
  add_exit("/room/post","west");
  add_exit("/room/kontor","south");
  set_short("A narrow alley");
  set_long("A narrow alley that continues north. West lies the post office.\n"+
	   "East is the tourist information office. To the south lies\n" +
           "the Bailiffs office.\n");
  add_item("post office","In the post office you can read and send mail \n"+
	                 "to other players in the mud");
  add_item(({"tourist office", "information office", "tourist information",
	     "tourist information office"}),
	     "In the tourist information office you can find out about the\n"+
	     "various interesting areas in this mud");
  add_item("office","Which office do you mean, the post office, the tourist\n"+
	            "information office or the Bailiffs office");
  add_item("bailiffs office",
	   "The Bailiffs office is a building made entirely by millions\n" +
	   "of gold-coins");
  set_no_castle_flag(1);
  set_light(1);
  set_outdoors();
}

void init() {
  ::init();
  if(this_player()->query_npc()) add_action("npcblock","west");
}

int npcblock() {return 1;}
