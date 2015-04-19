/* Earendil -- July 95 */
/*inherit "/std/basic_room"; Temporary */
inherit "/room/extra/time_room";
#define LB(X) sprintf("%-78=s\n", X)

/* Ingis is here to stay.  Why don't we put it in the room files? */
#define INGIS

void reset(int arg) {
  if (arg) return;

  time_item_setup();
  set_light(1);
  add_property("outdoors");
  set_short("Village green");
  set_long(
    "You are in a large open grassy place just south of the village church, which"+
    " dominates the scene. The main village lies over to the east; to the west is a "+
    "hump-backed bridge crossing a wide river, and beyond that untamed wilderness.  "
#ifdef INGIS
    +"Ingis Lane starts here and leads off south."
#endif
   );

  add_item("church", LB("The church towers above you, dominating this "+
	"peaceful scene. It is a haven against the dangers of the "+
	"wild lands of Nanny, and stands vigil here on the borders "+
	"of those lands.\nThe church is a wooden structure, with "+
	"a tall steeple reaching toward the sky. You may enter it "+
	"to the north."));
    add_item("steeple", "The steeple is visible for miles around, "+
	"and serves to guide lost adventurers back to the town "+
	"proper. You note a small window in the side of it, that "+
	"must look out from the attic.");
#ifdef INGIS
    add_item(({"lane", "ingis lane"}), "Ingis Lane leads through "+
	"a residential part of the village (you spy several houses) "+
	"and on out into the wilderness and eventually the sea.");
    add_item(({"house","houses"}), "Most notable is a large two-story "+
	"house directly south and east. Probably somebody important "+
	"lives there.");
#endif
    add_item("grass", "The green is made up of healthy, green grass.\n");
    add_item("bridge", "The hump-backed bridge crosses over the "+
	"cold wide river running down from the mountains.  Crossing "+
	"over it will take you west into the wild and untamed "+
	"western lands, where you may find adventure, treasure, "+
	"and perhaps fame, or death.");
    add_item("village", "The main part of the village lies east.  "+
	"There you may find the shop, the Adventurers' guild, and "+
	"many other business of various and sundry natures.");

#ifdef INGIS
    add_exit("south", "/players/ingis/village/lane");
#endif
    add_exit("east", "/room/vill_track");
    add_exit("north", "/room/church");
    add_exit("west", "/room/hump");
}
