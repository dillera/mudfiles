/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/room";

#define NAME (this_player()->query_name())
#define HE this_player()->query_pronoun()
#define HIS this_player()->query_possessive()
#define HIM this_player()->query_objective()
#define PO previous_object()

string animal;
object wolfie;
object moose;
int rand; /* For random() in exam etc. */

reset(arg) {
  if (arg) return;
  set_light(1); 
  set_short("In a forest");
  add_item(({"tree","trees"}),"They look like normal trees");
  add_item("forest","It's a large green forest, and even though it seems\n"+
	   "like a nice forest, you feel a little afraid of getting lost");
  add_item(({"bird","birds"}),"You can hear them sing and now and then you see one");
  set_outdoors();
}

init() {
  ::init();
  add_action("exam","exa");
  add_action("exam","look");
}

exam(arg) {
  if(arg && sscanf(arg,"%sanimal%s",arg,arg)) {
    if((rand=random(3))==1) {
      if((rand=random(4))==1) {
	say("A badger runs over the path and disappears in the forest.\n");
	write("When you look around for animals, a badger suddenly runs over the path\n"+
	      "and disappears in the forest.\n");
	return 1; }
      if(rand==0) {
	say("A crow lands in a nearby tree.\n");
	write("You see a crow landing in a nearby tree.\n");
	call_out("fly_away",(random(6)+4));
	return 1; }
      if(rand==2) {
	write("You don`t see any animals, but you feel something on your back.\n");
	say(NAME+" scratches "+HIM+"self on "+HIS+" back.\n");
	call_out("find_spider",random(5)+4);
	return 1; }
      if(rand==3) {
	if(random(1)) {
	  if (!wolfie) {
	    wolfie = clone_object("obj/monster");
	    wolfie->set_name("wolf");
	    wolfie->set_level(3);
	    wolfie->set_short("A wolf");
	    wolfie->set_long("A grey wolf running around. It has\n" +
			     "some big dangerous teeth.\n");
	    wolfie->set_wc(7);
	    wolfie->set_whimpy();
	    move_object(wolfie, this_object());
	    tell_room(this_object(),"A wolf comes from the forest.\n");
	    call_out("wolf_leave",random(20)+4);
	    return 1; }
	  else {
	    destruct(wolfie);
	    remove_call_out("wolf_leave");
	    return tell_room(this_object(),"The wolf runs back into the forest.\n"),1;
	  } }
	else {
	  if (!moose) {
	    moose = clone_object("obj/monster");
	    moose->set_name("moose");
	    moose->set_level(6);
	    moose->set_short("A big moose");
	    moose->set_long("It's a big moose. It's chewing on some grass.\n");
	    moose->set_wc(7);
	    moose->set_whimpy();
	    move_object(moose, this_object());
	    tell_room(this_object(),"A moose comes from the forest.\n");
	    call_out("moose_leave",random(20)+4);
	    return 1; }
	  else {
	    destruct(moose);
	    remove_call_out("moose_leave");
	    return tell_room(this_object(),"The moose gets bored looking at you, so he walks back \n"+
			     "into the forest.\n"); } }
      }
    }
    return (say(NAME+" looks for animals but "+HE+" doesn't see any.\n"),
	    write("You don`t see any animals right now.\n"),1);
  }
  return 0;
}

moose_leave() {
  destruct(moose);
  return tell_room(this_object(),"The moose returns to the forest, in a slow pace.\n");
}

wolf_leave() {
  destruct(wolfie);
  return tell_room(this_object(),"The runs back into the forest, drooling.\n");
}

fly_away() {
  return tell_room(this_object(),"The crow leaves the tree.\n");
}

find_spider() {
  if ((rand=random(4))==0) {
    say(NAME+" screams as "+HE+" finds a big spider on "+HIS+" back.\n");
    return write("You scratch yourself on the back. You scream in horror as you realize \n"+
		 "that the thing on your back is a big, hairy spider.\n"); }
  else {
    call_out("find_spider",random(10)+4);
    return write("You scratch yourself on the back, but you don't find anything.\n"),1;
  }
}
