object make()
{
  object o;

  o=clone_object("/std/basic_thing");
  o->set_name("rock");
  o->set_short("a rock");
  o->set_weight(3);
  o->set_value(1);
  o->set_property("stone");
  return o;
}
