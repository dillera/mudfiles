inherit "room/room";

#define TEXT "It is about three feet wide, and often overstepped by wizards"

reset(arg) {
    if (arg) return;
    set_light(1);
    short_desc = "Entrance of the Mad Wizard's castle";
    no_castle_flag = 0;
    long_desc = "Entrance of the Mad Wizard's castle.\n"+
      "The red line, known as the border of logical\n"+
      "exits and entrances, crosses the room.\n";
    dest_dir = ({
		 "local/madwizard/castle_1","west",
		 "players/morgar/magic","east"
	       });
    items = ({"line",TEXT,
		"red line",TEXT,
		"border",TEXT
		});
}

int inorout=1;/* Pell */
