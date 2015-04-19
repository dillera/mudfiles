inherit "/room/room";

reset(arg) {
  if(arg) return;
  set_indoors();
  set_light(1);
  add_exit("room/crypt", "east");
  set_short("The Western Crypt");
  set_long("You are standing in a quite dark crypt. However, you\n"+
	   "can see that the walls are clad with dark black velvet.\n");
  add_item(({"walls","wall"}),"The walls are clad with black velvet");    
} /* reset */
