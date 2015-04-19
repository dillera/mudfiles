inherit "/room/room";

reset(arg) {
  if(arg) return;
  set_indoors();
  set_light(1);
  add_exit("room/crypt", "northeast");
  set_short("The Southwestern Crypt");
  set_long("You are standing in a quite dark crypt. However, you\n"+
	   "can see that the walls are clad with dark green velvet.\n");
  add_item(({"walls","wall"}),"The walls are clad with green velvet");    
} /* reset */
