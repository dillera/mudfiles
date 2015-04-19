inherit "/std/basic_room";

/* undef this if I ever get banished :) -- Earendil */
#define OUTFIT

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("realm", "village");
    add_property("inside");

    add_exit("up", "/room/vill_road2");

    set_short("Village Station");
    set_long("This is the central station of the village of Nanny.  A "+
	"lot of traffic seems to pass this way, both on foot and by "+
	"other means.  You can probably catch a transport to other "+
	"lands from here.  "
#ifdef OUTFIT
	+"The Adventurers' Outfitters shop is to the east."
#endif
	);

#ifdef OUTFIT
    add_exit("east", "/players/earendil/castles/outfit/outfitters");
    add_item(({"shop", "outfitters"}), "Lanterns, torches, sacks, "+
	 "and other essential items for your adventuring and "+
	"exploring experience can be purchased there.");
#endif
  }
}
