/*  obj/quest_mailer.c
 *  This object (a monster!) sends mail to the player,
 *  asking for his (her, its) opinions on a quest he (she, it) has solved.
 *	Padrone, Jan 24 1992
 */

inherit "obj/monster";

reset(arg)
{
  ::reset(arg);
  if (!arg)
  {
    set_name("quest_mailer");
    set_alias("quest_mailer");
    set_long("This is a small demon that works for the quest system.\n" +
	     "Don't touch it!\n");
/* This old, old and obsolete line commented out by Brom 960112.
 * The object has obvioulsy been re-worked, but there is no docs
 * aboute the 
 *  add_action("do_send_the_mail", "send_the_mail");
 *
 */
  }
} /* reset */

solved (player_name, quest_name)
{
  string wizard_name, message;
  string mailtext, other_wiz;
  int quest_no;
  mixed *quests;
  object quest_object;

  quests="/room/quest_room"->query_quest_data();
  quest_no=member_array(quest_name, quests[0]);
  quest_object=quests[3][quest_no];
  if (quest_object == -1) return;

  wizard_name = creator(quest_object);

  if (wizard_name == 0)
    return 0;	/* No creator */

  if (restore_object("players/" + wizard_name) == 0)
    return 0;	/* That wizard doesn't exist */

  if(other_wiz=(quest_object->query_other_wiz())) wizard_name=other_wiz;

  set_name(wizard_name);

  is_npc = 1;	/* Destroyed by the restore_object? */

  message =
    (
     "Hello, " + capitalize(player_name) + "! You have just solved the quest\n" +
     "\"" + quests[2][quest_no] + "\",\n" +
     "and we are now very interested in your opinions:\n" +
     "    Did you like this quest, and why (or why not?)\n" +
     "    Did you find any bugs or other problems?\n" +
     "    Do you have any suggestions for improvement?\n" +
     "    Was the number of quest points adequate?\n" +
     "Please tell us what you think!\n\n" +
     "This message was sent to you auto-magically by a small demon somewhere\n" +
     "deep inside the game, in the name of " + capitalize(wizard_name) +
     ". If you reply to this message\n" +
     "with the 'r' command, your reply will be sent directly to " +
     capitalize(wizard_name) + ".\n");

  if (mailtext=(quest_object->query_mailtext()))
  {
    string a,b;
    message=mailtext; /* If alt. text, set it */

    /* Ugly fix !  /Profezzorn */
    if(sscanf(message,"%s"+quest_object->short()+"%s",a,b))
      message=a+quests[2][quest_no]+b;
  }
  
  log_file("QUEST_MAILER", ctime(time()) + ": from " + wizard_name +
	   " to " + player_name + " about '" + quest_name + "'\n");

  call_other("obj/daemon/mail_reader", "quest_send",
	     ({ player_name, wizard_name, 
		  "Subj: The quest '" + quest_name + "'\n", message, "" }));

  /* Fixed by Celeborn Feb 94 */

  set_name("quest_mailer");
 
} /* solved */
