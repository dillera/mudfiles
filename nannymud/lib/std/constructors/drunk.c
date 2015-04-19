/* A Drunkard for /std/lib.c, made by Bive 950902 */

object make() {

   object drunk;
   drunk=clone_object("/obj/smartmonster");
   drunk->set_name("drunk");
   drunk->set_race("human");
   drunk->set_short("a drunkard");
   drunk->set_long("A very drunk man.\n");
   drunk->set_level(2+random(5));
   drunk->set_gender(1);
   drunk->set_wimpyness(20);
   drunk->set_chat_chance(10);
   drunk->set_chat_in_fight(0);
   drunk->set_soul();
   drunk->set_response_object(this_object());
   drunk->add_chat("The drunkard drink another beer.\n");
   drunk->add_chat("The drunkard crawl on the floor.\n");
   drunk->add_response("smiles", "!burp $LOWOTHER");
   drunk->add_response("kicks you", "!stumble");
   drunk->response("sings", "!sing");
   return drunk;
}




