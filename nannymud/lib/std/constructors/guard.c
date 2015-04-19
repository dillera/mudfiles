/*  guard make() by Rand
    some attack-shouts from /players/mthead/monster/sentry.c
    950709 V1.0 
*/

object make()
{
  object g;
  g=clone_object("/obj/smartmonster");
  g->set_name("guard");
  g->set_alias("dirty crap");
  g->set_race("human");
  g->set_short("a guard");
  g->set_long("A tall, muscular guard with a flinty gaze.\n");

  g->set_level(random(3) + 1);
  g->set_hp(35 + random(5));
  g->set_ep(1000 + random(40));
  g->set_al(-50+random(100));          /* -50<= al < 50 */
  g->set_ac(0);
  g->set_aggressive(0);
  g->set_gender(1);

  g->add_attack("Guard shouts: Alarm!\n", 0, 10);
  g->add_attack("Guard shouts: Up! Up! Rally to me!\n", 0, 10);
  g->add_attack("Guard says: Halt!\n", 0, 10);
  g->add_attack("Guard says: Time to die, stranger!\n", 0, 10);
  g->add_attack("Guard says: Death before dishonor!\n", 0, 10);
  g->add_attack("Guard says: If I were you I'd leave and never come back.\n", 
		0, 10);
  g->add_attack("Guard says: Give up or die, fool.\n", 0, 10);
  g->add_attack("Guard shouts: Help! We're under attack.\n", 0, 10);
  g->add_attack("Guard screams: DIE!\n", 0, 10);
  g->add_attack("Guard shouts: Resistance is useless!\n", 0, 10);
  
  switch(random(3))
  {
  case 0:
    transfer("/std/lib"->make("sword"), g);
    command("wield sword", g);
    break;

  case 1:
    transfer("/std/lib"->make("spear"), g);
    command("wield spear", g);
    break;

  case 2:
    transfer("/std/lib"->make("hand axe"), g);
    command("wield axe", g);
    break;
  }
  return g;
}
