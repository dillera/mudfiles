inherit "obj/smartmonster";
int last_follow;

void reset(int arg) {
  ::reset(arg);
  /* Better check every reset if the driver has lost our call_out */
  if (find_call_out("move_around")==-1) {
    if (!_isclone(this_object())) return;
    call_out("move_around",10);
  }
  if(arg) return;
  set_name("harry");
  add_alias("fjant");
  set_male();
  set_short("Harry the affectionate");
  set_long("Harry has an agreeable look.\n");
  set_level(3);
  set_al(50);
  set_ep(2083);	/* Not needed - it's set by set_level */
  set_ac(2);
  set_wc(5);
  set_aggressivity(0);	/* Yes, not "set_aggressive". */
  /* Not needed - 0 is the default! */
  set_soul();
  set_random_pick(20);
  last_follow=time();

  set_chat_chance(2);
  set_chats(({
    "Harry says: What are you waiting for?\n",
    "Harry says: Hello there!\n",
    "Harry says: I don't like winter.\n",
    "Harry says: I don't like snow.\n",
    "Harry says: I don't like rain.\n",
    "Harry says: Who are you?\n",
    "Harry says: Why do you look like that?\n",
    "Harry says: What are you doing here?\n" }));
  
  /* Just to show how "add_chat" is used: */
  add_chat("Harry says: Nice weather, isn't it?\n");
  add_chat("Harry smiles.\n");

  /* We use attacks with damage 0 to get the old "attack-chats": */
  set_attacks(({
    "Harry says: Don't hit me!\n", 0, 0, 5,
    "Harry says: That hurt!\n", 0, 0, 5,
    "Harry says: Help, someone!\n", 0, 0, 5,
    "Harry says: Why can't you go bullying elsewhere?\n", 0, 0, 5,
    "Harry says: Aooooo\n", 0, 0, 5,
    "Harry says: I hate bashers!\n", 0, 0, 5 }));

  /* Just to show how "add_attack" is used: */
  add_attack("Harry says: Bastard\n", 0, 5, 10);
  add_attack("Harry says: You big brute!\n", 0, 5, 10);
  
  /* An attack that actually does some damage: */
  add_attack("Harry punched $OTHER's nose.\n",
	     "Harry punched your nose.\n", 2, 10);
  
  set_say_handler(this_object(), "answer_say");
  set_tell_handler(this_object(), "answer_say");
  set_give_handler(this_object(), "handle_give");
  set_give_money_handler(this_object(), "handle_give_money");
  set_arrive_handler(this_object(), "say_hello");
  set_leave_handler(this_object(), "follow");

  set_response_object(this_object());
  add_response("sells", "*why_did",0,0);
  add_response("attacks", "*why_did",0,0);
  add_response("takes", "*why_did",0,0);
  add_response("drops", "*why_did",0,0);
  add_response("is now level", "*how_does_it_feel",0,0);

  add_response("left the game", "Harry says: Why did $OTHER quit the game ?\n",0,0);
  add_response("falls down laughing",
	       "Harry looks at $OTHER.\n",
	       "Harry looks at you.\n",0);
  add_response("bounces", ({ "!flip", "!smile" }),0,0);
  add_response("sings", "Harry says: You really have a nice voice!\n",0, 50);
  add_response("sings", "Harry tries to sing too.\n",0, 50);
  add_response("sings", "!applaud", 50, 0);
  add_response("prays", "!pray",0,0);
  add_response("smiles happily", "Harry smiles happily.\n",0,0);
  /* This would give the same effect:  add_response("smiles happily", "!smile"); */
}

query_time_elapsed() /* How long since Harry followed someone */
{
  return time()-last_follow;
}

query_next_move_around()
{
  return find_call_out("move_around");
}

move_around()
{
  object harrys_old_env;
  int time_elapsed, i;
  object obj, obj2;
  mixed *dest_dir;
  string str, strpart;

  /* Brom was here 951218. You don't think this is really needed? Think again. */
  if (!objectp(this_object())) return;

  remove_call_out("move_around");
  time_elapsed=time()-last_follow;
  /* If we have followed someone resently then stay here */
  if (time_elapsed<(60*5)) {
    call_out("move_around",60*5);
    return 1;
  }
  harrys_old_env=environment(this_object());
  random_move();
  if (harrys_old_env==environment(this_object())) {
    /* Strange, we did not manage to move...perhaps someone is trying
       to fool me. */
    if (time_elapsed<(60*10)) { /* Well, since we havent waited that long
                                   lets wait a little more. */
      call_out("move_around",random(60*5));
      return 1;
    }
    /* Now we have waited long. */
    /* Is there any player around? */
    obj = first_inventory(environment(this_object()));
    while(obj && !interactive(obj)) {
      obj = next_inventory(obj);
    }
    if (obj && interactive(obj)) {
      /* We have someone interactive in this room lets be careful. */
      if (query_idle(obj)>60*5) {
        /* Aha, he is idle, lets see if we can sneak away. */
        dest_dir=environment(this_object())->query_dest_dir();
        if (dest_dir==0) {
          /* OH, shit "No obvious exits." */
          str=file_name(environment(this_object()));
          /* Lets learn a few tricks for special rooms. */
          if (str=="players/akasha/akilles/lab2") {
	    init_command("pray");
            time_elapsed=time();   /* We will now be in church and
                                      does not need to do anything for
                                      a while. */
	    call_out("move_around",60*30);
	    return 1;
          }
          /* End of special places. */
          /* Bah, maybe we shall let us be beaten after all, but
             check once in a while if the player has gone or got dested
             du to too long idle time. */
	  call_out("move_around",60*10);
	  return 1;
        } /* dest_dir==0 */
        /* There are obvious exits, lets try them one at a time until
           we find one that we are allowed to use. */
	harrys_old_env=environment(this_object());
	i=1;
        while( ((i+1)<(sizeof(dest_dir))) &&
               (harrys_old_env==environment(this_object())))
	{
	  init_command(dest_dir[i]);
          i=i+2;
        }
	if (harrys_old_env==environment(this_object())) {
          /* Darn, we where not allowed to use any of the obvious exits. 
             Maybe we shall let us be beaten after all, but
             check once in a while if the player has gone or got dested
             du to too long idle time. */
	  call_out("move_around",60*10);
	  return 1;
	}
        /* We managed to get out through one of the dest_dir directions. */
        call_out("move_around",60);
        return 1;
      } /* interactiv with long idle-time */
      /* Interactive person here that is not that idle, lets hang around. */
      call_out("move_around",60*3);
      return 1;
    } /* interactive */
    /* No interactive player here. */
    str=file_name(environment(this_object()));
    /* Are we in a central area? */
    sscanf(str, "%s/", strpart);
    if (strpart=="room") {
      /* We are in the room area, lets just move around a lot. */
      call_out("move_around",4);
      return 1;
    }
    /* Nah, we are not in the room area and there are noone here, lets
       teleport away. */
    obj=find_object("room/vill_road1"); /* This is a good place to teleport
                                           to, if it is empty. */
    if (obj) {
      obj2 = first_inventory(obj);
      while (obj2) {
        /* Hm, something there. */
        if (interactive(obj2)) {
          /* Ohoh, never teleport to an interactive. Let try again later. */
	  call_out("move_around",60);
	  return 1;
        }
        /* obj2 was not interactive, lets check for more objects */
        obj2 = next_inventory(obj2);
      }
      /* No interactives in the place we want to teleport to. Lets go */
      move_object(this_object(),obj);
      call_out("move_around",60*10);
      return 1;
    }
  }
}


init() {
  ::init();
  add_action("jobbig","south");
  add_action("jobbig","north");
  add_action("jobbig","west");
  add_action("jobbig","east");
}
  
jobbig() {
  /* This is a place where things happen lets hang around here a while. */
  last_follow=time();
  if(this_player()->short()) { /* visible object */
    if(random(4) == 1) {
          /* Dont stop walking wizards, Rohan 940328 */
      if (this_player()->query_level()>19) return 0; 
      say("Harry says: No, "+this_player()->query_name()+"! I will not let you leave me!\n");
      return 1;
    } else 
      return 0;
  } else
    return 0;
}

why_did(who, what, how) {
  sscanf(how, "%s.", how);
  if (what == "sells")
    say("Harry says: Why did you sell " + how + "?\n");
  else if (what == "attacks")
    say("Harry says: Why does " + who + " attack " + how + "?\n");
  else if (what == "takes")
    say("Harry says: Why did " + who + " take " + how + " ?\n");
  else if (what == "drops")
    say("Harry says: Why did " + who + " drop " + how + " ?\n");
} /* why_did */
	
how_does_it_feel(who, what, how) {
  int lvl;
  sscanf(how, "%d.", lvl);
  say("Harry says: How does it feel, being of level " + lvl + " ?\n");
}
    
word_in_phrase(word, phrase) {
  string lphrase, junk1, junk2;
  return sscanf(phrase, "%s" + word + "%s", junk1, junk2) == 2;
} /* word_in_phrase */

answer_say(who_obj, who_name, phrase) {
  string lphrase, a, b;
  /* Only answer interactives, to prevent noc-to-npc talk that can lag
     the mud until nothing else happens. */
  if (!interactive(who_obj)) return 1;
  lphrase = lower_case(phrase);
      /* Some support for RL mudclub, Rohan 940328 */
  if  (word_in_phrase("membership",lphrase) ||
       word_in_phrase("mudclub",lphrase)) check_membership(who_obj);
  else
    if (word_in_phrase("hello", lphrase)
      || word_in_phrase("hi", lphrase))
    say("Harry says: Pleased to meet you!\n");
  else if (word_in_phrase("shut up", lphrase))
    say("Harry says: Why do you want me to shut up ?\n");
  else if (   word_in_phrase("stay here", lphrase)
	   || word_in_phrase("not follow", lphrase)
	   || word_in_phrase("get lost", lphrase))
    say("Harry says: Ok then.\n");
  else
    say("Harry says: Why do you say '" + phrase + "'?\n");
} /* answer_say */

/* Function to give membership to mudclub if autoloader is lost, Rohan 940328*/
check_membership(who_obj) {
object membership;
  if ("players/rohan/obj/div/mudlist"->
       is_member(who_obj->query_real_name())==-1) {
    say("Harry says: To join the mudclub you have to be living in Sweden.\n"+
        "Harry says: For more information send a mail to Rohan.\n");
    return 1;
  }
  if (present("mudmembershipcard",who_obj)) return 1;
  say("Harry says: I see that you have lost your membership card "+
      who_obj->query_name()+".\n");
  say("Harry says: I will give you a new membership card.\n");
  say("Harry takes out a small card from his back pocket and gives it to "+
      who_obj->query_name()+".\n");
  membership=clone_object("/players/rohan/obj/div/mudclub");
  move_object(membership,who_obj);
  return 1;
}

follow(who_obj, who_name, where) {
  last_follow=time();
  init_command(where);
}

handle_give(who_obj, who_name, what_obj, what_name) {
  int rand;
  object obj, next_obj;

  if (what_name == "corpse") {
    say("Harry says: HEY, bury your corpses yourself, asshole!\n");
    transfer(what_obj, who_obj);
    say("Harry returned the " + what_name + " to " + who_name + ".\n");
  }
  else if (what_obj->id("bottle")) {
    /* Someone gave Harry a drink - wonder why? :-) */
    rand = random(4);
    if(rand == 0) {
      if(random(10) > 3) {
	say("Harry sighs and says: I guess you're gonna kill me now.\n");
	obj = first_inventory(this_object());
	while(obj) {
	  next_obj = next_inventory(obj);
	  /* changed "harry" to "obj" -- padrone 920429 */
	  transfer(obj, environment(this_object()));
	  say("Harry drops " + obj->short() + ".\n");
	  obj = next_obj;
	}
	init_command("west");
      }
    }
    else if (rand == 1) {
      init_command("drink " + what_name);
    }
    else if (rand == 2) {
      transfer(what_obj, environment(this_object()));
      say("Harry drops the " + what_name + ".\n");
    }
    else if (rand == 3) {
      transfer(what_obj, who_obj);
      say("Harry returned the " + what_name + " to " + who_name + ".\n");
    }
  }
  else {
    say("Harry says: Thank you very much, sir.\n");
  }
} /* handle_give(str) */

handle_give_money(who_obj, who_name, nr_coins) {
  say("Harry says: Thank you! You don't have some more money, do you?\n");
} /* handle_give_money */
    
say_hello(who_obj, who_name, what, how) {
  say( "Harry says: Hi " + who_name + ", nice to see you !\n");
}

/* This doesn't work - the monster is empty when this function is called! */
monster_died() {
  object obj, b;
  int num;
  obj = first_inventory(this_object());
  while(obj) {
    b = next_inventory(obj);
    /* changed "harry" to "obj" -- padrone 920429 */
    if(call_other(obj, "id", "bottle")) {
      destruct(obj);
      num = 1;
    }
    obj = b;
  }
  if(num)
    say("There is a crushing sound of bottles breaking, as the body falls.\n");
} /* monster_died */









