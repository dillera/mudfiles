/* An Amulet for /std/lib.c, made by Bive 950716 */

object make()  {

   object amulet;
   amulet=clone_object("/std/simple_armour");
   amulet->set_name("amulet");
   amulet->set_short("an amulet");
   amulet->set_long("An amulet made of clay.\n");
   amulet->set_weight(1);
   amulet->set_value(100);
   amulet->set_type("amulet");
   amulet->set_class(1);
   amulet->set_property("clay");
   return amulet;

}
