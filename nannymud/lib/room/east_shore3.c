/* Added by Leowon, to straighten up the layout of the land a bit, and
   to allow waterareas to connect, and such. This will also make Tsc's
   area fit in better topolographically. 951217 */

inherit "/std/basic_room";

void reset(int arg) {
  if(arg) return;
  add_property("outdoors");
  add_property("water");
  set_light(1);
  set_short("On a narrow beach");
  set_long("You are walking on a narrow beach. The area of sand is wider "+
	   "to the north, while south of you the ground seems grassy.");
  add_exit("north","/room/east_shore2");
  add_exit("south","/room/east_shore4");
  add_item(({"ground","grass"}),
	   "The narrow beach is thinning away to the south, where green "+
	   "grass is covering the ground.");
  add_item(({"shore","shoreline","coast","waterside"}),
	   "There is a narrow strip of sand just by the waterside.");
  add_item(({"sand","beach"}),
           "The sand is fine and soft.");
  add_item(({"wave","waves","water","sea","ocean","sea of claws"}),
	   "Lazy waves roll in from the Sea of Claws.");
}

