object make()
{
  object o;
  mixed arg;

  o = clone_object("obj/monster");
  o->set_name("troll");
  o->set_race("troll");
  o->set_level(9);
  o->set_hp(100);
  o->set_wc(12);
  o->set_al(-60);
  o->set_short("a troll");
  o->set_long("It is a nasty troll that look very aggressive.\n");
  o->set_aggressive(1);
  o->set_spell_mess1("The troll mumbles something.");
  o->set_spell_mess2("The troll says: Your mother was a bitch!");
  o->set_chance(20);
  o->add_money(random(500));
  return o;
}
