/* Written by Profezzorn */

inherit "/std/room";

void init() 
{
  ::init();
  if (this_player()->query_level(0) < 20)
  {
    write("Hey! You! Puny mortal! " +
	  "What are you trying to do in here? Get out!\n");
    this_player()->move_player("X#room/adv_guild");
  }
}

string foobar(string f)
{
  switch(f)
  {
  case "1": return read_file("/SORT_LEVEL");
  case "2": return read_file("/doc/statistics/levels");
  case "3": return read_file("/doc/statistics/autoloaders");
  case "4": return read_file("/doc/statistics/countries");
  case "5": return read_file("/doc/statistics/puzzles");
  case "6": return read_file("/doc/statistics/quests");
  case "7": return read_file("/doc/statistics/death_and_horror");
  case "8": return read_file("/doc/statistics/diskusage");
  case "9": return read_file("/doc/statistics/objects");
  case "10": return read_file("/doc/statistics/inherits");
  case "11": return read_file("/doc/statistics/includes");
  }
}

void reset(int arg) 
{
  object board;
  if(!arg)
  {
    set_light(1);
    set_short("The statistics room");
    set_long("You're in a room filled with graphs, tables and notes.\n");
  }

  add_item(({"1","table 1","top player list"}),"@foobar(1)");
  add_item(({"2","table 2","level statistics"}),"@foobar(2)");
  add_item(({"3","table 3","autoloader statistics"}),"@foobar(3)");
  add_item(({"4","table 4","country statistics"}),"@foobar(4)");
  add_item(({"5","table 5","puzzle statistics"}),"@foobar(5)");
  add_item(({"6","table 6","quest statistics"}),"@foobar(6)");
  add_item(({"7","table 7","death and horror statistics"}),"@foobar(7)");
  add_item(({"8","table 8","disk usage statistics"}),"@foobar(8)");
  add_item(({"9","table 9","object type statistics"}),"@foobar(9)");
  add_item(({"10","table 10","inherit statistics"}),"@foobar(10)");
  add_item(({"11","table 11","include statistics"}),"@foobar(11)");

  add_item_cmd("read","1","@read(1)");
  add_item_cmd("read","2","@read(2)");
  add_item_cmd("read","3","@read(3)");
  add_item_cmd("read","4","@read(4)");
  add_item_cmd("read","5","@read(5)");
  add_item_cmd("read","6","@read(6)");
  add_item_cmd("read","7","@read(7)");
  add_item_cmd("read","8","@read(8)");
  add_item_cmd("read","9","@read(8)");
  add_item_cmd("read","10","@read(8)");
  add_item_cmd("read","11","@read(8)");

  add_item(({"statistics","graphs","tables","notes"}),
	"There are 11 major tables available:\n"+
	"  1. Top player list\n"+
	"  2. Level statistics\n"+
	"  3. Autoloader statistics\n"+
	"  4. Country statistics\n"+
	"  5. Puzzle statistics\n"+
	"  6. Quest statistics\n"+
	"  7. Death and horror statistics\n"+
	"  8. Disk usage statistics\n"+
	"  9. Object type statistics\n"+
	" 10. Inherit statistics\n"+
	" 11. Include statistics\n");	

  add_exit("north","inner5");
  add_property("indoors");

  if(!present("board"))
  {
    board=clone_object("obj/newboard");
    board->set_name("statistics","The Statistics board",
		    "This is the board for discussion of the statistisc\n"+
		    "posted in the statistics room.\n",
		    "Profezzorn",817946305,21);
    move_object(board,this_object());
  }
}

void long(string s)
{
  this_player()->simplemore(query_long(s));
  this_player()->flush(); 
}

int read(string what)
{
  long(what);
  return 1;
}

