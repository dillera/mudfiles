/* Rewritten using /std/basic_room.c, Mats 960222 */

inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(0);
  add_property("cave");
  add_property("water");

  set_short("Underground Cavern");
  set_long(LB("Here the tunnel ends, opening up into a large underground "+
	"cavern.  A huge waterfall tumbles down on the north end of the "+
	"cavern, its roar making all normal conversation impossible.  "+
	"The waterfall pours into an underground river which rushes out "+
	"through a low tunnel to the southeast."));

  add_exit("north","cavern_centre");
  add_exit("south","tunnel_room");

  add_command("drink %s","@/room/hill7->drink_verb()");
  add_command("whisper %s","You certainly can't whisper through this noise!\n");
  add_command(({"say %s","' %s"}),"You try to say something, but your "+
    "pitiful voice cannot be heard above\nthe roar of the waterfall.\n"+
    "\b|\bPRON mouths some words, but you can't hear \bposs voice above "+
    "the roar\nof the waterfall.\n");
  add_command("southeast","You can't enter the river tunnel!\n"+
	"At the speed it is rushing by, you would surely die by its\n"+
	"violence alone, if drowning did not take you first.\n");
}
