/* A Boy for /std/lib.c, made by Bive 950716 */

object make() {

   object boy;
   boy=clone_object("/obj/smartmonster");
   boy->set_name("boy");
   boy->set_race("human");
   boy->set_short("a boy");
   boy->set_long("A cocky little boy.\n");
   boy->set_level(3);
   boy->set_gender(1);
   boy->set_soul();
   boy->set_response_object(this_object());
   boy->add_response("smiles", "!glare $LOWOTHER");
   boy->add_response("kicks you", "!kill $LOWOTHER");
   boy->add_attack("The boy kicked $OTHER hard.\n",
                   "The boy kicks you hard.\n", 0, 5, 10);
   return boy;
}




