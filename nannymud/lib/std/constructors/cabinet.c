object make()
{
  object o;

  o=clone_object("/obj/lockable");
  o->set_type(0);		/* No special key needed */
  o->set_max_weight(4);
  o->set_name("cabinet");
  o->set_short("a large cabinet");
  o->set_long("A large cabinet.");
  o->set_alias("large cabinet");
  o->set_weight(20);
  o->set_max_weight(18);
  o->set_value(1000);
  return o;
}
