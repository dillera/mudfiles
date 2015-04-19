object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_name("ant");
  o->set_race("ant");
  o->set_short("a little worker ant");
  o->set_long("It is a friendly little worker ant, carrying a very heavy load.\n");
  o->set_level(1);
  o->set_move_at_reset();
  o->set_gender(2);
  o->set_spell_mess1("The little ant squirts a meagre amount of acid on you.\n");
  o->set_spell_mess2("The little ant tries to tickle your leg frenetically.\n");
  o->set_spell_dam(1);
  o->set_spell_chance(5);
  return o;
}
