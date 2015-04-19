/* Rewritten using add and set functions in room.c Banshee 950215 */

inherit "room/room";
object rat,black_stone;

reset(arg) {
  if (!arg) {
    set_light(1);
    set_no_castle_flag(1);
    set_short("A black room"); 
    set_long("You are standing in a black room.\n"+
	     "There is a door in the east wall.\n");
    add_exit("room/sub/door_trap","east",0,"check_east"); }

  if (!rat || !present(rat)) {
    rat=clone_object("obj/monster");
    rat->set_name("rat");
    rat->set_alias("black rat");
    rat->set_level(3);
    rat->set_short("An ugly black rat");
    rat->set_wc(5);
    rat->set_agressive();
    move_object(rat, this_object());
    black_stone=clone_object("obj/treasure");
    black_stone->set_id("stone");
    black_stone->set_alias("black stone");
    black_stone->set_short("A black stone");
    black_stone->set_value(60);
    move_object(black_stone, rat); }
}

init() {
  add_action("hmm","exa");
  add_action("hmm","look at");
  ::init();
}

check_east() {
  if (call_other("room/sub/door_trap","query_west_door")==0) {
    write("The door is closed.\n");
    return 1; }
  else return 0; 
}

hmm(str) {
  string a,b;
  if (!str) return;
  if (sscanf(str,"%sdoor%s",a,b)==2) {
    if (call_other("room/sub/door_trap","query_west_door")==0) 
      write("The door is closed.\n");
    else 
      write("The door is open.\n"); 
    return 1; }
  return 0;
}

