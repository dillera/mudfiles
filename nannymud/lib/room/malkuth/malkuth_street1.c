#include <std_mudlib.h>
inherit I_ROOM;

reset(arg) {
  if (!arg) {
    set_light(1);
    add_property("outdoors");
    add_property("road");
    add_property("author", "brom");
    add_exit("west", "malkuth_street2");
    add_exit("east", "/room/elm_road/elm_road1");
    add_exit("north","/players/taren/rooms/claim_shop");
    set_short("Narrow street");    
    set_long
      ("Here Malkuth street starts on it's winding path. It is narrow, old and\n"+
       "worn by the passage of many feet. East is Elm Road, and west Malkuth\n"+
       "street continues. The buildings along the street seem to be inhabited\n"+
       "but are in poor shape. A sign hangs from a pole, swinging on a rusty\n"+
       "chain. To the north is a small hut belonging to a craftsman.\n");
    add_item("hut","The hut contains a Claim Mark maker.");
    add_item( ({ "street", "malkuth street" }),
	     "It is old and worn. It could need the attention of a cobbler.\n"+
	     "No one knows from where the name \"Malkuth\", came, or who it\n"+
	     "might have been.\n");
    add_item( ({ "road", "elm road" }),
	     "Elm road is west of here. It goes north and south through\n"+
	     "Lars Town. It has been named for the elms growing along it.\n");
    add_item( ({ "building", "buildings", "house", "houses" }),
	     "The people living at Malkuth street obviously care less about\n"+
	     "their houses outward appearance than saving money. The houses\n"+
	     "are in need of some repair, and there is garbage in the gutters\n");
    add_item("sign",
	     "It is an old, yellow sign hanging on a chain from the pole.\n");
    add_item("pole",
	     "It is old and gray, with some remains of yellow paint. A chain\n"+
	     "holds a sign to the pole.\n");
    add_item("chain",
	     "It is a sturdy but rusty chain. At one end it holds on to the pole,\n"+
	     "and at the other it holds a sign.\n");

  }
} /* reset */

init() {
  :: init();
  add_action("cmd_read", "read");
} /* init */

cmd_read(str) {
  notify_fail("What do you wish to read, did you say?\n");
  if (!str || sscanf(str, "%*ssign%*s")!=2) return 0;

  write(" ___________________________________________\n"+
	"/                   __                      \\\n"+
	"|                  (#()                     |#\n"+
	"|                                           |#\n"+
	"|    __  __       _ _          _   _        |#\n"+
	"|   |  \\/  | __ _| | | ___   _| |_| |__     |#\n"+
	"|   | |\\/| |/ _` | | |/ / | | | __| '_ \\    |#\n"+
	"|   | |  | | (_| | |   <| |_| | |_| | | |   |#\n"+
	"|   |_| _|_|\\__,_|_|_|\\_\\\\__,_|\\__|_| |_|   |#\n"+
	"|      / ___|| |_ _ __ ___  ___| |_         |#\n"+
	"|      \\___ \\| __| '__/ _ \\/ _ \\ __|        |#\n"+
	"|       ___) | |_| | |  __/  __/ |_         |#\n"+
	"|      |____/ \\__|_|  \\___|\\___|\\__|\        |#\n"+
	"\\___________________________________________/#\n"+
	"  ############################################\n");
  return 1;
}

