object make()
{
  object o;

  o = clone_object("obj/monster");
  o->set_name("hobgoblin");
  o->set_race("hobgoblin");
  o->set_level(5);
  o->set_wc(9);
  o->set_short("a hobgoblin");
  o->set_long("This hobgoblin looks really nasty.\n");
  o->add_money(10+random(50));
  return o;
}
