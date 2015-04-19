/* A Twohanded, No Multi Attack Sword for /std/lib.c, made by Bive 950716 */

object make() {
  
  object sword;
  sword=clone_object("/std/simple_weapon");
  sword->set_name("sword");
  sword->set_short("a twohanded sword");
  sword->set_long("It's a heavy, twohanded sword.\n");
  sword->set_class(random(3)+14);
  sword->set_weight(4);
  sword->set_value(350+random(100));
  sword->set_type("slash");
  sword->add_property("no_weight_message");
  sword->add_property("steel");
  sword->add_property("two_handed");
  sword->add_property("no_multi_attacks");
  return sword;  
}
