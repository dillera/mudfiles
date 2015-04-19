/* A King for /std/lib.c, made by Bive 950716 */

object make() {

   object king;
   king=clone_object("/obj/smartmonster");
   king->set_name("king");
   king->set_race("human");
   king->set_short("a king");
   king->set_long("A handsome king.\n");
   king->set_level(18);
   king->set_hp(300);
   king->set_gender(1);
   king->add_friend("queen");
   king->set_soul();
   king->set_response_object(this_object());
   king->add_response("bows to you", "!bow $LOWOTHER");
   king->add_response("greets you", "!greet $LOWOTHER");
   king->add_attack("The king throws a ball of fire at $OTHER.\n",
                   "The king hit you with a fireball.\n", 0, 30, 20);
/*
   king->add_attack("The king hit $OTHER with his spear.\n",
                   "The king hit you with his spear.\n", 0, 5, 10);
*/
   transfer("/std/lib"->make("longsword"),king);
   command("wield sword",king);
   return king;
}
