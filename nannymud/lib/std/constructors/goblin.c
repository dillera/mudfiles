object make()
{
  object o;

  o = clone_object("obj/monster");
  o->set_name("goblin");
  o->set_race("goblin");
  o->set_level(3);
  o->set_short("a goblin");
  o->set_long
    ("The goblins are dirty creatures, with filthy habits.\n"+
     "This one is exactly like that.\n");
  o->add_money(10+random(20));
  return o;
}
