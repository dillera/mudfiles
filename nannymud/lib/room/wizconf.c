/* A new room for more orderly wizconfs... Pell -91 */

#define TP this_player()
#define TPQRN this_player()->query_real_name()

inherit "room/room";

string banner;

reset(arg)
{
  if (arg) return;
    
/*
 * Replace this banner with something more interesting. I was originally
 * thinking of adding the ability to set the banner through a wizard
 * action, but it's too much hassle with save_object()...
 */

  banner = "Welcome to the new wizard conference room!";

  set_light(1);
  short_desc = "Conference room";
  no_castle_flag = 1;
  items = ({"throne", "foo"});	/* Just for the id()'s sake */
  dest_dir = ({"room/pub2", "pub",
	       "room/adv_guild", "guild",
	       "room/adv_inner", "inner"});
}

long(arg)			/* Override standard long() */
{     
  if (arg == "throne") {
    write("The throne is made of black marble and is large and imposing.\n");
    if (TPQRN == "mats")
      write("As you can feel, it is remarkably comfortable.\n");
    else if (present("mats"))
      write("Mats is slouched back on it, in his usual relaxed posture.\n");
    else
      write("It is made for Mats, super high lord turbo power overgod.\n");
  }
  else
    write(
"This is the Wizard Conference room. The walls are made of black obsidian\n"+
"and the ceiling provides a blue, soft light.\n"+
"The room is furnished with tables made of glass, arranged in a big circle.\n"+
"In the middle of the room a black marble throne is located for Mats,\n"+
"the god of NannyMud.\n    ===> help wizconf <===  describes available commands.\n"+
"A banner streches through the room. It reads:\n*** "+ banner + " ***\n"+
"    There are three very obvious exits: pub, guild and inner.\n");
}

init()
{
  /* Add default action first, so the room actions can override */
  add_action("parse_command", "", 1);
  ::init();			/* Setup room actions */
}

parse_command(args)
{
  string verb;

  verb = query_verb();

  if (verb == "help" && args == "wizconf") {
    if (TPQRN == "mats")
      write(
"Hi, Mats!  You can do anything here of course. Use 'get word'\n"+
"to get the Word. This is what other wizards get as help message:\n\n");

    write(
"You will find lots of normal feelings, emotions, etc, disabled as long\n"+
"as you don't have the Word. Don't try to override this; it is very simple\n"+
"and you should view this as a strong hint that the person with the Word\n"+
"should be able to speak uninterrupted.  These are the special commands:\n\n"+
"word           Tells Mats that you want to speak.\n"+
"get word       Tells Mats that you want to speak.\n"+
"return         Returns the word to Mats.\n"+
"agree          Says '<your name> agrees.'\n"+
"agree <name>   Says '<your name> agrees with <name>.'\n"+
"                 Might be good if the connection is very slow...\n"+
"disagree       See 'agree'. Be careful with its usage!\n"+
"yes, no        Synonyms to 'agree' and 'disagree' respectively.\n");
    return 1;
  }				/* if 'help wizconf' */

  if (TPQRN == "mats") {	/* Parse special Mats-stuff now */
    string who;

    if (verb == "give" && args && sscanf(args, "word to %s", who) == 1) {
      if (present("the word of speech", TP)) {
	if (present(lower_case(who))) {
	  move_object(present("the word of speech", TP),
		      present(lower_case(who)));
	  write("Ok.\n");
	  say(capitalize(who) + " has the Word.\n");
	}
	else
	  write(capitalize(who) + " doesn't seem to be present.\n");
      }
      else
	write("But you don't have the word! Use 'get word' to get it.\n");
      return 1;
    }
	
    if (verb == "get" && args == "word") {
      int got; object i, word;

      got = 0;
      i = first_inventory(this_object());
      while (i) {
	if (i == TP) {		/* This is Mats! */
	  i = next_inventory(i);
	  continue;
	}
	/* No need to do a special check for players-only */
	word = present("the word of speech", i);
	if (word) {
	  if (got)		/* Hmmm... more words in circulation */
	    destruct(word);
	  else {
	    got = 1;
	    move_object(word, TP);
	    say("Mats has the Word.\n");
	  }
	}
	i = next_inventory(i);
      }				/* while */

      if (!got)			/* Hm, noone had it... */
	move_object(clone_object("obj/wizconfword"), TP);
      /* I could have made some protection against cloning by others
	 than Mats, but Mats has the "get word" command which destructs
	 all Words found, save one */

      return 1;
    }
    return 0;			/* Mats can do anything */
  }

  if (verb == "word" || (verb == "get" && args == "word")) {
    write("You ask for the favour to speak.\n");
    say(capitalize(TPQRN) + " would like to speak.\n");
    return 1;
  }

  if (verb == "return") {	/* 'return word' works too of course */
    object mats;
    
    if (present("the word of speech", TP)) {
      mats = present("mats");
      if (mats) {
	move_object(present("the word of speech", TP), mats);
	write("You return the Word to Mats.\n");
	say(capitalize(TPQRN) + " returns the Word to Mats.\n");
      }
      else
	write("Strange, but Mats isn't present. You keep the Word.\n");
    }
    else
      write("But you don't have it!\n");
    return 1;
  }

  if (verb == "agree" || verb == "yes") {
    /* Yes, I know. args can be a sentence. We'll see how folks use this */
    if (args) {
      write("You agree with " + args + ".\n");
      say(capitalize(TPQRN) + " agrees with " + args + ".\n");
    }
    else {
      write("You agree.\n");
      say(capitalize(TPQRN) + " agrees.\n");
    }
    return 1;
  }

  if (verb == "disagree" || verb == "no") {
    /* Yes, I know. args can be a sentence. We'll see how folks use this */
    if (args) {
      write("You disagree with " + args + ".\n");
      say(capitalize(TPQRN) + " disagrees with " + args + ".\n");
    }
    else {
      write("You disagree.\n");
      say(capitalize(TPQRN) + " disagrees.\n");
    }
    return 1;
  }

  if (present("the word of speech", TP)) { /* Check for special actions */

/*
 * If the wizard has the Word, s/he should be able to use normal
 * emotions to augment the speech.
 */
    return 0;
  }

/* Allow these actions */

  if (verb == "goto" || verb == "trans" || verb == "tell" || verb == "home" ||
      verb == "earmuffs" || verb == "vis" || verb == "people" ||
      verb == "wizlist" || verb == "localcmd" || verb == "more" ||
      verb == "who" || verb == "bug" || verb == "typo" || verb == "idea" ||
      verb == "help" || verb == "exa" || verb == "examine" || verb == "look" ||
      verb == "i" || verb == "inventory" || verb == "quit") return 0;

/* If we have come this far, the wizard tries a command we don't like */

  write(
"That action is unavailable here. Use 'get word' if you want to speak\n"+
"or 'help wizconf' for a list of special commands.\n");
  return 1;
}
