inherit "room/room";

reset(arg) {
    if (arg) return;

    set_light(1);
    short_desc = "The main hall";
    no_castle_flag = 0;
    long_desc = "The main hall of the castle.\n"+
      "The 5011 portals in here gives this\n"+
      "room many exits, though few usable\n"+
	"to puny mortals.\n";
    dest_dir = ({
		 "local/madwizard/dusty_chamber", "north",
		 "local/madwizard/garden1","west",
		 "local/madwizard/entrance","east"
	       });
    items = ({"portal","It would be hard to make out a single one",
	      "portals","They dazzle your mind, leaving you a little less sane"
		});
}

int inorout=1;/* Pell */
