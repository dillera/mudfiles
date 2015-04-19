/* A Horse for /std/lib.c, made by Bive 950716 */

object make() {

   object horse;
   horse=clone_object("/obj/smartmonster");
   horse->set_name("horse");
   horse->set_race("horse");
   horse->set_short("a horse");
   horse->set_long("A majestic horse.\n");
   horse->set_level(random(3) +11);
   horse->set_gender(1+random(2));
   horse->set_wimpyness(15);
   horse->add_attack("The horse neighs at $OTHER.\n",
                     "The horse neighs at you.\n", 3, 10);
   return horse;
}
