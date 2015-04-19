/*************************************************************************
 **                                                                     **
 **  Name    :  creditcard.c                                            **
 **  Author  :  Navigator (RL: Edwin P. Santing)                        **
 **  Mail    :  edwin@duteca.et.tudelft.nl                              **
 **  Date    :  May  7 1991                                             **
 **  Version :  1.12                                                    **
 **  Files   :  creditcard.c, robot_bank.c                              **
 **                                                                     **
 **  ------------------------- Revision List -------------------------  **
 **  Ver   Date       Name         Remarks                              **
 **  0.10  10-02-91   Navigator    First working version                **
 **  1.00   7-03-91   Navigator    Added extra security, save_object    **
 **  1.10  22-03-91   Navigator    Added defines for LOGFILE and such.  **
 **  1.11  20-04-91   Navigator    Added drop-action that gives message.**
 **  1.12   7-05-91   Navigator    Removed id()-bug on some muds.       **
 **                                                                     **
 ************************************************************************/
inherit "std/basic_thing";

#define MAXTYPE    5
#define AUTOLOAD   "/room/bank/creditcard:"
#define VAULT      "/room/bank/vault"

string  colour;
object  owner;
int     cardvalue, cardtype;

reset(arg)
{
  if (arg) return;

  set_value(0);
  set_weight(0);
  set_name("creditcard");
  add_alias("card");
  set_long("@get_long()");
  init_creditcard();
} /* reset */

init()
{
  if (environment(this_object()) == this_player()) {
    add_action("balance", "balance");
    add_action("help_creditcard", "help");
    add_action("drop_it", "drop");
  }
} /* init */

get_long() {
  if (objectp(owner)) {
    return "A " + colour + " creditcard issued to " + owner->query_name() +
      " by Aurora's First Planetory Bank.\n" +
      "The maximum amount that can be stored with this card is " + 
      query_card_maxvalue(cardtype) + " credits.\n";
  } 
  return "A " + colour + " creditcard issued by Aurora's First Planetory Bank.\n";
} /* get_long */


init_creditcard(str) {
  if (intp(str))
    cardtype = str;
  else if (!stringp(str)) cardtype = 0; else sscanf(str, "%d", cardtype);
  if (cardtype < 0)        cardtype = 0;
  if (cardtype >= MAXTYPE) cardtype = MAXTYPE - 1;
  
  colour = query_card_colour(cardtype);
  set_short("A " + colour + " creditcard");
  if (objectp(this_player())) owner = this_player();
  add_alias(colour + " creditcard");
} /* init_creditcard */



help_creditcard(str)
{
  int i, colour, cost, max;

  if (!id(str)) return 0;

  write("This creditcard enables you to store money in a bank. This money will be\n" +
	"available to you, even after you have died. There are " + MAXTYPE + " different creditcards.\n" +
	"They differ in cost and the amount of money you can store with them:\n\n" +
	"                         cost    maximum amount\n");
  for (i = 0; i < MAXTYPE; i++) {
    colour = " " + query_card_colour(i);
    cost   = " " + query_card_cost(i);
    max    = " " + query_card_maxvalue(i);
    write(" " + colour + spacing("creditcard : ", 21 - strlen(colour)) +
	  spacing(cost, 7) + spacing(max, 18) + "\n");
  }
  if (!may_balance()) {
    write("\nMore expensive cards also have extra features.\n");
  } else {
    write("\nThis card has the following extra commands:\n\n");
    write("  balance creditcard : shows the amount of credits on your account.\n");
  }
  write("On the back of the card you notice the text 'made by Navigator'.\n");
  return 1;
} /* help_creditcard */

balance(str)
{
  if (!may_balance() || !id(str)) return 0;
  write("The creditcard contains: " + cardvalue + " credits.\n");
  return 1;
} /* balance */

drop_it(str)
{
  if (id(str)) {
    write("You can't drop your card: go to the Teller to revoke it.\n");
    return 1;
  }
} /* drop_it */


init_arg(arg)
{
  call_out("check_wiz",10);
  if (!arg) 
    return 1;
  else 
    sscanf(arg, "%d#%d", cardvalue, cardtype);
  init_creditcard(cardtype);
} /* init_arg */

check_wiz()
{
  object env;
  env=environment(this_object());
  if (env&&objectp(env)&&env->query_level()>19)
    {
      tell_object(env,"As you are a wizard you no longer have use for a creditcard.\n"+
		  "The creditcard dissolves.\n");
      VAULT->do_revoke(this_object(),env);
    }
}

/* Left pad the integer 'num' to be 'len' characters long */
spacing(num, len)
{
  string str;
  int    i;

  str = " " + num;
  len = len - strlen(str);
  if (len > 0) {
    for (i = 0; i < len; i++) { str = " " + str; }
  }
  return str;
} /* spacing */

wiz()    { return this_player()->query_level() > 19; }
may_balance()        { return (cardtype >  0); }
drop()               { return 1; }
get()                { return 0; }
add_cardvalue(num)   { return set_cardvalue(cardvalue + num); }
set_cardvalue(num)
{
  cardvalue = num;
  if (cardvalue < 0) cardvalue = 0;
  if (cardvalue > query_card_maxvalue(cardtype))
    cardvalue = query_card_maxvalue(cardtype);
  return cardvalue;
}

set_cardtype(num)    { init_creditcard(num); return 1; }
query_cardvalue()    { return cardvalue; }
query_owner()        { return owner; }
query_cardtype()     { return cardtype; }
query_colour()       { return colour; }
query_maxtype()      { return MAXTYPE; }


query_card_cost(num)
{
  switch(num) {
  case 0: return 0;
  case 1: return 200;
  case 2: return 400;
  case 3: return 2000;
  default: return 6000;
  }
} /* query_card_cost */

query_card_maxvalue(num)
{
  switch(num) {
  case 0: return 2000;
  case 1: return 7000;
  case 2: return 12000;
  case 3: return 45000;
  default: return 150000;
  }
} /* query_card_maxvalue */

query_card_colour(num)
{
  switch(num) {
  case 0: return "pink";
  case 1: return "yellow";
  case 2: return "blue";
  case 3: return "black";
  default: return "golden";
  }
}

colour_to_type(str) {
  switch(str) {
  case "pink": return 0;
  case "yellow": return 1;
  case "blue": return 2;
  case "black": return 3;
  case "golden": return 4;
  default: return -1;
  }
} /* colour_to_type */


query_auto_load()
{
  return AUTOLOAD + cardvalue + "#" +cardtype;
}


