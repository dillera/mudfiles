/*************************************************************************
 **                                                                     **
 **  Name    :  robot_bank.c                                            **
 **  Author  :  Navigator (RL: Edwin P. Santing)                        **
 **  Mail    :  edwin@duteca.et.tudelft.nl                              **
 **  Date    :  June  7 1991                                            **
 **  Version :  1.30                                                    **
 **  Files   :  robot_bank.c, creditcard.c                              **
 **                                                                     **
 **  ------------------------- Revision List -------------------------  **
 **  Ver   Date       Name         Remarks                              **
 **  0.10  18-02-91   Navigator    First working version                **
 **  1.00  15-03-91   Navigator    Added extra security, save_object    **
 **  1.01  21-03-91   Navigator    Added defines for LOGFILE and such.  **
 **  1.02  22-03-91   Navigator    Removed no-creditcard bug.           **
 **  1.10  28-03-91   Navigator    Changed returning of money & MONEY   **
 **  1.11  10-04-91   Navigator    Added 'balance!' for total stored.   **
 **  1.21  15-04-91   Navigator    Added cpp-directives.                **
 **  1.30   7-06-91   Navigator    Remove unused players now.           **
 **        14-12-95   Anna         Split into two files.                **
 **                                                                     **
 ************************************************************************/
inherit "obj/smartmonster";

reset(arg)
{
  ::reset(arg);
  if (arg) return;


  set_name("teller");
  set_alias("clerk");
  set_alt_name("banking clerk");
  set_short(cap_name + " the banking clerk (punctual)");
  set_long("A banking clerk chained to a desk.\n");

  set_aggressive(0);
  set_level(10); 
  set_al(700);
  set_hp(510); 
  set_ac(24); 
  set_wc(13);
  add_money(random(100)+1);

  set_soul();
  
  set_chat_chance(5);

  add_chat("Teller says: Can I help you?\n");
  add_chat("Teller rattles his chains.\n");


  add_attack("Teller says: Would you please remove yourself from this area.\n",0,0,20);
  add_attack("Teller says: 1500 years of quiet work and now this.\n",0,0,20);
  add_attack("Teller says: If only the guards would still function.\n",0,0,20);


  add_response("kicks you","!kick $LOWOTHER");
  add_response("smiles","!smile $LOWOTHER");
  add_response("bows to you",
	       "The teller tries to bow but fails since he is chained to his desk.\n");
  add_response("kisses you","!blush $LOWOTHER");

  set_say_handler(this_object(), "handle_say");

} /* reset */

msg(mixed a)
{
  if(a[0])
  {
    tell_object(a[0],a[1]);
    tell_room(environment(),a[2],({a[0]}));
  }else{
    tell_room(environment(),a[2]);
  }
} /* msg */

msg2(mixed a)
{
  tell_room(environment(),a);
} /* msg2 */


_signal_gotten(object obj)
{
  string who, what, where, rest;
  object owner;

  if (attacker_ob && present(attacker_ob, environment(this_object()))) return;

  owner=this_player();
  what=obj->short();
  who=owner->query_name();
  if(sscanf(file_name(obj),"obj/money#%*d") && owner)
  {
    int val;
    val=obj->query_value();
    if (val > 0)
    {
      call_out("msg",0,({owner,
			   cap_name+" says: Use 'deposit <amount>', " + query_polite(owner->query_gender()) +
			     ", to deposit money.\n"+
		       cap_name + " gives you " + what +".\n",
		       cap_name + " gives " + capitalize(owner->query_real_name()) +
			 " some money.\n" }) );
      add_money(-val);
      owner->add_money(val);
      return;
    }
    return;
  }
  

  if (!transfer(obj, owner))
  {
      call_out("msg2",2, cap_name + 
	       " says: I'm sorry " + query_polite(owner->query_gender()) +
	       ", but I have no need for " + what + ".\n"+
	       cap_name + " gives " +
	       what + " to " + who + ".\n");
  } else {
    transfer(obj, environment(this_object()));
      call_out("msg2",2, cap_name + 
	       " says: I'm sorry " + query_polite(owner->query_gender()) +
	       ", but I have no need for " + what + ".\n"+
	       cap_name + " drops " + what + ".\n");
  }
} /* _signal_gotten */

handle_say(who_object,who_string,phrase_string) {
  string a,b,c,nam;

  if (attacker_ob && present(attacker_ob, environment(this_object()))) return;
  if (!interactive(who_object)) return;

  a=lower_case(phrase_string);
  if (sscanf(a,"%syes%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: For help do 'say teller help'.\n");
      say_some(who_object);
      return 1; 
  }
  if (sscanf(a,"%shelp card%s",b,c)==2 || sscanf(a,"%shelp creditcard%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: for help on your creditcard, type 'help creditcard'.\n");
      say_some(who_object);
      return 1; 
  }
  if (sscanf(a,"%shelp open%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: To get a free creditcard of your own, type 'open creditcard'.\n");
      say_some(who_object);
      return 1; 
  }
  if (sscanf(a,"%shelp revoke%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: To revoke your creditcard (rarely useful), type 'revoke creditcard'.\n");
      say_some(who_object);
      return 1; 
  }
  if (sscanf(a,"%shelp balance%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: When you type 'balance creditcard', I will give you the amount of\n" +
		  "coins stored in your account.\n");
      say_some(who_object);
      return 1; 
  }
  if (sscanf(a,"%shelp new%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: To get a creditcard that can hold a larger or lesser amount type\n" +
		  "'new <colour>'. The available colours are given by your creditcard by typing\n" +
		  "'help creditcard'. No additional costs are charged for changing your card.\n");
      say_some(who_object);
      return 1; 
  }
  if (sscanf(a,"%shelp deposit%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: Type 'deposit <amount>' to store money in your account. However,\n" +
		  "you cannot exceed your card-limit as shown by 'help creditcard'.\n");
      say_some(who_object);
      return 1; 
  }
  if (sscanf(a,"%shelp withdraw%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: Type 'withdraw <amount>' to retrieve money from your account.\n");
      say_some(who_object);
      return 1; 
  }
  if (sscanf(a,"%shelp%s",b,c)==2) {
      tell_object(who_object,
                  "Teller says: You can give the following commands:\n\n" +
		  "  open     creditcard\n" +
		  "  revoke   creditcard\n" +
		  "  balance  creditcard\n" +
		  "  new      <colour>\n" +
		  "  deposit  <amount>\n" +
		  "  withdraw <amount>\n\n" +
		  "For privacy reasons you should not 'say' those commands, but type them. For\n" +
		  "help on those commands just do 'say " + cap_name + " help <command>'.\n");
      say_some(who_object);
      return 1; 
  }
} /* handle_say */


static say_some(obj)
{
  say("Teller says something to " + capitalize(obj->query_real_name()) + ".\n", obj);
} /* say_some */


query_polite(gender)
{
  if (!gender)     return "Creature";
  if (gender == 1) return "Sir";
  if (gender == 2) return "Madam";
  return "Unknown";
} /* query_polite */

