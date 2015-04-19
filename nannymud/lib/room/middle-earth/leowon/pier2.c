
inherit "/std/room";

void reset(int arg) {
  if(arg) return;
  set_light(1);
  set_short("At the start of a stone pier");
  add_property("outdoors");
  add_property("water");
  set_long("You stand on the start of a white stone pier, that reaches "+
	   "far out into the water. The scent of the sea permeats the air, "+
	   "and you feel slightly restless.");
  add_exit("east","grey_havens1");
  add_exit("west","pier1");
  
  add_item(({"pier","pillars"}),
	   "The pier is made of white stone, and it rests on massive stone "+
	   "pillars that disappear down into the water.");
  add_item(({"sea","water"}),
	   "The sea is grey, with a slight tone of blue. Farther out waves "+
	   "break, creating crests of white foam reminding you of swans.");
}

void init() {
  ::init();
  add_action("scent","scent");
  add_action("scent","smell");
} 

status scent(string arg) {
  if(!arg || arg == "water" || arg == "air" || arg == "sea") {
    write(line_break("The scent of the great sea stirs a restlessness in "+
		     "you, an urge to seek the end of the horizon, to "+
		     "cross the waves and discover whatever lies beyond."));
    say(this_player()->query_name()+" takes a deep breath and looks out over "+
	"the sea.\n",this_player());
    return 1;
  }
}
    
