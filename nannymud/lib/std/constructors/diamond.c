object make()
{
  object o;
  o=clone_object("/std/basic_thing");
  o->set_name("diamond");
  o->set_short("a diamond");
  o->set_long("It glimmers prettily.\n");
  o->set_value(300);
  o->set_weight(1);

  return o;
}
