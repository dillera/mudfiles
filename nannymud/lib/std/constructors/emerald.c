object make()
{
  object o;
  o=clone_object("/std/basic_thing");
  o->set_name("emerald");
  o->set_short("an emerald");
  o->set_long("It glimmers prettily.\n");
  o->set_value(70);
  o->set_weight(1);

  return o;
}
