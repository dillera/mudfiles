object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_name("vulture"); 
  o->set_alias("bird");
  o->set_short("a vulture");
  o->set_long("The vulture fly around searching for rotting corpses of\n"+
	      "careless adventurers. You think you can see an evil grin\n"+
	      "at the vultures face as it looks at you, but how could a\n"+
	      "vulture grin?\n");
  o->set_race("bird");
  o->set_level(12);
  o->set_al(-100);
  o->set_hp(o->query_hp()+random(30));
  o->set_chance(8);
  o->set_spell_dam(15);
  o->set_spell_mess1("The vulture makes some horrible noises.");
  o->set_spell_mess2("With a horrible scream the vulture picks your head.");
  return o;
}
