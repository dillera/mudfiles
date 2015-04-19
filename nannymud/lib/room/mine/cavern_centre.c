/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(0);
  add_property("cave");
  add_property("water");

  set_short("Underground Cavern");
  set_long(LB("You are in the centre of a large subterranean cavern, "+
	"deep in the Dragonspine mountains.  Just north, in front of you, "+
	"a huge waterfall tumbles down out of a hole near the ceiling "+
	"of the cavern.  The river it forms rushes by at your feet, "+
	"bending to take a southeasterly course."));

  add_item("river", LB("The river is about 8 feet wide here and several "+
	"feet deep.  It flows past at an incredibly speed, and its roar "+
	"makes conversation impossible.  Occasionally, you see some "+
	"tiny white fish swept along by its irresistable pull."));
  add_item("fish", LB("Apparently some small blind fish swim the "+
	"waters of these caverns."));
  add_item("waterfall", "Going north will give you a better view.\n");

  add_exit("north","cavern_north");
  add_exit("south","cavern_south");

  add_command("drink %s","@/room/hill7->drink_verb()");
  add_command("whisper %s","You certainly can't whisper through this noise!\n");
  add_command(({"say %s","' %s"}),"You try to say something, but your "+
    "pitiful voice cannot be heard above\nthe roar of the waterfall.\n"+
    "\b|\bPRON mouths some words, but you can't hear \bposs voice above "+
    "the roar\nof the waterfall.\n");
}
