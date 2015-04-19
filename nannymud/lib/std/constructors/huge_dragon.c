/* A Huge Dragon for /std/lib.c, made by Bive 950716 */

object make() {

   object hdragon;
   hdragon=clone_object("/obj/smartmonster");
   hdragon->set_name("dragon");
   hdragon->set_race("dragon");
   hdragon->set_short("a huge dragon");
   hdragon->set_long("The huge dragon is a killer.\n");
   hdragon->set_level(random(2) +17);
   hdragon->set_wc(19);
   hdragon->set_ac(8);
   hdragon->set_gender(random(3));
   hdragon->set_al(-500);
   hdragon->set_chat_chance(5);
   hdragon->add_chat("The dragon tries to blow flames but fails.\n");
   hdragon->add_chat("The dragon blows tiny flames.\n");
   hdragon->add_attack("The dragon throw a ball of fire at $OTHER.\n",
                       "The dragon throws a fireball on you.\n", 30, 25);
   hdragon->add_attack("The dragon hit $OTHER with the wings.\n",
                       "The dragon hit you with the wings.\n", 10, 10);
   return hdragon;
}




