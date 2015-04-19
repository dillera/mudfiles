/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "room/room";
object leather; 

reset(arg) {
  if (!arg) {
    set_light(0);
    set_short("In a maze");
    set_long("You finally find the end of the maze.\n");
    add_exit("/room/maze1/maze4","south");
    set_indoors(); }
  
  if (!leather || !present(leather)) {
    leather=clone_object("obj/armour");
    leather->set_class(5);
    leather->set_name("armour");
    leather->set_alias("leather armour");
    leather->set_value(2000);
    leather->set_short("A leather armour");
    leather->set_long("A smooth leather armour decorated with golden\n"+
		      "threads and silver buttons.\n");
    leather->set_weight(2);
    leather->set_type("armour");
    move_object(leather, this_object()); }
}
















