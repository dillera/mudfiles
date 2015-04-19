inherit "room/room";

reset(arg) {
    if (arg) return;

    set_light(1);
    short_desc = "The garden of the Mad Wizard";
    no_castle_flag = 0;
    long_desc = 
	"In the garden of the Mad Wizard.\n" +
	 "In the centre stands a model of the\n"+
	   "well from the village.\n"+
	     "East is the Mad Wizard's castle.\n";
    dest_dir = ({
		 "local/madwizard/castle_1", "east"});
    items = ({"well","It is only a model of the real one, so you can't enter it",
		"garden","It looks like the work of a madman",
		"castle","Looks ancient, even older than insanity itself",
		"model","It is not the real thing, thus impossible to enter"
		});
}

int inorout=2;/* Pell */
