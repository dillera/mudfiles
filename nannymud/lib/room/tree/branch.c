inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void make_animal() {
  object o,m;
  int i;

  i = random(3);

  m = clone_object("obj/monster");
  m->set_name("forest animal");
  m->set_level(1);
  m->set_hp(30);
  m->set_al(20); /* innocent little animal */
  move_object(m, this_object());
  if( i==0 ) {
     m->set_alias("squirrel");
     m->set_alt_name("furry squirrel");
     m->set_short("a furry squirrel");
     m->set_long("A squirrel, in search of acorns to store for winter.\n");
     o=clone_object("obj/thing");
     o->set_name("acorn");
     o->set_aliases(({"ash acorn"}));
     o->set_short("an acorn");
     o->set_long("It's a little acorn.\n");
     o->set_value(3);
     o->set_weight(0);
     move_object(o, m);
  }
  if( i==1 ) {
     m->set_alias("blackbird");
     m->set_alt_name("bird");
     m->set_short("a blackbird");
     m->set_long("A blackbird, happily hopping about looking for things for its nest.\n");
     o=clone_object("obj/thing");
     o->set_name("twig");
     o->set_short("a twig");
     o->set_long("A tiny twig.\n");
     o->set_value(2);
     o->set_weight(0);
     move_object(o, m);
  }
  if( i==2 ) {
     m->set_alias("ant");
     m->set_alt_name("tree ant");
     m->set_short("a tree ant");
     m->set_long("A tree ant, looking for food for his colony.\n");
     m->set_hp(10);
     m->set_exp(20);
     o=clone_object("obj/food");
     o->set_name("crumb");
     o->set_short("a crumb");
     o->set_long("It might provide substanence...for an ant.\n");
     o->set_eating_mess(" eats a crumb.\n");
     o->set_eater_mess("You eat a crumb.\n");
     o->set_strength(1);
     move_object(o, m);
  }
}

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("outside");

    set_short("Out on a Limb");
    set_long(LB("Out on a Limb\n"+
	"You are standing precariously on one of the lower branches of "+
	"the tall ash tree.  Leaves surround you on all sides, so you "+
	"can't see down or up very well, but you can make out a wooden "+
	"structure above you some 50 feet, and the ground an equal "+
	"distance below you."));

    add_item(({"structure", "castle"}),
	LB("The structure sprawls throughout the tree above you, with "+
	"many levels and a complex plan."));
    add_item("ground", "It is far below you....\n"+
	"You feel dizzy looking down.\n");
    add_item("leaves", "Leaves surround you on all sides.\n");
    add_item(({"tree", "ash"}), "You can't see the tree for the leaves.\n");
  }
  if( !present("forest animal") && !random(4) )
    make_animal();
}
