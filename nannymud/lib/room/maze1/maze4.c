/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "room/room";
int exit_num;

reset(arg) {
  if (!arg) {
    set_light(0);
    set_short("A maze");
    set_indoors();
  }
}

long() {
  write("You are walking around in a maze and you are very confused.\n");
  write("  There are four obvious exits: north, south, west and east.\n");
}
	      
init() {
  add_action("north","north");
  add_action("maze","south");
  add_action("maze","west");
  add_action("maze","east");
  ::init();
}

maze() {
  exit_num=random(this_player()->query_int()/2+1);
  if (!exit_num || exit_num>5) {
    write("That was the way out.\n");
    this_player()->move_player("the maze#room/well");
    return 1; }
  else {
    write("Yes! A wise decision!\n");
    this_player()->move_player("further in the maze#room/maze1/maze"+exit_num);
    return 1; }
}

north() {
  this_player()->move_player("further in the maze#room/maze1/maze5");
  return 1; 
} 
