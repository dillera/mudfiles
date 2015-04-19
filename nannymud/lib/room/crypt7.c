inherit "/room/room";

reset(arg) {
  if(arg) return;
  set_indoors();
  set_light(1);
  add_exit("room/crypt", "west");
  set_short("The Eastern Crypt");
  set_long("You are standing in a quite dark crypt. However, you\n"+
	   "can see that the walls are clad with soft grey velvet.\n");
  add_item(({"walls","wall"}),"The walls are clad with grey velvet");    
} /* reset */
