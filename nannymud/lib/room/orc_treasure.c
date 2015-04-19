/* Rewritten using add and set functions in room.c. Banshee 950218 */
/* Rewritten using /std/room.c, Mats 960227 */
/* Fixed by Brom 960323 */

inherit "/std/room";

reset(arg) {

  :: reset(arg);
  if (arg) return;

  set_light(1);
  add_property("cave");

  set_short("The orc treasury");
  set_long("You are in the orc treasury. It is normally heavily guarded.\n");
  add_item("treasury","It's just a heavaily guarded cave, at least it's \n"+
	              "usually heavily guarded.");
  add_item("treasure","The treasures seems to be somewhere else, maybe\n"+
	              "that's why it's not as heavily guarded as usual.");
  add_exit("south","fortress");

  add_object("@make_shaman()");
}

weapon_hit(attacker) {
  if(attacker->id("orc")) return write("Ziiing\n"),10;
}

make_shaman() {
  object gold_stick, orc_slayer, shaman;

  gold_stick = clone_object("obj/treasure");
  gold_stick->set_id("staff");
  gold_stick->set_alias("golden staff");
  gold_stick->set_short("a golden staff");
  gold_stick->set_value(300);
  orc_slayer = clone_object("obj/weapon");
  orc_slayer->set_name("short sword");
  orc_slayer->set_alias("sword");
  orc_slayer->set_short("short sword");
  orc_slayer->set_alt_name("orc slayer");
  orc_slayer->add_property("special_item");
  orc_slayer->set_long("This was once a very fine blade, but after being\n"+
		       "in the hands of orcs for a very long time, it now\n"+
		       "has lost its shine. Among the ancient runes on the\n"+
		       "blade you can also see some rusty spots and the \n"+
		       "engraved picture of the sword slicing an orc is \n"+
		       "not as visible  as it used to be.\n");
  orc_slayer->set_read("The only thing you can read is the word 'orc'.\n");
  orc_slayer->set_class(9);
  orc_slayer->set_weight(2);
  orc_slayer->set_value(200);
  orc_slayer->set_hit_func(this_object());
  shaman = clone_object("obj/smartmonster");
  shaman->set_name("shaman");
  shaman->set_alias("orc");
  shaman->set_race("orc");
  shaman->set_level(10);
  shaman->set_al(-300);
  shaman->set_short("an orc shaman");
  shaman->set_wc(10);
  shaman->set_ac(5);
  shaman->set_aggressive(1);
  shaman->add_attack("The shaman casts a magic missile.\n",
		     "You are hit by a magic missile.\n",20,20);
  move_object(gold_stick, shaman);
  move_object(orc_slayer, shaman);
  return shaman;
}

