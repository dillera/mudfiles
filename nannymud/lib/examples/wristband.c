inherit "/std/simple_armour";

reset(arg) {
  if (arg) return;
  set_type("glove");
  set_class(0);
  set_weight(1);
  set_value(200);
  set_name("wristband");
  set_short("A wristband");
  set_long("A wristband with four small rat's skulls hanging from it.\n");
  add_item(({"skulls","rat skulls"}),"It looks like real rat skulls, just a little smaller.\n");
  add_property("__remote",file_name(this_object()));
} /* reset */

remote_do_wear() {
  add_property("cursed");
  write("The wristband tightens around your wrist so much that it hurts.\n");
  this_player()->reduce_hit_point(20);
  return;
} /* remote_do_wear */

