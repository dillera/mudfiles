object make()
{
  object o;

  o=clone_object("/std/basic_thing");
  o->set_name("pebble");
  o->set_short("a pebble");
  o->set_weight(1);
  o->set_value(1);
  o->set_property("stone");
  
  return o;
}
