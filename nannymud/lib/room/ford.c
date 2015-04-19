/* Earendil -- July 17, 95 */
/* Rewritten using /std/basic_room, Mats 960220 */

inherit "/std/basic_room";

void reset(int arg) {
  if(arg) return;

  set_light(1);
  add_property("river");

  set_short("River Ford");
  set_long("Here the wide, cold river running down from the mountains "+
	"widens so that it is shallow enough to cross on foot.  To the west "+
	"are the open plains, stretching as far as the eye can see.  "+
	"Across the river to the east the ground slopes upward to a "+
	"land of rolling hills.  To the south the ground becomes marshy.");

  add_item("river", "The river flows down from the mountains to the "+
	"north.  It is wide and its waters are ice-cold, and here is one "+
	"of the few places where you may cross it safely.");
  add_item("mountains", "Off in the distance to the north are the "+
	"tall and craggly mountains which shadow these lands. "+
	"Somewhere up there is the source of this river."); 
  add_item("hills", "The hills slope upward to the east.  They "+
	"seem largely uninhabited.  A slight mist hangs over them so "+
	"that you cannot see very far into the distance.");
  add_item("mist", "The mist, which only obscures the distances, "+
	"lends to the loneliness of that hilly land.");
  add_item(({"plain","plains"}), "The plains stretch out to the west.  "+
	"Here and there dotting them are small castles and old ruins.  "+
	"In the far distance to the west is a gigantic tree, standing "+
	"alone at the entrance to a dark valley.");
  add_item("valley", "It is dark and evil-looking.\n");
  add_item("tree", "An enormous oak growing to the west, it stands "+
	 "alone on the plains,\ntall and proud.\n");
  add_item("source", "You cannot see the source of the river from here."+
	"  It is said that it emerges from some dark cavern high in the "+
	"mountains, and is in fact a river that flows through the "+
	"mountains from some northern land, entering on the other "+
	"side an exiting here in the south, but of this rumour nothing "+
	"has been proven.");
  add_item("marsh", "The ground becomes marshy to the south.  You "+
	"can hear the calls of insects and frogs in that direction.");

  add_exit("east", "hill7");
  add_exit("south","plane9");
  add_exit("west", "plane8");

  add_command("drink %s","@/room/hill7->drink_verb()");
  add_command("north","The river is too swift and cold for you to walk"+
	      " upstream.\n\bPRON must find some other way, if you wish"+
	      " to reach its source.\n\b|\bPRON tries to go north, but the"+
	      " current forces \bobj back.\n");
}
