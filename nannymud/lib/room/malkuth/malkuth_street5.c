#include <std_mudlib.h>
inherit I_ROOM;

reset(arg) {
  if (!arg) {
    set_light(1);
    add_property("outdoors");
    add_property("road");
    add_property("author", "brom");
    add_exit("south", "malkuth_street4");
    if (find_object("players/mortis/castle"))
      add_exit("north", "/players/mortis/shades/alleyent", 0, "check_north"); 
    set_short("A narrow alley");
    set_long
      ("Malkuth street is very narrow here. The houses are old, with barred windows\n"+
       "and no paint. Refuse litters the ground.\n");
    add_item( ({ "street", "road" }),
	     "It is so narrow you can touch both sides at the same time.\n");
    add_item( ({ "house", "houses" }),
	     "The houses are old and worn. The windows are barred and the doors\n"+
	     "are closed.\n");
    add_item("ground", "It is littered with refuse.\n");
    add_item("refuse", 
	     "Household wastes and worse; the stench is unbearable.\n");
  }
  if (!present("thief"))
    make_thief();
} /* reset */

make_thief() {
  object t, o;

  t = clone_object("obj/smartmonster");
  t -> set_name("rwolf");
  t -> add_alias("thief");
  t -> set_short("A sturdy thief");
  t -> set_long
    ("Rwolf was born on the street, and there he grew up too. He is a nasty\n"+
     "fellow with bad breath, yellow teeth and a wicked knife, which he have\n"+
     "no scruples about using to the fullest.\n");
  t -> set_level(14);
  t -> set_al(-72);
  t -> set_race("human");
  t -> set_male();
  t -> set_aggressivity(10);
  t -> set_chat_chance(17);
  t -> add_chat("Rwolf uses his knife on a nearby wall.\n");
  t -> add_chat("Rwolf grins evilly at you.\n");
  t -> add_chat("Rwolf says: Hey! Gotta gold piece for me?\n");
  t -> add_chat("Rwolf says: Gimme yur purse!\n");
  t -> add_chat("Rwolf says: I want what you have. Give or die.\n");
  t -> add_chat("Rwolf says: Come on, now, sirra, to give is to get.\n");
  t -> add_chat("Rwolf says: Better lose a purse than an eye, right?\n");

  t -> add_attack("Rwolf puts his thumb on the blade and thrustes upwards!\n", 
		  0, 11, 5);
  t -> add_attack("Rwolf grins evilly at $OTHER.\n",
		  "Rwolf grins evilly at you.\n", 0, 5);
  t -> add_attack("Rwolf slashes $OTHER over the face!\n",
		  "Rwolf slashes you over the face!\n", 15, 5);
  t -> add_attack("Rwolf goes for $OTHERs eyes, barely missing!\n",
		  "Rwolf goes for your eyes, barely missing.\n", 16, 5);
  t -> add_attack("Rwolf breathes on $OTHER! $OTHER turns white!\n",
		  "Rwolf breathes on you! You feel ill.\n", 0, 5);
  t -> add_attack("Rwolf kicks!\n", 0, 10, 7);
  t -> add_attack("Rwolf bites!\n", 0, 7, 7);
  t -> add_attack("Rwolf snarls!\n", 0, 0, 12);
  t -> add_money(10+random(7));
  o = clone_object(I_WEAPON);
  o -> set_name("knife");
  o -> set_short("a knife");
  o -> set_long
    ("It is a piece of iron, sharpened on one side. A dirty rag wrapped around\n"+
     "one end serves as a handle. The weapon looks contagious.\n");
  o -> set_type("slash");
  o -> set_class(9);
  o -> set_value(50+random(47));
  o -> set_weight(1);
  move_object(o, t);
  move_object(t, this_object());
} /* make_thief */

check_north()
{
  if(present("thief")) {
    write("The thief steps in front of you.\n"); 
    say(this_player() -> query_name()+
	" tries to go north, but the thief blocks the way!\n");
    return 1;
  }
  return 0;
}
