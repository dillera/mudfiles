/* A Girl for /std/lib.c, made by Bive 950716 */

object make() {

   object girl;
   girl=clone_object("/obj/smartmonster");
   girl->set_name("girl");
   girl->set_race("human");
   girl->set_short("a girl");
   girl->set_long("A cute little girl.\n");
   girl->set_level(3);
   girl->set_gender(2);
   girl->set_wimpyness(30);
   girl->set_soul();
   girl->set_response_object(this_object());
   girl->add_response("smiles", "!smile $LOWOTHER");
   girl->add_response("kicks you", "!hit $LOWOTHER");
   girl->add_attack("The girl screams for help!\n", 0, 0, 10);
   return girl;
}
