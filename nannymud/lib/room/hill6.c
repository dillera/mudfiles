/* Earendil -- July 95 */
/* Rewritten using /std, Mats 960220 */

inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(1);
  add_property("hills");

  set_short("Rolling Hills");
  set_long(LB("You are at the southern edge of the northern land of "+
	"rolling hills.  To your west, a wide and slow-moving river "+
	"flows southward.  Directly south, a tall stone wall is built. "+
	"The hills continue to the north and east."));

  add_item(({"wall", "stone wall"}), LB("The wall is about twelve "+
	"feet high, and, though old, is built well and more than "+
	"sufficient to prevent passage south.  Over the top you spy "+
	"a small castle of sorts, and a single large oak."));
  add_item("castle", LB("It is more like a summer home than a castle "+
	"built for defense."));
  add_item("oak", LB("The large oak grows out of the backyard behind "+
	"the castle.  None of its branches are in reach, however."));
  add_item("river", LB("The river is moving slowly. It is extremely "+
	"cold and very deep, and cannot be crossed at this point.  "+
	"There is a ford to the north, however."));
  add_item("hills", LB("The hills that surround you are quiet and "+
	"apparently largely uninhabited, except for insects and small "+
	"animals.  The skies are strangely empty of avians, and you "+
	"see no man-made structures around you.  You intuit an unexplained "+
	"spookiness in this forlorn place, however."));
  add_item("mist", LB("The mist, which only obscures the distances, "+
	"lends to the loneliness of the land."));

  add_exit("north","hill7");
  add_exit("east", "hill4");

  add_command("south","The wall blocks your way.\n");
  add_command(({"west","swim %s"}),"@swim_verb()");
  add_command("climb %s","@climb_verb()");
}

int climb_verb(string arg, string str) {
  if( str == "wall" || str == "stone wall" ) {
    write(LB("The stone blocks of which the wall is built are too "+
	"closely fitted to allow for handholds.  There is probably "+
	"another entrance someplace else, however."));
    say(this_player()->query_name()+" tries to climb the stone wall, "+
	"but fails.\n");
    return 1;
  }
  notify_fail("Climb what?\n");
}

int swim_verb(string arg, string str) {
  if( !str || str == "river" || str == "in river" ) {
    write(query_long("river"));
    say(this_player()->query_name() + " considers swimming the "+
	"river but thinks better of it.\n");
    return 1;
  }
  query_verb() == "swim" && notify_fail("Swim where?\n");
}
