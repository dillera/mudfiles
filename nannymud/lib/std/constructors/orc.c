object make()
{
  object o;
  o=clone_object("/obj/smartmonster");
  o->set_name("orc");
  o->set_alias("dirty crap");
  o->set_race("orc");
  o->set_level(random(2) + 1);
  o->set_hp(30+random(3));
  o->set_ep(1000+random(40));
  o->set_al(-10-random(80));
  o->set_short("an orc");
  o->set_ac(0);
  o->set_aggressive(1);
  o->set_gender(1);
  o->add_attack("Orc says: Kill him!\n",0,15);
  o->add_attack("Orc says: Bloody humans!\n",0,15);
  o->add_attack("Orc says: Stop him!\n",0,15);
  o->add_attack("Orc says: Get him!\n",0,15);
  o->add_attack("Orc says: Let's rip out his guts!\n",0,15);
  o->add_attack("Orc says: Kill him before he runs away!\n",0,15);
  o->add_attack("Orc says: What is that human doing here!\n",0,15);
  switch(random(3))
  {
  case 0:
    transfer("/std/lib"->make("knife"),o);
    command("wield knife",o);
    break;

  case 1:
    transfer("/std/lib"->make("curved knife"),o);
    command("wield knife",o);
    break;

  case 2:
    transfer("/std/lib"->make("hand axe"),o);
    command("wield axe",o);
    break;
  }
  return o;
}
