object make()
{
  object o;
  o=clone_object("obj/smartmonster");
  o->set_name("giant");
  o->set_race("giant");
  o->set_level(15);
  o->set_short("a giant");
  o->set_long("A giant. He looks very dangerous.\n");
  o->set_ac(7);
  o->set_al(-150);
  o->set_aggressive(1);
  o->set_hp(202);
  o->set_wc(20);
  o->add_attack("Giant kicks $OTHER.\n","Giant kicks you.\n",20,10);
  o->set_gender(1+random(2));
  return o;
}

