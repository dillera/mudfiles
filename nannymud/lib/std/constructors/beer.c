/* A Bottle of Beer for /std/lib.c, made by Bive 950719 */

object make() {

   object beer;
   beer=clone_object("obj/alco_drink");
   beer->set_name("beer");
   beer->set_short("a plain bottle of beer");
   beer->set_long("Drinking it would make you feel better.\n");
   beer->set_drinking_mess(" gulps down the beer.\n");
   beer->set_value(100);
   beer->set_weight(1);
   beer->set_strength(20);
   return beer;
  }
