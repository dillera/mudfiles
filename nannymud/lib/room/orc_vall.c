/* Rewritten using add and set functions in room.c. Banshee 950218 */

inherit "room/room";

reset(arg) {
  object orc, weapon;
  int i,class,value,weight,t;
  string w_name,alt_name;

  if (!arg) {
    set_light(1);
    set_short("The orc valley");
    set_long("You are in the orc valley. This place is inhabited by orcs.\n" +
	     "There is a fortress to the north, with lot of signs of orcs.\n"+
	     "  To the west there is a narrow passage.\n");
    add_exit("room/slope", "east");
    add_exit("room/fortress", "north");
    add_exit("players/taren/road/west_road1", "west");
    add_item("valley","The valley would probably make an idyllic spot for a\n"+
	     "house if it wasn't for the orcs");
    add_item(({"signs","orcshit"}),"You examine the orcshit carefully");
    add_item("fortress","It's a primitive wood fortress");
    add_item("forest","Though this place is inhabited by orcs, the trees\n"+
	     "still look green and healthy");
    set_outdoors(); }
  if (!present("orc")) {
    i=0;
    t=1+random(4);
    while(i<t) {
      i += 1;
      orc = clone_object("obj/smartmonster");
      orc->set_name("orc");
      orc->set_alias("dirty crap");
      orc->set_race("orc");
      orc->set_level(random(2) + 1);
      orc->set_hp(30);
      orc->set_ep(1014);
      orc->set_al(-60);
      orc->set_short("An orc");
      orc->set_ac(0);
      orc->set_aggressive(1);
      orc->add_attack("Orc says: Kill him!\n",0,15);
      orc->add_attack("Orc says: Bloody humans!\n",0,15);
      orc->add_attack("Orc says: Stop him!\n",0,15);
      orc->add_attack("Orc says: Get him!\n",0,15);
      orc->add_attack("Orc says: Let's rip out his guts!\n",0,15);
      orc->add_attack("Orc says: Kill him before he runs away!\n",0,15);
      orc->add_attack("Orc says: What is that human doing here!\n",0,15);
      weapon = clone_object("obj/weapon");
      switch(random(3)) {
      case 0:
	w_name = "knife";
	class = 5;
	value = 8;
	weight = 1;
	break;
      case 1:
	w_name = "curved knife";
	class = 7;
	value = 15;
	weight = 1;
	alt_name = "knife";
	break;
      case 2:
	w_name = "hand axe";
	class = 9;
	value = 25;
	weight = 2;
	alt_name = "axe"; }
      weapon->set_name(w_name);
      weapon->set_class(class);
      weapon->set_value(value);
      weapon->set_weight(weight);
      weapon->set_alt_name(alt_name);
      transfer(weapon, orc);
      command("wield " + w_name, orc);
      move_object(orc, this_object());
    }
  }
}
