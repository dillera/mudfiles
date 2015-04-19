/* leo.c
 *
 * This is the object which tries to behave like Leo the archwizard.
 * It will handle new wizards and their castles...
 *
 * Updated by Profezzorn.
 * Updated by Brom 960107.
 */
static inherit "obj/smartmonster";
#define LB(X) sprintf("%-=78s\n", X)

reset(arg)
{
  if (arg) return;
  :: reset(0);
  set_name("leo");
  set_short("Leo the Archwizard");
  /* His equipment exists solely as part of the long desc.
   */
  set_long(LB("This is Leo, the first Arch Wizard. He is a venerable old man with "+
	      "a long and flowing white beard and piercing blue eyes. He might be "+
	      "old, but the power in those eyes...")+
	   "  Leo is carrying:\n"+
	   "An ornate wooden staff.\n"+
	   "A golden robe.\n"+
	   "A pointed hat.\n"+
	   "A pair of curly slippers.\n");
  set_level(40);
  set_gender(1);
  set_say_handler(this_object());
  set_arrive_handler(this_object()); /* Greet people. */
  set_give_handler(this_object());   /* The orc slayer quest. */

} /* reset */


hit_player(dam, flag)
{
  tell_room(environment(),
	    "Leo the Archwizard is protected by his magic.\n");
}


catch_tell(str)
{
  log_file("LEO", str + "\n");
  if (!this_player() || !interactive(this_player()))
    return;

  :: catch_tell(str);
} /* catch_tell */

handle_say(who, name, str)
{
  object env;
  int tmp;

  name = capitalize(who -> query_real_name());
  env = environment();

  tmp = sizeof(regexp( ({ str }), "[Hh]i|(ello)"));
  if (tmp)
    tell_room(env,
	      "Leo the Archwizard says: Hello, " + name + ".\n");

  tmp = sizeof(regexp( ({ str }), "(want)|(give).*castle"));
  if (tmp)
  {
    string msg;
    switch(who -> query_level())
    {
    case 20:
      msg = LB("Leo the Archwizard says: Sure, " + name + ", but first you must give me "+
	       "a validated wizards form.");
    case 21 .. 50:
      msg = LB("Leo the Archwizard says: What happened to your old castle, "+
	       "the one I gave you last time?\n");
    default:
      msg = "Leo the Archwizard says: But " + name + ", you have no use of a castle?\n";
    }
    tell_room(env, msg);
    return;
  }

  tmp = sizeof(regexp( ({ str }), "(form)|(book)"));
  if (!tmp) return;

  if (who -> query_level() == 20 && tmp)
  {
    object all;
    int i, ok;

    /* Check if we've gotten the form, if so DON'T give another. */
    all = all_inventory(this_object());
    ok = 1;
    for (i=0; i<sizeof(all); i++)
      if (all[i] -> id("form") &&
	  all[i] -> query_wizard() == who -> query_real_name())
	ok = 0;

    if (!present("wizform", who) && ok)
    {
      move_object(clone_object("obj/leo_form"), who);
      tell_room(env,
		LB("Leo the Archwizard says: Here, " + name + ", this form will "+
		   "help you to become a true wizard, in the fullest sense of the word."));
      tell_room(env,
		"Leo the Archwizards gives a form to " + name + ".\n",
		({ who }) );
      tell_room(env,
		"Leo the Archwizard gives you a form.\n");
    }
    if (!present("wizbook", who))
    {
      move_object(clone_object("obj/wizbook"), who);
      tell_room(env,
		LB("Leo the Archwizard says: Here, " + name + ", this book will "+
		   "help you to become a full wizard, in the truest sense of the word."));
      tell_room(env,
		"Leo the Archwizards gives a book to " + name + ".\n",
		({ who }) );
      tell_room(env,
		"Leo the Archwizard gives you a book.\n");
    }
  }
}  

static castle(form,wizard)
{
  object castle;
  string player_name;
  string cap_sponsor;
  object sponsor_obj;

  cap_sponsor = form -> query_sponsor();
  player_name = form -> query_wizard();


  if(player_name != wizard -> query_real_name())
  {
    tell_room(environment(),
	      LB("Leo the Archwizard says: But this isn't your form, "+
		 capitalize(wizard -> query_real_name()) + "!\n"+
		 "Here, take it back."));
    move_object(form,wizard);
    return;
  }
  if(wizard->query_level() != 20)
  {
    tell_room(environment(),
	    LB("Leo the Archwizard says: " + name + ", you are "+
	       "an apprentice wizard!\n"+
	       "Leo the Archwizard rips the form to shreds."));
    destruct(form);
    return;
  }
  player_name = capitalize(player_name);
  
  write_file("/log/SECURE/WIZFORMS",
	     "========================================================================\n"+
	     ctime(time()) + "\n"+
	     (string)form->query_long2());
  destruct(form);

  player_name = wizard->query_real_name();

  write_file("log/SECURE/SPONSOR",
	     cap_sponsor + " : " + capitalize(player_name) +
	     " " + ctime(time()) + "\n");
  write_file("log/SPONSOR",
	     cap_sponsor + " : " + capitalize(player_name) +
	     " " + ctime(time()) + "\n");
  wizard -> set_level(21);
  wizard -> set_title("the new wizard");
  castle = create_wizard(player_name);

  if (castle) castle -> short();
  tell_object(wizard,
	      LB("Congratulations, you are now a complete wizard with your own "+
		 "castle located in the room south of here. When you have "+
		 "built enough to open your area, then mail/talk to one of the "+
		 "arches, before putting your castle into the players world. "+
		 "You will get more wizard commands at next log in."));

}

/* Greet people who enters, but not NPC's.
 */
handle_arrive(who, name, how)
{
  object env;

  if (!(env = environment()) || !who || !interactive(who)) return;
  if (present(who, env))
    tell_room(env,
	      "Leo the Archwizard says: Welcome, " +
	      capitalize(who -> query_real_name()) + ".\n");
  if(this_player()->query_level() == 20)
    write(LB("Leo the Archwizard tells you: You might be interested in a wizards form "+
	     "and/or a wizards guide book. If so, just say so."));
} /* handle_arrive */

/* Take care of the orc slayer quest, and the wizards forms.
 */
handle_give(who, name, thing, desc)
{
  int w;
  object env, form;

  env = environment();
  name = capitalize(who -> query_real_name());

  tell_room(env,
	    "Leo the Archwizard says: Thank you very much.\n");

  if(file_name(thing)[0..11]=="obj/leo_form")
  {
    if(!thing -> query_validator())
    {
      tell_room(env,
		"Leo the Archwizard gives a form to " +	name + ".\n",
		({ who }) );
      tell_object(who,
		  LB("Leo tells you: This form isn't validated, You must have a highwizard "+
		     "or higher validate the form for you."));
      form = present("wizards form");
      if (form)
	move_object(form, who);
    }
    else
    {
      castle(thing,who);
      return;
    }
  }

  if (thing -> id("orc slayer"))
  {
    tell_room(environment(),
	      "Leo the Archwizard says: Well done. You have fullfilled this quest.\n");
    who -> set_quest("orc_slayer");
    destruct(thing);
    return;
  }
  tell_room(environment(),
	    "But what would I do with this? Here, have it back.\n");
  if (transfer(thing, who))
  {
    tell_room(environment(),
	      "Leo the Archwizard hmms. Then, he drops the item.\n");
    w = thing -> query_weight();
    add_weight(-w);
    move_object(thing, environment());
  }
  if (present(thing)) /* Bah! */
    _destruct(thing);
} /* handle_give */
