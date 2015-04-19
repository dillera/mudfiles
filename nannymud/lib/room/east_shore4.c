/* Added by Leowon, to straighten up the layout of the land a bit, and
   to allow waterareas to connect, and such. This will also make Tsc's
   area fit in better topolographically. 951217 */

inherit "/std/basic_room";

void reset(int arg) {
  if(arg) return;
  add_property("outdoors");
  add_property("water");
  set_light(1);
  set_short("A grassy shoreline");
  set_long("You are strolling by the waterside of the Sea of Claws. "+
	   "The air smells pleasantly here.");
  add_exit("north","/room/east_shore3");
  add_item(({"air","smell","breeze"}),
	   "A pleasant smell, which you cannot quite identify, is carried "+
	   "to you by a passing breeze.");
  add_item(({"shore","shoreline","coast","waterside"}),
	   "The ground is grassy here, all the way to the waterside, and "+
	   "you think you see the dark shade of trees to the south.");
  add_item(({"ground","grass"}),
           "Green, healthy-looking grass covers the ground here.");
  add_item(({"wave","waves","water","sea","ocean"}),
	   "Far away from the shore, you can see waves breaking. The coast "+
	   "seems to curve eastward farther south, away from you.");
}

