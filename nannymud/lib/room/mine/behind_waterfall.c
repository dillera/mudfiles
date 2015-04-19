/* Rewritten using /std/room.c, Mats 960222 */

inherit "/std/room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if(arg) return;

  set_light(1);
  add_property("cave");
  add_property("water");

  set_short("Underground Cavern, Behind Waterfall");
  set_long(LB("You are in a hidden cavern behind a subterranean "+
	"waterfall.  Though the waterfall is tremendously large "+
	"and only a few feet away, there are some strange acoustics "+
	"here that keep its roar down to but a soft whisper.  "+
	"Growing on the walls of the cavern in the moist air are "+
	"several species of unusual fungi which emit a pale, greenish "+
	"light."));

  add_item("waterfall", LB("The waterfall to the south rushes by.  "+
	"You can see that you are at its base, and you can probably "+
	"easily step through it to the south."));
  add_item("fungi", LB("These strange fungi put out a soft "+
	"greenish light, but, unfortunately, when you scrape some off "+
	"with your finger, you notice it immediately loses all "+
	"luminous qualities when separated from the cavern wall."));

  add_exit("south","cavern_north");
  add_command("drink %s","@/room/hill7->drink_verb()");

  add_object("an emerald");
}
