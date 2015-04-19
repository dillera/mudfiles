/* Added by Leowon, to straighten up the layout of the land a bit, and
   to allow waterareas to connect, and such. This will also make Tsc's
   area fit in better topolographically. 951217 */

#define CHANCE_TO_PASS 66

inherit "/std/basic_room";

void reset(int arg) {
  if(arg) return;
  add_property("outdoors");
  add_property("water");
  set_light(1);
  set_short("Walking on a beach");
  set_long("You are walking on a beach. The sand is soft beneath your feet. "+
	   "The sound of the waves is soothing to your mind.");
  add_exit("north","/room/east_shore1",0,"check_walk");
  add_exit("south","/room/east_shore3",0,"check_walk");
  add_item(({"shore","shoreline","coast"}),
	   "This part of the coast is a fairly wide beach, with fine sand.");
  add_item(({"sand","beach"}),
           "You are walking on the sand, which is fine and pale. On a warm "+
	   "day, you imagine this would be a pleasant spot to get a tan.");
  add_item("feet",
	   "They are right below you.");
  add_item("foot",
	   "Yep, that is one of them.");
  add_item(({"wave","waves","water","sea","ocean","sea of claws"}),
	   "Waves lap slowly at the shore, darkening the sand as they "+
	   "wet it.");
}

check_walk() {
  if(random(100) < CHANCE_TO_PASS) return 0;
  write("Your feet sink down in the soft sand, hampering your movement.\n");
  return 1;
}
