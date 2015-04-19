inherit "/std/stationary";
 
object innerobj;

reset(arg) {
  if (arg) return;
  add_property("no_clean_up");
}

init() {
  add_action("board","board");
  add_action("board","enter");
} /* init */

set_inner(obj) {
  innerobj=obj;
} /* set_inner */

board(str) {
  if (!id(str)) return 0;
  write("You board the "+query_name()+".\n");
  this_player()->move_player("into the "+query_name(),innerobj);
  return 1;
} /* board */
