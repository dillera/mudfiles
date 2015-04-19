/* A Queen for /std/lib.c, made by Bive 950716 */

object make() {

   object queen;
   queen=clone_object("/obj/smartmonster");
   queen->set_name("queen");
   queen->set_race("human");
   queen->set_short("a queen");
   queen->set_long("A proud queen.\n");
   queen->set_level(15);
   queen->set_gender(2);
   queen->set_soul();
   queen->add_friend("king");
   queen->set_response_object(this_object());
   queen->add_response("bows to you", "!greet $LOWOTHER");
   queen->add_response("worships you", "!peck $LOWOTHER");
   queen->add_attack("The queen summons a flash from the sky\n",
                     "The queen hit you hard with a flash.\n", 0, 15, 20);
   return queen;
}
