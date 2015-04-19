object make()
{
  object o;

  o=clone_object("/obj/lockable");
  o->set_max_weight(4);
  o->set_name("small box");
  o->set_short("a small box");
  o->set_type("generic");
  o->insert_matching_key();
  o->set_long("A small box.\nThe lock doesn't look all that "
	      + "secure, though.\n");
  o->set_alias("box");
  o->set_weight(1);
  o->set_max_weight(1);
  o->set_value(50);
  return o;
}
