/* A simple short sword. Brom 960108 */

object make() {
  
  object sword;
  sword = clone_object("/std/simple_weapon");
  sword -> set_name("sword");
  sword -> add_alias("shortsword");
  sword -> add_alias("short sword");
  sword -> set_short("a twohanded sword");
  sword -> set_long("A sturdy short sword of simple design.\n");
  sword -> set_class(random(5)+8);
  sword -> set_weight(2);
  sword -> set_value(75+random(50));
  sword -> set_type("slash");
  sword -> add_property("no_weight_message");
  sword -> add_property("iron");
  sword -> add_property("no_multi_attacks");
  return sword;  
}
