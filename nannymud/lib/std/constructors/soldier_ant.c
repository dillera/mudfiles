object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_name("ant");
  o->set_race("ant");
  o->set_short("a fierce little soldier ant");
  o->set_long("It is a fierce little soldier ant, but it won't do you any harm\n"+
	      "as long as you leave it alone.\n");
  o->set_level(1);
  o->set_gender(2);
  o->set_spell_mess1("The fierce little ant squirts out a small amount of burning acid.\n");
  o->set_spell_mess2("The fierce little ant bites at your skin and squirts acid on it.\n");
  o->set_spell_dam(2);
  o->set_spell_chance(10);
  return o;
}
