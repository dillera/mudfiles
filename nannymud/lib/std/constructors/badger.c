object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_race("badger");
  o->set_name("badger");
  o->set_level(2);
  o->set_short("a badger");
  o->set_long("A shy badger, almost as afraid as you.\n");
  o->set_move_at_reset();
  o->set_gender(1+random(2));
  return o;
}
