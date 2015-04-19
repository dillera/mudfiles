/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/room";
object hobgoblin,money;

reset(arg) {
  if (!arg) {
    set_light(0);
    set_short("A small room");
    set_long("You are standing in a small room in the mountain.\n"+
	     "The walls are very roughly cut and water is dripping from the ceiling.\n");
    add_exit("room/mine/tunnel5","south");
    set_indoors(); }

  if (!hobgoblin || !living(hobgoblin)) {
    hobgoblin = clone_object("obj/monster");
    hobgoblin->set_name("hobgoblin");
    hobgoblin->set_level(5);
    hobgoblin->set_wc(9);
    hobgoblin->set_short("a hobgoblin");
    hobgoblin->set_long("This hobgoblin looks really nasty.\n");
    move_object(hobgoblin, this_object());
    money = clone_object("obj/money");
    money->set_money(10+random(50));
    move_object(money, hobgoblin); }
}
