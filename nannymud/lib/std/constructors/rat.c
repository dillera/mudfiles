object make()
{
  object o;

  o=clone_object("obj/monster");
  o->set_name("rat");
  o->set_race("rat");
  o->set_alias("black rat");
  o->set_level(3);
  o->set_short("an ugly black rat");
  o->set_long("An ugly, black, rat.\n");
  o->set_wc(5);
  o->set_agressive(1);
  return o;
}
