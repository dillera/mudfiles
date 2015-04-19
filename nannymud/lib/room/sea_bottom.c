/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/water_room";

object octopus;

make_octopus() {
  object chest;
  object money;
  octopus = clone_object("obj/monster");
  octopus->set_name("octopus");
  octopus->set_level(9);
  octopus->set_hp(100);
  octopus->set_wc(12);
  octopus->set_al(-20);
  octopus->set_short("An octopus");
  octopus->set_long("A very big octopus with long arms, reaching for you.\n");
  octopus->set_spell_mess1("The octopus says: Mumble");
  octopus->set_spell_mess2("The octopus says: I will convert you to a pulp!");
  octopus->set_chance(20);
  move_object(octopus, this_object());

  chest = clone_object("obj/chest");
  move_object(chest,octopus);
  
  money = clone_object("obj/money");
  money->set_money(random(500));
  move_object(money,chest);
}

reset(int arg) {
  ::reset(arg);
  if (!arg) {
    set_light(1);
    set_short("Sea bottom");
    set_long("You are at the bottom of the sea.\n");
    property = "waterfilled";
    set_surface("room/sea");
    set_outdoors();
    add_exit("room/sea","up"); }
  if (!octopus || !living(octopus)) make_octopus();
}
