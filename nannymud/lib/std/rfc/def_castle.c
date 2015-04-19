#define NAME "Mats"
#define DEST "players/mats/test/clean"

inherit "/std/stationary";

reset(arg) {
  if (arg) return;

  set_name("castle");
  add_alias("castle of "+lower_case(NAME));
  set_long("This is the castle of "+NAME+".\n"+
	   NAME + " is a rather new wizard, but it is an amazing castle\n"+
	   "just the same. However, the gates are closed.\n");

  move_object(this_object(),DEST);
  add_property("in_long_short","The castle of "+NAME+" is here");
  add_item_cmd("enter","castle","It is not an open castle.\n");
  /*
  add_property("in_long_short","To the north is "+NAME+" castle");
  DEST->add_exit("north","d2");
  */
  DEST->compute_extra_long();
}
