object make()
{
  object o;
  o=clone_object("/std/simple_weapon");
  o->set_name("awl-pike");
  o->add_alias("pike");
  o->add_alias("awl");
  o->set_short("an awl-pike");
  o->set_long("A heavy mace-like weapon with spikes on the head.");
  o->set_value(55);
  o->set_weight(1);
  o->set_type("pierce");
  o->set_wc(9);
  o->add_property("wood");
  return o;
}
