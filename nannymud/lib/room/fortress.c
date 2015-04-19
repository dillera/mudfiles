/* Rewritten using add and set functions in room.c. Banshee 950218 */
/* Rewritten using /std/room.c, Mats 960227 */
/* Fixed by Brom 960323 */
inherit "/std/room";

reset(arg) {
  int i;

  :: reset(arg);
  if (arg) return;

  set_light(1);
  add_property("plain");

  set_short("The orc fortress");
  set_long("This fortress is the local strong point of the orcs.\n"+
	   "There is an entrance to a small room to the north.\n");
  add_item("fortress","It's a small fortress made of logs.");
  add_item("logs","Quite uninteresting logs actually.");
  add_item("point","Well, it's more like a place really.");
  add_item("place","Well, not exactly a place, more like a spot actually.");
  add_item("spot","And what a great spot for a stronghold it is.");
  add_item("stronghold","It's not much of a stronghold really, but orcs can be tricky.");
  add_item("room","It seems to be important to the orcs.");
  add_item("entrance","It leads north into the room there.");
  add_exit("south","orc_vall");
  add_exit("north","orc_treasure",0,"@check_north()");
  for(i=0;i<8;i++)
    add_object("@make_orc()");
}

check_north() {
  if(present("orc")) return "An orc bars your way.\n";
}

make_orc() {
  object orc, weapon;
  int n,class,value,weight;
  string w_name,alt_name;

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
  n = random(3);
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
  return orc;
}
