object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_race("squirrel");
  o->set_name("squirrel");
  o->set_alt_name("animal");
  o->set_level(random(2)+2);
  o->set_wc(random(4));
  o->set_al(0);
  o->set_aggressive(0);
  o->set_short("a small squirrel");
  o->set_long("It looks like any squirrel. Quick.\n");
  o->set_gender(1+random(2));
  return o;
}
