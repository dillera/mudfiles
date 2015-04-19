/*  /room/vill_road2.c
 *     -- rewritten by me (Padrone) to use my /obj/smartmonster
 *     This Harry doesn't act exactly like the standard Harry,
 *     but almost. He also has some extra features.
 *  Latest change of this file: August 3, 1992
 */

/* Rewritten using add and set functions in room.c. Banshee 950217 */

inherit "room/room";

object harry;
int count;

reset(arg) {
  if (!arg) {
    set_light(1);
    set_short("Village road");
    set_long("A long road going through the village. The road continues \n"+
	     "to the west and ends at a shore to the east. To the north \n"+
	     "is the shop, and to the south is the adventurers' guild.\n"+
	     "There are stairs going down.\n");
    add_item("stairs", "The stairs leads down into the underground");
    add_item("shop","This is the main shop, it's where most people buy and\n"+
	            "sell things that they find on their glorious adventures");
    add_item(({"guild","adventurers' guild"}),
	     "The guildhouse is somewhat bigger than the other buildings\n"+
	     "and it has a certain flare of dignity. People usually go \n"+
	     "there to enhance their characteristics");
    add_item("road","It's the main village road");
    add_exit("room/vill_road1","west");
    add_exit("room/vill_shore","east");
    add_exit("room/shop","north");
    add_exit("room/adv_guild","south");
    add_exit("room/station","down"); 
    set_outdoors(); }
  start_harry();
} /* reset */

start_harry() {
  if(!harry) {
    harry = clone_object("obj/harry");
    move_object(harry,this_object()); }
}
