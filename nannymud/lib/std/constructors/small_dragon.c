/* A Small Dragon for /std/lib.c, made by Bive 950716 */

object make() {

   object sdragon;
   sdragon=clone_object("/obj/smartmonster");
   sdragon->set_name("dragon");
   sdragon->set_race("dragon");
   sdragon->set_short("a small dragon");
   sdragon->set_long("The small dragon looks very friendly.\n");
   sdragon->set_level(random(2) +4);
   sdragon->set_wc(4);
   sdragon->set_ac(2);
   sdragon->set_gender(random(3));
   sdragon->set_wimpyness(20);
   sdragon->set_chat_chance(2);
   sdragon->add_chat("The dragon tries to blow flames but fails.\n");
   sdragon->add_chat("The dragon blows tiny flames.\n");
   sdragon->add_attack("The dragon blow flames on $OTHER.\n",
                       "The dragon blows flames on you.\n", 5, 30);
   sdragon->add_attack("The dragon hit $OTHER with the tail.\n",
                       "The dragon hit you with the tail.\n", 2, 10);
   return sdragon;
}




