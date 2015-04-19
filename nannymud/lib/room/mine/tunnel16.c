/* Rewritten using /std/room.c, Mats 960222 */

inherit "/std/room";

reset(arg) {
  ::reset(arg);
  if (arg) return;

  set_light(0);
  add_property("cave");

  set_short("Tunnel");
  set_long("In a tunnel leading through the mines.\n");
  add_exit("north","tunnel17",0,"@check_north()");
  add_exit("west","tunnel15");

  add_object("@make_dwarf()");
}

check_north() {
  if (present("dwarf")) {
    write("The dwarf bars the way !\n");
    return 1;
  }
  return 0;
}

make_dwarf() {
  object dwarf;
  dwarf = clone_object("obj/monster");
  dwarf->set_name("dwarf");
  dwarf->set_level(15);
  dwarf->set_al(-100);
  dwarf->set_short("A short and sturdy dwarf");
  dwarf->set_wc(15);
  dwarf->set_ac(5);
  return dwarf;
}
