/* A Twohanded, big Axe for /std/lib.c, made by Bive 950716 */

object make() {
  
  object axe;
  axe=clone_object("/std/simple_weapon");
  axe->set_name("axe");
  axe->add_alias("big axe");
  axe->set_short("a big axe");
  axe->set_long("It's a big two-handed doublebladed axe.\n");
  axe->set_class(random(2)+13);
  axe->set_weight(4);
  axe->set_value(500);
  axe->set_type("chop");
  axe->set_property("steel");
  axe->add_property("two_handed");
  return axe;  
}
