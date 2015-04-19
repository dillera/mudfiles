inherit "std/room";
reset(arg) {
  if (arg) return;
  add_property("outdoors");
  add_property("water");
  set_light(1);
  add_exit("north","river2");
  add_exit("south","south_coast");
  set_short("Along the west side of a river");
  set_long(          
    "The river to your east is rather wide and full of life here.\n"+
    "It looks like the river gets more calm up north.\n");
  add_item(({"river","water"}), 
	   "This is the west bank of a river that flows north-south");
} /* reset */


