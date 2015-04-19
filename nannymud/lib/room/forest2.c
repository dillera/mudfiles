/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/forest_def";

object troll;

reset(arg) {
  if(arg) return; ::reset(arg);
  fix_troll();
  add_exit("room/clearing", "east");
  add_exit("room/slope", "west");
  set_long("You are in a big forest. You can hardly see the sky through the trees.\n"+
	   "Now and then you hear birds singing and animals walking. To the west \n"+
	   "the forest get lighter and to the east you see a clearing.\n");
}

fix_troll() {
  object money;
  if (!troll || !living(troll)) {
    troll = clone_object("obj/monster");
    troll->set_name("troll");
    troll->set_level(9);
    troll->set_hp(100);
    troll->set_wc(12);
    troll->set_al(-60);
    troll->set_short("A troll");
    troll->set_long("It is a nasty troll that look very aggressive.\n");
    troll->set_aggressive(1);
    troll->set_spell_mess1("The troll mumbles something.");
    troll->set_spell_mess2("The troll says: Your mother was a bitch!");
    troll->set_chance(20);
    move_object(troll, this_object());
    money = clone_object("obj/money");
    money->set_money(random(500));
    move_object(money, troll);
  }
}

