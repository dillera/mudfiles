object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_name("dog");
  o->set_race("dog");
  o->set_short("a dog");
  o->set_level(7);
  o->set_gender(1+random(2));
  return o;
}
