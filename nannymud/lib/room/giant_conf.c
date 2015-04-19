/* Rewritten using /std/room.c, Mats 960223 */

inherit "/std/room";

reset(arg) {
  ::reset(arg);
  if (arg) return;

  set_light(1);
  add_property("mountain");

  set_short("Giants conference of human bashing");
  set_long("You are at the yearly conference of human bashing,\n" +
	   "organized by the giants.\n");
  add_exit("east","giant_lair");

  add_object("@make_giant(frost)");
  add_object("@make_giant(fire)");
  add_object("@make_giant(stone)");
}

make_giant(string type) {
  object o, weapon;
  o = clone_object("obj/monster");
  o -> set_name("giant");
  o -> set_alias(type+" giant");
  o -> set_level(15);
  o -> set_short("A "+(type=="small" ? "":type+" ")+"giant");
  o -> set_long("A"+(type=="small" ? "":" large "+type)+" giant. He looks very dangerous.\n");
  o -> set_ac(5);
  o -> set_al(-150);
  o -> set_aggressive(1);
  o -> set_hp(type=="small" ? 200 : 500);
  if(type != "small") {
    weapon = clone_object("/std/simple_weapon");
    weapon -> set_name("sword");
    weapon -> add_alias("sword of "+ type);
    weapon -> set_short("sword of "+ type);
    weapon -> set_class(17 + random(4));
    switch(type) {
      case "frost":
        weapon -> set_type("cold");
	break;
      case "fire":
	weapon -> set_type("fire");
	break;
      default:
	weapon -> set_type("crush");
	break;
    }
    weapon -> set_weight(3);
    weapon -> set_value(700 + random(3 * weapon -> weapon_class()));
    move_object(weapon, o);
    command("wield sword", o);
  }
  o -> set_wc(20);
  return o;
}

