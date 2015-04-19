inherit "/std/basic_room";

void reset(int arg) {
  if( !arg ) {
    set_light(0);
    set_short("Maze of Twisty Passages");
    set_long("You are lost in a maze of twisty passages, all alike.\n");
    add_property("inside");
    add_item(({ "passages", "twisty passages" }),
	"They all look alike.  You can't find your way around.\n");
    "/room/maze1/vcompile"->add_maze_exits(this_object());
  }
}
