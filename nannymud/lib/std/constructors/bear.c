object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_race("bear");
  o->set_name("bear");
  o->set_alt_name("animal");
  o->set_level(random(6)+2);
  o->set_wc(random(10));
  o->set_al(1);
  o->set_aggressive(1);
  o->set_short("a huge bear");
  o->set_long("It looks like any bear. Hungry.\n");
  o->set_spell_mess1("the bear rips you with it's claws.\n");
  o->set_spell_mess2("");
  o->set_spell_dam(5);
  o->set_spell_chance(10);
  o->set_gender(1+random(2));
  return o;
}
