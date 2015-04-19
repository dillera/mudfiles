/* Number of rooms through maze */
#define LENGTH_OF_MAZE 6

string *path_thru_maze;
string *directions = ({ "north", "south", "east", "west" });

string guess_direction() {
  return directions[random(sizeof(directions))];
}
  
void add_maze_exits(object r);

void reset(int arg) { 
  int i;
  object r;
  path_thru_maze = map_array(allocate(LENGTH_OF_MAZE), "guess_direction",
	this_object());
  for( i=1; i<=LENGTH_OF_MAZE; i++ )
    if( r = find_object("/room/maze1/maze"+i) )
      add_maze_exits(r);
}

object compile_virtual(string filename) {
  int num, i;
  object room, leather;

  if( sscanf(filename, "/room/maze1/maze%d", num) && num >= 1 &&
	num <= LENGTH_OF_MAZE ) {
    room = clone_object("/room/maze1/maze_room");
    if( num == LENGTH_OF_MAZE ) {
      leather = clone_object("/obj/armour");
      leather->set_class(5);
      leather->set_name("leather armour");
      leather->set_alias("armour");
      leather->set_type("armour");
      leather->set_value(250);
      leather->set_short("a suit of leather armour");
      leather->set_weight(2);
      move_object(leather, room);
      return room;
    }
    return room;
  }
}

void add_maze_exits(object room) {
  int i, num;

  if( !sscanf(file_name(room), "room/maze1/maze%d", num) )
    return;
  if( num == LENGTH_OF_MAZE )
  {
    room->set_long("You have finally reached the end of the maze.\n");
    room->add_exit("north", "/room/maze1/maze"+(num-1));
    return;
  }
  num--;
  for( i=0; i<sizeof(directions); i++ ) {
    if( directions[i] == path_thru_maze[num] ) 
      room->add_exit(directions[i], "/room/maze1/maze"+(num+2));
    else
      room->add_exit(directions[i], "/room/well");
  }
}

void _exit() {
  int i;
  object r;

  for( i=1; i<=LENGTH_OF_MAZE; i++ )
    if( r = find_object("/room/maze1/maze"+i) ) {
      write(sprintf("Destructing %O\n", r));
      destruct(r);
    }
}
