inherit "/std/room";

/*
#define CONTROLER "/players/rohan/castle/rooms/ferry_control"
#define HARBOUR "/room/middle-earth/pier1"
Also remove comment-marks from one line in ~rohan/castle/room/ferry_control
*/

void reset(int arg) {
  if(arg) return;
  set_light(1);
  set_short("On a white stone pier");
  add_property("outdoors");
  add_property("water");
  set_long("The waves lap gently at the massive stone pillars that support "+
	   "the white stone pier you stand on. All around is the grey sea. "+
	   "It air carries the scent of salt water, and the song of the "+
	   "waves is calling to you.");
  add_exit("east","pier2");
  
  add_item(({"pier","pillars"}),
	   "The pier is made of white stone, and it rests on massive stone "+
	   "pillars that disappear down into the water.");
  add_item(({"sea","water","waves"}),
	   "The sea is grey, with a slight tone of blue. Farther out waves "+
	   "break, creating crests of white foam reminding you of swans.");
  add_item_cmd("listen","sea", line_break("The song of the waves calls to you, beckons "+
	       "to you to sail away, to see what lies beyond the horizon.  "+
	       "\b|\bPRON listens to the sound of the waves."));
  /*
  Perhaps change texts and stuff so you sing to the sea instead.
  add_item("brazier",
           "It is a big shallow bowl made of beaten copper. It can be lit "+
           "to signal a ship to the pier. Beside the brazier is a lit torch "+
           "and a little bowl with green powder in it.");
  add_item("bowl",
           "The bowl is attached to the brazier and contains a green powder.");
  add_item("torch",
           "It is a magical torch that will burn forever.");
  add_item("powder",
           "It looks like it would burn very well.");
  add_item_cmd("light","brazier","@call_ferry()");
  add_command("signal ship","@call_ferry()");
  */

}

int call_ferry() {
  write("You take some of the green powder and put in the brazier.\n"+
	"When you light it with the torch a bright green flame bursts\n"+
	"up from the brazier almost engulfing you spreading sparks\n"+ 
	"several meters away. The flame then stabilizes and burns for\n"+
	"a while with a strong light that can be seen from far away.\n");
  say(this_player()->query_name()+" puts some power in the brazier.\n"+
      "Then "+this_player()->query_pronoun()+" lights it with the torch\n"+ 
      "and it starts to burn with a strong light.\n");
/*   return CONTROLER->call_ferry(HARBOUR),1; */
} /* call_ferry */
