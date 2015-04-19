object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_name("bumblebee");
  o->set_race("bee");
  o->set_short("a buzzing bumblebee");
  o->set_long("It is a friendly little bumblebee about in its own business.\n");
  o->set_level(1);
  o->set_move_at_reset();
  o->set_gender(2);
  o->set_spell_mess1("The bumblebee buzzes around, madly stinging your face.\n");
  o->set_spell_mess2("The bumblebee makes a wholehearted attempt to sting you.\n");
  o->set_spell_dam(2);
  o->set_spell_chance(10);
  return o;
}
