inherit "/std/simple_armour";

reset(arg) {
  if (arg) return;
  set_type("ring");
  set_class(2);
  set_weight(1);
  set_value(190);
  set_name("ring");
  set_short("A small diamond ring");
  set_long("It is a small golden ring with a diamond.");
  add_item(({"diamond","jewel"}),
	   "The diamond is sparkling delightfully in the light.");
  add_property("gold");
  add_property("__remote",file_name(this_object()));
} /* reset */

remote_do_wear() {
  write("You feel very proud of your beautiful ring.\n");
} /* remote_do_wear */

remote_do_remove() {
  write("You already miss your beautiful ring.\n");
} /* remote_do_remove */

remote_prevent_wear() {
  if(this_player()->query_con() > 10) {
    write("The ring is too small to fit you.\n");
    return 1;
  }
  return 0;
} /* remote_prevent_wear */
