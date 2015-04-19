/* A Duck for /std/lib.c, made by Bive 950718 */

object make() {

   object duck;
   duck=clone_object("/obj/smartmonster");
   duck->set_name("duck");
   duck->set_race("duck");
   duck->set_short("a duck");
   duck->set_long("It's a ordinary duck.\n");
   duck->set_level(random(2) +2);
   duck->set_wc(1);
   duck->set_ac(1);
   duck->set_gender(random(2)+1);
   duck->set_wimpyness(30);
   duck->set_chat_chance(2);
   duck->add_chat("The duck says: Quack, quack.\n");
   return duck;
}




