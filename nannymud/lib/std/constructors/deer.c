object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_name("deer");
  o->set_race("deer");
  o->set_alt_name("animal");
  o->set_level(random(4));
  o->set_wc(random(4));
  o->set_al(0);
  o->set_aggressive(0);
  o->set_short("a small deer");
  o->set_long("It looks like any deer.\n");
  o->set_spell_mess1("the deer looks at you with big, brown, inocent eyes.\n");
  o->set_spell_mess2("");
  o->set_spell_dam(1);
  o->set_spell_chance(10);
  o->set_gender(1+random(2));
  return o;
}
