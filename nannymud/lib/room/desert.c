inherit "std/basic_room";

reset(arg) {
  if (arg) return;
  set_light(1);
  add_exit("east","/room/plane1");
  add_exit("west","/room/desert2");
  set_short("In the desert");
  set_long("You are walking in the desert, surrounded by sand.\n");
  add_property("outdoors");
  add_property("desert");
  add_item("sand","Soft sand, shining almost like gold");
} /* reset */


