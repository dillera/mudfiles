/* Added by Leowon, to straighten up the layout of the land a bit, and
   to allow waterareas to connect, and such. This will also make Tsc's
   area fit in better topolographically. 951219 */

inherit "/std/basic_room";

void reset(int arg) {
  if(arg) return;
  add_property("outdoors");
  add_property("water");
  set_light(1);
  set_short("Shore of the Sea of Claws");
  set_long("Your gaze is drawn out over the vast Sea of Claws to your east. "+
	   "The shoreline here is even, and waves caress the land gently.");
  add_exit("north","/room/jetty");
  add_exit("south","/room/east_shore2");
  add_item(({"shore","shoreline","coast"}),
	   "The shore runs north-south, and you see a beach to the south.");
  add_item(({"ground","earth","land"}),
	   "Waves slowly lap at the shore, water trying to conquer earth "+
	   "with neverending patience.");
  add_item("beach",
	   "It is south of here.");
  add_item("jetty",
	   "The jetty is north of here.");
  add_item(({"water","sea","ocean","wave","waves","sea of claws"}),
	   "Waves come rolling in from the Sea of Claws.");
}

