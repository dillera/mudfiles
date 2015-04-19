/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(0);
  add_property("cave");
  add_property("water");

  set_short("Underground Cavern, Near Waterfall");
  set_long(LB("You stand beside the mighty waterfall as it tumbles "+
	"down perhaps a hundred feet from above.  The noise is almost "+
	"deafening, and you are continuously sprayed by water and "+
	"mist.  The underground river and the cavern continue "+
	"south, but the tunnel ends here and you cannot go further "+
	"north."));

  add_item(({"waterfall", "fall"}), LB("The waterfall is about a hundred "+
	"feet tall, plunging down from a tunnel in the ceiling of "+
	"the cavern.  The water is ice cold and makes you shiver.  "+
	"You might have glimpsed an open area behind the waterfall..."));
  add_item("river", LB("The river continues south, disappearing into "+
	"another tunnel off at the southern end of the cavern."));
  add_item(({"passage", "open area", "area"}), LB("Now that you peer "+
	"closely, you notice a passage hidden behind the waterfall.  "+
	"You could probably reach it by entering the waterfall."));

  add_exit("south", "/room/mine/cavern_centre");

  add_command("up","You cannot climb up beside the waterfall.  Besides, "+
	      "it looks\nas if there is little of interest up there.\n");
  add_item_cmd("search","waterfall","@query_long(passage)");
  add_item_cmd("enter","waterfall","@enter_verb()");
  add_command("drink %s","@/room/hill7->drink_verb()");
  add_command("whisper %s","You certainly can't whisper through this noise!\n");
  add_command(({"say %s","' %s"}),"You try to say something, but your "+
    "pitiful voice cannot be heard above\nthe roar of the waterfall.\n"+
    "\b|\bPRON mouths some words, but you can't hear \bposs voice above "+
    "the roar\nof the waterfall.\n");
}

void init() {
  if( !this_player()->query_wet() ) {
    write("You are sprayed by water from the waterfall.\n\n");
    this_player()->set_wet(60);
  }
  ::init();
}

enter_verb() {
  write("You step behind the waterfall and discover a hidden passage!\n\n");
  say(this_player()->query_name() + " enters the waterfall, discovering "+
	"a hidden passage.\n");
  this_player()->move_player("behind the waterfall",
	"/room/mine/behind_waterfall");
  return 1;
}

