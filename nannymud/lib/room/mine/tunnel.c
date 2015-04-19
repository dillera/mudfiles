/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";

reset(arg) {
  if (arg) return;

  set_light(0);
  add_property("cave");

  set_short("Mine entrance");
  set_long("This is the entrance to the mines.\n"+
	   "There is a sign on a pole.\n");
  add_exit("north","tunnel2");
  add_exit("south","../mount_pass");

  add_item("pole","A wooden pole.");
  add_item("sign","A sign with some text.");
  add_item_cmd("read","sign","                   WARNING !!\n\n"+
	"Beware of falling rocks. Enter on your own risk.\n");
}
