object make()
{
  object o;
  o=clone_object("/std/basic_thing");
  o->set_name("ruby");
  o->set_short("a ruby");
  o->set_long("It glimmers prettily.\n");
  o->set_value(100);
  o->set_weight(1);

  return o;
}
