/* A he-goat for /std/lib.c. Brom 950920 */

object make()
{
  object goat;
  goat = clone_object("/obj/smartmonster");
  goat -> set_name("goat");
  goat -> set_race("goat");
  goat -> set_short("a goat");
  goat -> set_long("A he-goat. It looks at you with evil yellow eyes.\n");
  goat -> set_level(random(3) +1);
  goat -> set_gender(1);
  goat -> set_wimpyness(15);
  goat -> add_attack("The goat bashes $OTHER!\n",
		     "The goat bashes you.\n", 11, 10);
  return goat;
}
