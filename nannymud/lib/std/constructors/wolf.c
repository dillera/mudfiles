object make()
{
  object o;
  o = clone_object("obj/monster");
  o->set_name("wolf");
  o->set_race("wolf");
  o->set_level(3);
  o->set_short("A wolf");
  o->set_long("A grey wolf running around. It has some big dangerous teeth.\n");
  o->set_wc(7);
  o->set_move_at_reset();
  o->set_whimpy();
  o->set_gender(1+random(2));
  return o;
}
