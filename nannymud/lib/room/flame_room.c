/* /room/flame_room.c - the automatic flame room for broken egos
 *  Latest change of this file: Aug 27, 1993, by Padrone
 *
 */

#include "/room/room.h"

inherit "players/padrone/inherit/smartwrite";

object board;

void reset(int arg) {
    if (!arg) {
	add_exit("room/adv_guild", "up", 0, 0);
        property="no_fight";
	short_desc = "Flame and Ego Room";
	long_desc =
	    "This is the Flame and Ego Room,\n" +
	    "a cold and ugly room in the cellar of the Adventurers' Guild.\n" +
            "You can go to this room when you feel the need coming over you\n" +
	    "to engage in heated arguments with your enemies.\n" +
	    "When you have exhausted your vocabulary of insults,\n" +
	    "you can use the 'flame' command to let the room generate one for you,\n" +
	    "and if you think it's too much work to type 'flame' again and again,\n" +
	    "you can use the command 'autoflame'.\n" +
	    "You can stop autoflaming with 'stop flaming'.\n" +
	    "Finally, it's not very satisfying to scream and shout at someone\n" +
	    "who isn't here, so if the person you want to have a discussion with\n" +
	    "isn't here, you can use the 'create' command to create a suitably\n" +
	    "repentant copy of him or her.\n";
	set_light(1);
	board = clone_object("obj/newboard");
	board->set_name("flameboard","The Flame and Ego board",
			"Well.. The name should say it all.\n",
			"Padrone",769000000,2);
	move_object(board, this_object());
    }
} /* reset */

void init() {
    ::init();
    add_action("cmd_flame", "flame");
    add_action("cmd_flame", "insult");
    add_action("cmd_autoflame", "autoflame");
    add_action("cmd_autoflame", "auto-flame");
    add_action("cmd_unflame", "unflame");
    add_action("cmd_unflame", "un-flame");
    add_action("cmd_stop", "stop");
    add_action("cmd_create", "create");
    add_action("cmd_flamestatus", "flamestatus");
} /* init */

/*---------------------------------------------------------------------------*/

string random_flame(string name, status is_wiz, status is_player) {

    /* Special insults for wizards */
    if (is_player && is_wiz && random(2))
	return ({
	    name + ", did you let Harry code your castle?\n",
	    "Others have bugs in their code. " + name + " has code among the bugs.\n",
	    name + " programs in COBOL!\n",
	}) [ random(3) ];

    /* Special insults for mortal players */
    if (is_player && !is_wiz && random(2))
	return ({
	    "If " + name + " ever becomes a wizard, I'll start playing nethack instead.\n",
	    "Why don't you go pick on someone of your own size, " + name + "? Like Jennifer.\n",
	}) [ random(2) ];

    return ({
	name + "'s mother wears army boots!\n",
	"I'm right and " + name + " is wrong.\n",
	name + ", you childish moron!\n",
	name + ", you immature idiot!\n",
	"Get a haircut and a job, " + name + "!\n",
	"Do us all a service, " + name + ", and play Nintendo instead!\n",
	name + ", go back to where you came from - the orc valley!\n",
        "Do you know the difference between " + name + " and an orc? No? Well, neither do I!\n",
        "Go stick your head in a pig, " + name + ".\n",
        name + ", you're almost as stupid as... as... yourself!\n",
    }) [ random(10) ];
} /* random_flame */

string random_say(string possessive) {
    if (random(2))
	return "says:";
    else if (random(3))
	return ({ "screams", "whines", "growls", "shouts", "complains",
		  "angrily exclaims", "roars", "sputters",
		  "jumps up and down and screams", "shrieks", "mutters" })
	       [ random(11) ] + ":";
    else
	return "stomps " + possessive + " foot and shouts:";
} /* random_say */

void do_flame(object flamer, mixed flamee) {
    string flamer_name, flamee_name;
    int i, n;

    if (!flamer || !flamee) {
	/* Nothing */
    }
    else if (pointerp(flamee)) {
	n = sizeof(flamee);
	for (i = 0; i < n; ++i)
	    do_flame(flamer, flamee[i]);
    }
    else {
	flamer_name = (string)flamer->query_name();
	flamee_name = (string)flamee->query_name();
	tell_room(this_object(),
		  flamer_name + " " +
		  random_say((string)flamer->query_possessive()) + " " +
		  random_flame((string)flamee->query_name(),
			       (flamee->query_level() >= 20),
			       !flamee->query_npc()));
    }
} /* do_flame */

/*---------------------------------------------------------------------------*/

object *all_players_here(object exception) {
    object obj, *result;
    status was_exception;

    result = ({ });
    obj = first_inventory(this_object());
    was_exception = 0;
    while (obj) {
	if (obj == exception)
	    was_exception = 1;
	else if (living(obj))
	    result += ({ obj });
	obj = next_inventory(obj);
    }
    if (sizeof(result) == 0) {
	if (was_exception)
	    write("There is noone else in the flame room.\n");
	else
	    write("The flame room is empty.\n");
	return 0;
    }
    return result;
} /* all_players_here */

object random_player_here(object exception) {
    object *aph;

    aph = all_players_here(exception);
    if (!aph)
	return 0;
    else if (sizeof(aph) == 1) {
	write("Not too many people in here to choose randomly from...\n");
	return aph[0];
    }
    else
	return aph[random(sizeof(aph))];
} /* random_player_here */

/*---------------------------------------------------------------------------*/

int cmd_flame(string str) {
    string lstr;
    object who, *aph;

    if (!str) {
	notify_fail("Of course you want to do that, that's what this room is for!\n" +
		    "But WHO do you want to flame?\n");
	return 0;
    }
    lstr = lower_case(str);
    if (lstr == "all") {
	aph = all_players_here(this_player());
	if (aph && sizeof(aph) > 0) {
	    write("A good choice! You start to flame everyone who is here!\n");
	    do_flame(this_player(), all_players_here(this_player()));
	}
	return 1;
    }
    if (lstr == "random") {
	who = random_player_here(this_player());
	if (who) {
	    do_flame(this_player(), who);
	}
	return 1;
    }
    who = present(lstr, this_object());
    if (!who) {
	write("Are you blind or something? There is no " + str + " here!\n");
	return 1;
    }
    if (who == this_player()) {
	write("Flame yourself? Yes, you definitely deserve it!\n");
    }
    do_flame(this_player(), who);
    return 1;
} /* cmd_flame */

/*---------------------------------------------------------------------------*/

object *flamers, *flamees;

heart_beat() {
    int i, n;
    status was_flame;

    if (!pointerp(flamers)) {
	set_heart_beat(0);
	return;
    }
    was_flame = 0;
    n = sizeof(flamers);
    for (i = 0; i < n; ++i)
	if (   flamers[i] == 0 || flamees[i] == 0
	    || !present(flamers[i], this_object()) || !present(flamees[i], this_object())) {
	    flamers[i] = 0;
	    flamees[i] = 0;
	}
	else {
	    do_flame(flamers[i], flamees[i]);
	    was_flame = 1;
	}

    if (!was_flame) {
	flamers = 0;
	flamees = 0;
	set_heart_beat(0);
    }
} /* heart_beat */

void add_autoflame(object flamer, object flamee) {
    int i, n;

    if (flamers == 0)
	flamers = ({ });
    if (flamees == 0)
	flamees = ({ });

    n = sizeof(flamers);
    for (i = 0; i < n; ++i)
	if (flamer == flamers[i] && flamee == flamees[i]) {
	    write("You are already auto-flaming " + flamee->query_name() + ".\n");
	    return;
	}

    write("INFO: You start to auto-flame " + flamee->query_name() + ".\n");
    flamers += ({ flamer });
    flamees += ({ flamee });

    set_heart_beat(1);
} /* add_autoflame */

void remove_autoflame(object flamer, mixed flamee) {
    int i, n;

    if (!pointerp(flamers))
	return;
    n = sizeof(flamers);
    for (i = 0; i < n; ++i)
	if (flamer == flamers[i] && (flamee == 0 || flamee == flamees[i])) {
	    if (flamees[i])
		write("INFO: You stop auto-flaming " + flamees[i]->query_name() + ".\n");
	    flamers[i] = 0;
	    flamees[i] = 0;
	    if (flamee)
		return;
	}

    if (flamee)
	write("You are not auto-flaming " + flamee->query_name() + ".\n");
} /* remove_autoflame */

int cmd_autoflame(string str) {
    string lstr;
    object who, *aph;
    int i, n;

    if (!str) {
	notify_fail("Yes, but WHO do you want to auto-flame?\n");
	return 0;
    }
    lstr = lower_case(str);
    if (lstr == "all") {
	aph = all_players_here(this_player());
	if (aph && sizeof(aph) > 0) {
	    write("A good choice! You start to auto-flame everyone who is here!\n");
	    n = sizeof(aph);
	    for (i = 0; i < n; ++i)
		add_autoflame(this_player(), aph[i]);
	}
	return 1;
    }
    if (lstr == "random") {
	who = random_player_here(this_player());
	if (who) {
	    write("You pick someone at random, and start to auto-flame the poor bastart!\n");
	    add_autoflame(this_player(), who);
	}
	return 1;
    }
    who = present(lstr, this_object());
    if (!who) {
	write("Are you blind or something? There is no " + str + " here!\n");
	return 1;
    }
    if (who == this_player()) {
	write("Auto-flame yourself? Yes, you definitely deserve it!\n");
    }
    add_autoflame(this_player(), who);
    return 1;
} /* cmd_autoflame */

int cmd_stop(string str) {
    string lstr;
    object who, *aph;
    int i, n;

    if (!str) {
	notify_fail("You must type 'stop flaming name'.\n");
	return 0;
    }
    lstr = lower_case(str);

    if (lstr == "flaming" || lstr == "autoflaming" || lstr == "auto-flaming") {
	write("You stop auto-flaming.\n");
	remove_autoflame(this_player(), 0);
	return 1;
    }

    if (   sscanf(lstr, "flaming %s", lstr) != 1
	&& sscanf(lstr, "autoflaming %s", lstr) != 1
	&& sscanf(lstr, "auto-flaming %s", lstr) != 1) {
	notify_fail("You must type 'stop flaming name'.\n");
	return 0;
    }

    if (lstr == "all") {
	write("You stop auto-flaming.\n");
	remove_autoflame(this_player(), 0);
	return 1;
    }
    if (lstr == "random") {
	write("You can't stop auto-flaming a random person.\n");
	return 1;
    }
    who = present(lstr, this_object());
    if (!who) {
	write("Are you blind or something? There is no " +
	      capitalize(lstr) + " here!\n");
	return 1;
    }
    remove_autoflame(this_player(), who);
    return 1;
} /* cmd_autoflame */

/*---------------------------------------------------------------------------*/

int cmd_create(string str) {
    string lstr, cstr;
    object realone, sim;

    if (!str) {
	notify_fail("Yes, yes, but WHO do you want to create?\n");
	return 0;
    }
    lstr = lower_case(str);
    if (lstr == str)
	cstr = capitalize(str);
    else
	cstr = str;

    if (lstr == "all") {
	notify_fail("Create all? Are you nuts or something? You can only create ONE person.\n");
	return 0;
    }
    if (lstr == "random") {
	notify_fail("Create a random person? No, you must specify WHO you want to create.\n");
	return 0;
    }

    realone = present(lstr);
    if (realone) {
	notify_fail(cstr + " is already here.\n");
	return 0;
    }
    if(present("IDI_CLONE 5"))
      return notify_fail("There are too many clones in here already.\n"),0;
    /* We don't want 7411 smartmonsters eating CPU   /Slater  */
    /* to -> too Banshee 941012 */
    
    sim = clone_object("obj/smartmonster");
    sim->set_name(lstr);
    sim->set_alias("IDI_CLONE"); /* a common name... */
    sim->set_short(cstr + " the stupid clone");
    sim->set_long("This is a stupid clone of " + cstr + ".\n");
    sim->set_chat_chance(5);
    sim->set_ep(0);
    sim->set_chats(({ cstr + " looks stupid.\n",
		      cstr + " says: I am so stupid.\n" }));
    sim->add_say_response(lstr, cstr + " says: You are totally right.\n", 10);
    sim->add_say_response(lstr, cstr + " says: Yes, I agree. I am stupid.\n", 10);
    sim->add_say_response(lstr, cstr + " says: I agree to everything.\n", 10);
    sim->add_say_response(lstr, cstr + " says: You are absolutely right.\n", 10);
    sim->add_response("kicks you", cstr + " says: Yes, kick me! I deserve it!\n");
    sim->set_dead_ob(this_object()); /*Added by Qqqq, since people were creating corpses and using their guildskills on them*/
    say(this_player()->query_name() + " creates a copy of " + capitalize(str) + ".\n");
    write("You created a copy of " + capitalize(str) + ".\n");

    move_object(sim, this_object());

    call_out("kill_clone", 180, sim);

    return 1;
} /* cmd_create */

/*---------------------------------------------------------------------------*/

monster_died() { /*This is mine also.*/
  destruct(present("corpse"));
}

int cmd_flamestatus(string str) {
    int i, n;

    if (pointerp(flamers))
	n = sizeof(flamers);
    else
	n = 0;
    write("Number of active auto-flamers: " + (pointerp(flamers) ? sizeof(flamers) : 0) + "\n");
    for (i = 0; i < n; ++i) {
	write((i + 1) + ": ");
	smartwrite(flamers[i]);
	write(" is flaming ");
	smartwrite(flamees[i]);
	write("\n");
    } /* for */
    return 1;
} /* cmd_flamestatus */

/*---------------------------------------------------------------------------*/

void kill_clone(object sim) {
    if (sim) {
	tell_room(environment(sim),
		  "The stupid clone of " + sim->query_name() + " suddenly falls apart!\n" +
		  "It can't have been of very good quality.\n");
	sim->hit_player(10000);
    }
} /* kill_clone */
