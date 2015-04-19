#include <std_mudlib.h>
inherit I_ROOM;

object cobbler, merc;

reset(arg)
{
  if (!arg)
    {
      set_light(1);
      add_property("outdoors");
      add_property("square");
      add_property("authour", "brom");
      set_short("Lazlar square");
      add_exit("south", "elm_road2");
      add_exit("north", "elm_road3");
      set_long
        ("You stand in a small cobbled square. In the middle of the square is\n"+
	 "a fountain. Elm road goes both north and south from here.\n");
      add_item( ({ "road", "elm road" }),
	       "The road, known as Elm Road by the townspeople, comes from the north,\n"+
	       "passes the square and continues south.\n");
      add_item( ({ "square", "cobbled square" }),
	       "It is neat. The stone is laid in a kind of flowery pattern,\n"+
	       "giving the fountain a natural place in the center.\n");
      add_item("fountain",
	       "The water falls from a vessel, held by a young girl, into the\n"+
	       "pond. It is a lively tune it plays.\n");
      add_item("road",
	       "The road goes both north and south.\n");
      add_item( ({"house", "houses" }),
	       "Around the square, the houses are well tended.\n");
      add_item("vessel", "It is a jar in classic style.\n");
      add_item( ({ "girl", "young girl" }),
	       "A sculpture of a young innocent girl. She smiles at the water\n"+
	       "falling from the vessel.\n");
      add_item("smile", "It is a beautiful smile, making you feel young again.\n");
      add_item("pond", "It is clear and not very deep.\n");
      add_item("stone", 
	       "Many different stones have been used to cobble the square.\n"+
	       "The cobbler have arranged them into a flowery patter.\n");
      add_item( ({ "pattern", "flowery patter", "stone flower" }),
	       "The whole square is covered with a flower painted with the\n"+
	       "stones. Ah, this is art!\n");
      add_item("center",
	       "The fountain is at the center of the square.\n");
    }
  if (!cobbler) make_cobbler();
  if (!merc) make_merc();
} /* reset */

init() {
  :: init();
  add_action("cmd_drink", "drink");
}

cmd_drink(str) {
  notify_fail("Drink what from what?\n");
  if (!sizeof(regexp( ({ str }),
		     ".*from.*(pond)|(fountain)"))) return 0;
  if (this_player() -> check_busy()) {
    write("Rest some first.\n");
    return 1;
  }
  write("You take a refreshing drink of water from the fountain.\n");
  say(this_player() -> query_name() +
      " leans down and has a refreshing drink of water from the\n"+
      "fountain.\n");
  return 1;
} /* cmd_drink */

make_cobbler() 
{
  object item;
  cobbler = clone_object("obj/smartmonster");
  cobbler -> set_name("eric");
  cobbler -> add_alias("cobbler");
  cobbler -> set_short("Eric the cobbler");
  cobbler -> set_male();
  cobbler -> set_long
    ("Eric the cobbler is a middle aged man with gray hair and green\n"+
     "eyes. He has powerful arms and shoulders as befits a man who lifts\n"+
     "cobble stone all day long, most days of the year.\n");
  cobbler -> set_level(4);
  cobbler -> set_chat_chance(20);
  cobbler -> add_chat("The cobbler says: Hm. Perhaps.\n");
  cobbler -> add_chat("The cobbler wounders: It might need a touch there...\n");
  cobbler -> add_chat("The cobbler says: But if I do that there, and here?\n");
  cobbler -> add_chat("The cobbler asks: Perchance a lighter stone?\n");
  cobbler -> add_chat("The cobbler says: A red one would fit HERE.\n");
  cobbler -> add_chat("The cobbler asks: Perhaps that is too obvious?\n");
  cobbler -> add_chat("The cobbler says: A real art work, this.\n");
  cobbler -> add_chat("The cobbler says: Two persons could do this: me and my apprentice.\n");
  cobbler -> add_chat("The cobbler exclaims: I just might put a string of blue over there!\n");
  cobbler -> add_chat("The cobbler exclaims: Ha! That's right! A hard, rough edge!\n");
  cobbler -> add_chat("The cobbler fingers his chin.\n");
  cobbler -> add_chat("The cobbler wipes his brow.\n");
  cobbler -> add_chat("The cobbler tugs at his hair.\n");
  cobbler -> add_chat("The cobbler takes a few steps, examining the ground carefully.\n");
  cobbler -> add_chat("The cobbler looks closely on a stone.\n");
  cobbler -> add_chat("The cobbler examines his work.\n");
  cobbler -> add_chat("The cobbler paces the place, deep in thought.\n");
  cobbler -> add_attack
    ("The cobbler hits $OTHER with a rock!\n", 
     "The ccobbler hits you with a rock!\n",
     10, 12);
  cobbler -> add_attack
    ("The cobbler smashes $OTHER with a rock!\n",
     "The cobbler smashes you with a rock!\n",
     10, 13);
  cobbler -> add_attack
    ("The cobbler bops $OTHER on the head with his hammer!\n",
     "The cobbler bops you on the head with his hammer!\n",
     10, 14);
  cobbler -> add_attack
    ("The cobbler treats $OTHER like any recalcitrant blockhead!\n",
     "The cobbler treats you like any recalcitrant blockhead! Ouch!\n",
     5, 7);
  cobbler -> add_attack
    ("The cobbler sweeps the street with $OTHER!\n",
     "The cobbler sweeps the street with you!\n",
     1, 26);
  cobbler -> add_attack
    ("The cobbler chisels some sense into $OTHER's head!\n",
     "The cobbler smacks his chisel into your head!\n",
     1, 19);
  cobbler -> add_attack
    ("The cobbler expertedly throws a stone at $OTHER!\n",
     "The cobbler expertedly throws a stone at you!\n",
     1, 5);
  cobbler -> add_attack
    ("The cobbler smacks $OTHER over the mouth with his hammer!\n",
     "The cobbler smacks you over the mouth with his hammer!\n",
     1, 14);

  item = clone_object(I_TREASURE);
  item -> set_weight(1);
  item -> set_value(6);
  item -> set_name("stone");
  item -> add_alias("rock");
  item -> add_alias("small rock");
  item -> set_short("A small rock");
  item -> set_long("It is a small rock, with rough edges. It looks like a cobble stone.\n");
  item -> add_item( ({ "edge", "edges", "rough edge", "rough edges" }),
		   "Someone have worked on the stone with a chisel, leaving the edges\n"+
		   "rough and un-even.\n");
  item -> add_property("stone");
  move_object(item, cobbler);

  item = clone_object(I_WEAPON);
  item -> set_name("hammer");
  item -> add_alias("cobblers hammer");
  item -> set_short("A cobblers hammer");
  item -> set_long("It is a sturdy tool of the cobblers trade.\n");
  item -> set_weight(1);
  item -> set_class(4); /* Not really for fighting. */
  item -> set_value(27);
  item -> set_type("crush");
  move_object(item, cobbler);

  item = clone_object(I_WEAPON);
  item -> set_name("chisel");
  item -> add_property("steel");
  item -> set_short("An iron chisel");
  item -> set_long("It has been heavily used; the tip is rather blunt.\n");
  item -> set_weight(1);
  item -> set_class(6);
  item -> set_value(12+random(5));
  item -> set_type("pierce");
  move_object(item, cobbler);

  move_object(cobbler, this_object());
}

make_merc()
{
  object item;

  merc = clone_object("obj/smartmonster");
  merc -> set_name("bertrand");
  merc -> add_alias("mercenary");
  merc -> add_alias("warrior");
  merc -> add_alias("fighter");
  merc -> set_short("A warrior");
  merc -> set_long
    ("Bertrand is a stout middle-aged warrior. He has the look of a competent\n"+
     "soldier, and the equipment, too.\n");
  merc -> set_level(5);
  merc -> set_chat_chance(10);
  merc -> add_chat("Bertrand checks his armour.\n");
  merc -> add_chat("Bertrand polishes his armour.\n");
  merc -> add_chat("Bertrand has a look at his armour.\n");
  merc -> add_chat("Bertrand makes a show of inspecting his sword.\n");
  merc -> add_chat("Bertrand polishes his sword.\n");
  merc -> add_chat("Bertrand checks the wrapping in the hilt of his sword.\n");
  merc -> add_chat("Bertrand looks grim.\n");
  merc -> add_chat("Bertrand looks determined.\n");
  merc -> add_chat("Bertrand looks sternly at you.\n");
  merc -> add_chat("Bertrand exclaims: Bah! Thieves all of them!\n");
  merc -> add_chat("Bertrand exclaims: Worse than trolls, those adventurers!\n");
  merc -> add_chat("Bertrand asks: Disturbing the peace, are we?\n");
  merc -> add_chat("Bertrand sighs deeply.\n");
  merc -> add_attack
    ("Bertrand shouts: Run for your worthless life!\n", 0, 10, 0);
  merc -> add_attack
    ("Bertrand shouts: Guards! Guards!\n", 0, 10, 0);
  merc -> add_attack
    ("Bertrand parries $OTHERs attack!\n", "Bertrand parries your attack!\n",
     5, 0);
  merc -> add_attack
    ("Bertrand tricks $OTHER! $OTHER looks like an incompetent!\n",
     "Bertrand tricks you! You blush, looking like an incompetent!\n", 5, 10);
  merc -> add_attack
    ("Bertrand kicks $OTHER on the knee with real expertise.\n",
     "Betrand kicks you on the knee with real expertise!\n", 5, 12);
  merc -> add_attack
    ("Bertrand says: Yield and I will kill you quickly!\n", 0, 5, 0);
  merc -> add_attack
    ("Bertrand says: Stand, and I will make this last!\n", 0, 5, 0);
  merc -> add_attack
    ("Bertrand spits $OTHER in the eye!\n", "Bertrand spits you in the eye!\n",
     5, 2);
  if (!random(100)) /* 1 chance on 10000 this will appear... */
    merc -> add_attack
      ("Bertrand slashes $OTHER over the throat!\n",
       "Bertrand slashes you over the throat!\n", 1, 100);
  
  item = clone_object(I_ARMOUR);
  item -> set_type("armour");
  item -> set_name("breastplate");
  item -> add_alias("plate");
  item -> add_alias("breast plate");
  item -> set_short("A well-polished breast plate");
  item -> set_long
    ("An iron breast plate, polished until it gleams. Still, all those deep\n"+
     "scartches make it far from a mirror.\n");
  item -> add_item( ({ "scratch", "scratches" }),
		   "Old marks of battle, those.\n");
  item -> add_property("iron");
  item -> set_class(5);
  item -> set_weight(2);
  item -> set_value(179);
  move_object(item, merc);

  item = "/std/lib" -> make("shortsword");
  move_object(item, merc);

  move_object(merc, this_object());
	
}
