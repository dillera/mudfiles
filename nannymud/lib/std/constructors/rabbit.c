object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_name("rabbit");
  o->set_race("rabbit");
  o->set_short("a rabbit");
  o->set_level(2);
  o->set_gender(1+random(2));
  return o;
}
