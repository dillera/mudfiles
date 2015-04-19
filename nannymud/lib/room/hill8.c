/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(1);
  add_property("hills");
  add_property("water");

  set_short("Rolling hills");
  set_long("You are in some rolling hills and they continue in all directions.\n" +
      "To the west you can see a cascading river.\n"+
      "There is a stone house here.\n");
  add_item("river","The water is wild and you even get some drops in yor face.");
  add_item("house","It is a stone house. There is a sign on the door.");
  add_item("door","It is a large wooden door with a sign on.");
  add_item("sign","There is some text on the sign, try to read it.");

  add_item_cmd("enter","house","@enter_house()");
  add_item_cmd("read","sign","The sign says: House of Paragons.\n");

  add_exit("east", "hill5");
  add_exit("south","hill7");
}

enter_house() {
  write("You open the door to the house and enter it.\n");
  this_player()->
    move_player("into the house#room/paragon_entry_room");
  return 1;
}
