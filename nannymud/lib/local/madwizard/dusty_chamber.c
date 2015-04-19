inherit "room/room";

reset(arg) {
    if (arg) return;

    set_light(1);
    short_desc = "The dusty chamber of the Mad Wizard";
    no_castle_flag = 0;
    long_desc = 
	"In the dusty chamber of the Mad Wizard.\n" +
	  "Nothing in here seems to have been touched\n"+
	  "for ages. Maybe these are the remains of \n"+
	  "deserted or failed experiments.\n";
    dest_dir = ({
		 "local/madwizard/castle_1", "south"});
    items = ({"chamber","It's covered with inches of dust"
		});
}

int inorout=1;/* Pell */
