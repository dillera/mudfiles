object make()
{
  object o;

  o=clone_object("/std/basic_thing");
  o->set_name("boulder");
  o->set_short("a boulder");
  o->set_weight(20+random(13));
  o->set_value(1);
  o->set_property("stone");
  return o;
}
