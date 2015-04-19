/* A Medium Dragon for /std/lib.c, made by Bive 950716 */

object make() {

   object mdragon;
   mdragon=clone_object("/obj/smartmonster");
   mdragon->set_name("dragon");
   mdragon->set_race("dragon");
   mdragon->set_short("a medium dragon");
   mdragon->set_long("The medium dragon looks dangerous.\n");
   mdragon->set_level(random(2) +10);
   mdragon->set_wc(10);
   mdragon->set_ac(4);
   mdragon->set_al(-100);
   mdragon->set_gender(random(3));
   mdragon->set_chat_chance(5);
   mdragon->add_chat("The dragon moves slowly.\n");
   mdragon->add_chat("The dragon blow flames through the nostrils.\n");
   mdragon->add_attack("The dragon swat $OTHER with the tail.\n",
                       "The dragon swat you with the tail.\n", 10, 30);
   mdragon->add_attack("The dragon bites $OTHER.\n",
                       "The dragon bites you hard.\n", 7, 10);
   return mdragon;
}




