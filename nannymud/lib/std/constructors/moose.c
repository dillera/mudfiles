object make()
{
  object o;

  o = clone_object("obj/monster");
  o->set_name("moose");
  o->set_race("moose");
  o->set_level(6);
  o->set_short("a big moose");
  o->set_long("It's a big moose. It's chewing on some grass.\n");
  o->set_wc(7);
  o->set_whimpy();
  o->set_gender(1+random(2));
  return o;
}
