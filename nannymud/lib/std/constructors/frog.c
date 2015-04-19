/* A Frog for /std/lib.c, made by Bive 950716 */

object make() {

   object frog;
   frog=clone_object("/obj/smartmonster");
   frog->set_name("frog");
   frog->set_race("frog");
   frog->set_short("a frog");
   frog->set_long("A small frog.\n");
   frog->set_level(random(2) +2);
   frog->set_gender(0);
   frog->set_wimpyness(20);
   frog->set_move_at_reset();
   frog->set_chat_chance(5);
   frog->add_chat("The frog croaks Hriibit, Hrriiibit!\n");
   frog->add_chat("The frog jumps happily around.\n");
   return frog;
}
