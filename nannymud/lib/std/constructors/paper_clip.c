object make()
{
  object o;
  o=clone_object("/std/basic_thing");
  o->set_name("paper clip");
  o->add_alias("clip");
  o->set_value(1);
  o->set_long("It's a small metal paper clip.\n");
  
  return o;
}
