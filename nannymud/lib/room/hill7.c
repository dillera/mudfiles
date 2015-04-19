/* Earendil -- July 95 */
/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(1);
  add_property("hills");
  add_property("water");

  set_short("Rolling Hills Near River");
  set_long(LB("Here the hills flatten out and slope down to the edge "+
	"of a river to the west.  The river is slow-moving, and is shallow "+
	"enough at this point for you to ford.  Beyond the river you "+
	"catch sight of the open plains.  To the east is a dark and ancient "+
	"wood."));

  add_item("wood", LB("The small forest in the centre of the plains is "+
	"the Old Wood, said in folklore to be one of a few remnants of a "+
	"great forest which covered the whole of Nanny in millenia past, "+
	"before the coming of humans.  It is a dark and mysterious place, "+
	"where ancient powers sleep and dream of their past might."));
  add_item("river", LB("The river flows down from the mountains to the "+
	"north.  It is wide and its waters are ice-cold, and here is one "+
	"of the few places where you may cross it safely."));
  add_item("mountains", LB("Off in the distance to the north are the "+
	"tall and craggly mountains which shadow these lands.  It is "+
	"said that a great pine forest lies beyond them, where men "+
	"and dragons live peacefully, where wealth is abundant, and "+
	"evil does not exist.")); /* Not true, this rumour :) */
  add_item("hills", LB("The hills that surround you are quiet and "+
	"apparently largely uninhabited, except for insects and small "+
	"animals.  The skies are strangely empty of avians, and you "+
	"see no man-made structures around you.  You intuit an unexplained "+
	"spookiness in this forlorn place, however."));
  add_item("mist", LB("The mist, which only obscures the distances, "+
	"lends to the loneliness of the land."));

  add_exit("north","hill8");
  add_exit("east", "old_wood");
  add_exit("south","hill6");
  add_exit("west", "ford");

  add_command("drink %s","@drink_verb()");
}

int drink_verb(string arg, string s) {
  if( s == "water" || s == "from river" || s == "from water" ) {
    write("You drink some river water.  It is cold but very refreshing.\n");
    say(this_player()->query_name()+" drinks some water from the river.\n");
    this_player()->drink_soft(2);
    return 1;
  }
  notify_fail("Drink what?\n");
}
