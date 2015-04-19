
inherit "/std/room";

void reset(int arg) {
  if(arg) return;
  set_light(1);
  set_short("At the Grey Havens");
  add_property("outdoors");
  add_property("water");
  set_long("Here is the Grey Havens, the place where the fair people "+
	   "of Middle-Earth leave for the continent beyond the waves. "+
	   "There is a peaceful and serene feeling in the air, although "+
	   "somehow you feel sorrowful. The master of the Havens is Cirdan "+
	   "Shipwright, but you can't see him anywhere near. West a white "+
	   "stone pier leads out into the water. It is where the swanships "+
	   "land and take off, taking people to and from Middle-Earth.");
  
  add_exit("east","/players/tulkas/room2");
  add_exit("west","pier2");
  
  add_item(({"pier","pillars"}),
	   "The pier is made of white stone. The great white swanships land "+
	   "by it, taking people elsewhere.");
  add_item(({"sea","water"}),
	   "The sea is grey, and far out you can see waves breaking.");
}
