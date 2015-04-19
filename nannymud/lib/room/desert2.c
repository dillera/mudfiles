inherit "std/basic_room";

reset(arg) {
  if (arg) return;
  set_light(1);
  add_exit("north","/players/asyvan/sfinx/room/sfinx_front");
  add_exit("east","/room/desert");
  set_short("In the desert");
  set_long("You are walking in the desert, surrounded by sand.\n"+
	   "To the north there is a very big statue high up on a plateau\n"+
	   "of reddish stone.\n");
  add_property("outdoors");
  add_property("desert");
  add_item("sand","Soft sand, shining almost like gold");
  add_item("pyramid","An impressive buidling indeed");
  add_item("sculpture","A huge stone lion with the head of a man.\n"+
	   "It is the famous Sfinx. The legend can tell you stories about\n"+
	   "the treasures buried in in together with the ancient Pharaos.\n");
} /* reset */


