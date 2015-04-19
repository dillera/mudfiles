/* Earendil -- July 17, 95 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

int virgin_lilypad;

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("outdoors");

    set_short("Marsh, near River");
    set_long(LB("The plains sweep down here to a low-lying marsh beside "+
	"a south-flowing river.  The air is damp and filled with the "+
	"sounds of insects and many frogs, who sit perched about "+
	"on lilypads.  The marsh becomes unpassable to the south, "+
	"but the plains continue west.  There is an old ruin in that "+
	"direction."));

    add_item("river", LB("The river flows down from the northern "+
	"mountains past this marsh and on southwards.  Its waters "+
	"are wide, deep, and very cold.  "+
	"Across its way you see a land of rolling hills, and some forest."));
    add_item(({"ruin","old ruin"}), LB("It is an old ruined castle that "+
	"sits on the plains to the west.  No hint remains from here of "+
	"who built it or how old it might be."));
    add_item("insects", "Many insects seem to reside here.\n");
    add_item("frogs", LB("Many frogs live in and around this marsh.  "+
	"Their contented 'ribbits' fill the air with a rich, deep "+
	"hum.  They perch on lilypads around the marsh.  One large "+
	"lilypad lies in front of you."));
    add_item(({"lilypad", "pad", "lily pad", "lily", "lilypads"}),
        "@lilypad_long()");
    add_item("marsh", LB("This low-lying part of the plain forms "+
	"a wet marsh that leads right to the edge of the river.  "+
	"Many lilypads float around on the still and stagnant waters "+
	"here."));
    add_item("mountains", LB("Off in the distance to the north are the "+
	"tall and craggly mountains which shadow these lands. "+
	"Somewhere up there is the source of this river.")); 
    add_item("hills", LB("The hills fill the land to the east.  They "+
	"seem largely uninhabited.  A slight mist hangs over them so "+
	"that you cannot see very far into the distance."));
    add_item("mist", LB("The mist, which only obscures the distances, "+
	"lends to the loneliness of that hilly land."));
    add_item(({"plain","plains"}), LB("The plains stretch out to the west.  "+
	"Here and there dotting them are small castles and old ruins.  "+
	"In the far distance to the west is a gigantic tree, standing "+
	"alone at the entrance to a dark valley."));
    add_item("valley", "It is dark and evil-looking.\n");
    add_item("tree", "An enormous oak growing to the west, it stands "+
	 "alone on the plains,\ntall and proud.\n");
    add_item("source", LB("You cannot see the source of the river from here."+
	"  It is said that it emerges from some dark cavern high in the "+
	"mountains, and is in fact a river that flows through the "+
	"mountains from some northern land, entering on the other "+
	"side and exiting here in the south, but of this rumour nothing "+
	"has been proven."));
    add_item("forest",
	     "It is a light forest covering the easterm hills.");

    add_exit("north", "/room/ford");
    add_exit("west", "/room/ruin");
    add_exit("east", "/room/bridge2");
  }
  if( !present("frog") ) {
    tell_room(this_object(), 
	"A frog hops into the marsh and settles itself down on "+
	"a big lilypad.\n");
    move_object(clone_object("/room/obj/plane9_frog"), this_object());
  }
  if( !random(5) )
    tell_room(this_object(),
      "You notice the frog quickly hide something underneath the lilypad.\n");
  virgin_lilypad = 1;		/* An untouched lilypad */
}

void init() {
  add_action("_drink_verb", "drink");
  add_action("_south_verb", "south");

  add_action("_look_verb", "look");
  add_action("_lift_lilypad_verb", "lift");
  add_action("_lift_lilypad_verb", "search");
  add_action("_lift_lilypad_verb", "move");
  ::init();
}

int _drink_verb(string s) {
  if( s == "water" || s == "from river" || s == "from water" ) {
    write("You drink some water from the marsh.  It is stagant!\n"+
	"You quickly spit it out!!\n");
    say(this_player()->query_name()+" drinks some marsh water and "+
	"quickly spits it out.\n");
    return 1;
  }
  notify_fail("Drink what?\n");
}

int _south_verb() {
  write("The marsh becomes too wet and the mud too deep for you to\n"+
	"continue south.  It doesn't reach very far south, but you\n"+
	"must find some other way around it.\n");
  return 1;
}

int _lift_lilypad_verb(string str) {
  object scope;

  if( str!="lilypad" && str!="pad" && str!="lily pad" && str!="lily" ) {
    notify_fail("What is it that you wish to "+query_verb()+"?\n");
    return 0;
  }
  if( present("frog") ) {
    write("You can't do that while the frog is atop the lilypad!\n");
    say(this_player()->query_name()+" attempts to "+
	(query_verb()=="look" ? "look under" : query_verb())+
	" the lilypad, but there is a frog on it.\n");
    return 1;
  }
  write("You lift up the large lilypad and peer underneath it.\n");
  say(this_player()->query_name()+" lifts up the large lilypad and "+
	"peers underneath it.\n");
  if( !virgin_lilypad ) {
    write("However, you find nothing there.\n");
    say("However, "+this_player()->query_pronoun()+" finds nothing.\n");
    return 1;
  }
  virgin_lilypad = 0;
  write("Amazingly, you find a stethoscope under it!  You take it.\n");
  say("Amazingly, "+this_player()->query_pronoun()+" finds a "+
	"stethoscope under it!\n");
  if( transfer(scope = clone_object("/obj/stethoscope"), this_player()) ) {
    write("You are carrying too much to hold it, so you drop it.\n");
    say(this_player()->query_name()+" is carrying too much to hold it.\n");
    move_object(scope, this_object());
  }
  return 1;
}

int _look_verb(string str) {
  if( !str || 
    !(sscanf(str, "under %s", str) || sscanf(str, "beneath %s", str)) )
    return 0;
  if( str=="lilypad" || str=="pad" || str=="lily pad" || str=="lily" )
    return _lift_lilypad_verb("lilypad");
}

string lilypad_long() {
  string str;
  object frog;

  str = "Floating in the marsh near you is an unusually large lilypad.  ";
  if( virgin_lilypad )
    str += "It is slightly bumped up.  ";
  if( frog = present("frog") )
    str += "Sitting atop the large lilypad is a "+
	(frog->query_attack() ? "very angry" : "happy and contented")+
	" frog, wearing a golden crown.";
  return sprintf("%-78=s\n", str);
}
