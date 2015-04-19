#define SAVEFILE   "room/bank/save"
#define CREDITCARD "/room/bank/creditcard"
#define BANK       "/room/bank/bank"
#define FINGERD "/obj/daemon/fingerd"

mapping cardmapping;

reset(arg)
{
  if (arg) return;
  cardmapping=([]);
  restore_object(SAVEFILE);
}


/*
 * Generic entry point for all banking commands.
 */

start_checks() {
  if (!this_player() || !interactive(this_player())) return 0;

  check_player(this_player());
  check_card(this_player());
  return 1;
} /* start_checks */

end_checks() {
  this_player()->save_me(1);
} /* end_checks */


/* Check if player is in database but has no card */
check_player(obj)
{
  object card;
  int *carddata;

  if (present("creditcard", obj)) return 1;

  carddata=cardmapping[obj->query_real_name()];
  if (carddata) {
    tell_room(BANK, 
	      "Teller says: Good day " + query_polite(obj->query_gender()) +
	      ". You seem to have lost your creditcard.\n");
    card = clone_object(CREDITCARD);
    move_object(card, obj);      /* Weightless, so transfer not needed.*/

    card->init_creditcard(carddata[0]);
    card->set_cardvalue(carddata[1]);
    tell_object(obj, "Teller gives you a " + card->query_colour() +
		" creditcard.\n");
    say("Teller gives " + capitalize(obj->query_real_name()) + " a " +
	card->query_colour() + " creditcard.\n", obj);
    obj->save_me(1);
    return 1;
  }
} /* check_player */

/* Check if the card is consistent with the database. */
check_card(obj)
{
  object card;
  int *carddata;
  int cardvalue, cardtype;

  card = present("creditcard", obj);
  if (!card) return;

  carddata=cardmapping[obj->query_real_name()];
  if (!pointerp(carddata))
  {
    cardmapping[this_player()->query_real_name()]=
      ({card->query_cardtype(),card->query_cardvalue()});
  }
  cardvalue=carddata[1];
  if (cardvalue != card->query_cardvalue()) {
    if (card->query_cardvalue() > cardvalue)
      cardvalue = card->query_cardvalue();
  }
  cardtype=carddata[0];
  if (cardtype != card->query_cardtype()) {
    if (card->query_cardtype() > cardtype) 
      cardtype = card->query_cardtype();
  }
} /* check_card */


open(str)
{
  object card;
  if (!start_checks()) return 0;

  if (str != "card" && str != "creditcard" && str != "account") {
    write("Teller says: Use 'open card'\n");
    return 1;
  }
  if (present("creditcard", this_player())) {
    write("Teller says: You already have a creditcard. Use 'new colour', to get a\n" +
	  "different coloured one.\n");
    say_some(this_player());
    return 1;
  }
  if (present("monks_board",this_player())) {
    write("Teller says: Monks are supposed to be poor, so there is no need for you\n"+
	  "to have a creditcard.\n");
    return 1;
  }
  card = clone_object(CREDITCARD);
  move_object(card, this_player());  /* Weightless, so transfer not needed.*/
  card->init_creditcard(0);
  write("Teller gives you a " + card->query_colour() + " creditcard.\n");
  say("Teller gives " + capitalize(this_player()->query_real_name()) +
      " a " + card->query_colour() + " creditcard.\n", this_player());
  cardmapping[this_player()->query_real_name()]=({0,0});
  save_object(SAVEFILE);
  end_checks();
  return 1;
} /* open */


revoke(str)
{
  object card;
  int money;
  if (!start_checks()) return 0;
  
  if (str != "card" && str != "creditcard" && str != "account") {
    write("Teller says: Use 'revoke card'\n");
    return 1;
  }
  card = present("creditcard", this_player());
  if (!card) {
    write("Teller says: You don't have a creditcard.\n");
    say_some(this_player());
    return 1;
  }
  money=do_revoke(card,this_player());
  write("Teller says: The creditcard is revoked.\n");
  if (money > 0) {
    write("Teller says: There was some money left on your account.\n");
    write("Teller gives you "+money+" coins.\n");
    this_player()->add_money(money);
    say_some(this_player());
  }
  end_checks();
  return 1;
} /* revoke */

do_revoke(card,player)
{
  mixed carddata;
  int money_on_card, value_of_card;
  
  carddata=cardmapping[this_player()->query_real_name()];
  money_on_card=0;
  value_of_card=0;
  if(carddata)
    {
      money_on_card=carddata[1];
      value_of_card=card->query_card_cost(carddata[0]); 
      _m_delete(cardmapping,this_player()->query_real_name());
    }
  move_object(card, "room/void");
  destruct(card);
  save_object(SAVEFILE);
  return value_of_card+money_on_card;
} /* do_revoke */

balance(str)
{
  int    cardcost, i, cost;
  string colour;
  object card;
  if (!start_checks()) return 0;

  card = present("creditcard", this_player());
  if (!card) {
    write("Teller says: You don't have a creditcard. Use 'open creditcard' to get one.\n");
    say_some(this_player());
    return 1;
  }
  write("Teller says: You have " + cardmapping[this_player()->query_real_name()][1] +
	" coins on your account.\n");
  say_some(this_player());
  end_checks();
  return 1;
} /* balance */

new(str)
{
  int    cardcost, i, cost;
  string colour;
  object card;
  int cardtype,cardvalue, newtype;

  card = present("creditcard", this_player());
  if (!card) {
    write("Teller says: You don't have a creditcard. Use 'open creditcard' to get one.\n");
    say_some(this_player());
    return 1;
  }
  colour= card->query_colour();
  if (str == colour) {
    write("Teller says: You already have a " + str + " creditcard.\n");
    say_some(this_player());
    return 1;
  }
  cardtype=cardmapping[this_player()->query_real_name()][0];
  cardvalue=cardmapping[this_player()->query_real_name()][1];
  newtype=card->colour_to_type(str);
  if (newtype>=0) {
    cost = card->query_card_cost(newtype) - card->query_card_cost(cardtype);
    if (cost > this_player()->query_money()) {
      write("Teller says: You don't have enough money to buy a better creditcard.\n");
      say_some(this_player());
      return 1;
    }
    if (cardvalue > card->query_card_maxvalue(newtype)) {
      write("Teller says: Your account contains too much money for your new card.\n");
      say_some(this_player());
      return 1;
    }
    if (cost > 0) {
      write("Teller accepts " + cost + " coins from you.\n");
      say("Teller accepts some money from " +
	  capitalize(this_player()->query_real_name()) + ".\n", this_player());
    } else {
      write("Teller gives you " + -cost + " coins.\n");
      say("Teller gives " + capitalize(this_player()->query_real_name())
	  + " some money.\n", this_player());
    }
    write("Teller replaces your " + card->query_colour() + 
	  " creditcard with a " + str + " one.\n");
    say("Teller replaces " +
	capitalize(this_player()->query_real_name()) + "'s " +
	card->query_colour() + " creditcard with a " + str + " one.\n", this_player());
    card->init_creditcard(newtype);
    this_player()->add_money(-cost);
    cardmapping[this_player()->query_real_name()]=({newtype,cardvalue});
    save_object(SAVEFILE);    
    return 1;
  }
  write("Teller says: I'm sorry, " + 
	query_polite(this_player()->query_gender()) + 
	". I only know these colours:\n   ");
  for (i = 0; i < card->query_maxtype(); i++) {
    write(card->query_card_colour(i));
    if (i < card->query_maxtype() -1)
      write(", ");
    else
      write("\n");
    say_some(this_player());
  }
  end_checks();
  return 1;
} /* new */


deposit(str)
{
  int    maxvalue, amount;
  object card;
  if (!start_checks()) return 0;

  card = present("creditcard", this_player());
  if (!card) {
    write("Teller says: You don't have a creditcard. Use 'open creditcard' to get one.\n");
    say_some(this_player());
    return 1;
  }
  if (!str) {
    write("Teller says: You want to deposit nothing? Strange.\n");
    say_some(this_player());
    return 1;
  }
  sscanf(str, "%d", amount);
  if (amount <= 0) {
    write("Teller says: You want to deposit nothing? Strange.\n");
    say_some(this_player());
    return 1;
  }
  maxvalue  = card->query_card_maxvalue(cardmapping[this_player()->query_real_name()][0]);
  if (amount > this_player()->query_money()) {
    write("Teller says: You don't have enough money to deposit this amount.\n");
    say_some(this_player());
    return 1;
  }
  if (amount > (maxvalue - cardmapping[this_player()->query_real_name()][1])) {
    write("Teller says: You can't exceed the limit of " + maxvalue +
    " coins on this card.\n");
    say_some(this_player());
    return 1;
  }
  pay_player(this_player(), -amount);
  end_checks();
  return 1;
} /* deposit */


withdraw(str)
{
  int    amount;
  object card;
  if (!start_checks()) return 0;

  card = present("creditcard", this_player());
  if (!card) {
    write("Teller says: You don't have a creditcard. Use 'open creditcard' to get one.\n");
    say_some(this_player());
    return 1;
  }
  if (!str) {
    write("Teller says: You want to withdraw nothing? Strange.\n");
    say_some(this_player());
    return 1;
  }
  sscanf(str, "%d", amount);
  if (amount <= 0) {
    write("Teller says: You want to withdraw nothing? Strange.\n");
    say_some(this_player());
    return 1;
  }
  if (amount > cardmapping[this_player()->query_real_name()][1]) {
    write("Teller says: You don't have that much money on your account.\n");
    say_some(this_player());
    return 1;
  }
  pay_player(this_player(), amount);
  end_checks();
  return 1;
} /* withdraw */


static pay_player(player, num)
{
  object card;

  card = present("creditcard", this_player());
  if (!intp(num) || num == 0) return;
  if (num > 0) {
    write("Teller gives you " + num + " coins.\n");
    say("Teller gives " + capitalize(player->query_real_name()) +
	" some money.\n", player);
  } else {
    if (num > player->query_money()) {
      write("Teller says: You don't have enough coins.\n");
      say_some(player);
      return 1;
    }
    write("Teller accepts " + -num + " coins from you.\n");
    say("Teller accepts some money from " +
	capitalize(player->query_real_name()) + ".\n", player);
  }
  player->add_money(num);
  card->add_cardvalue(-num);
  cardmapping[this_player()->query_real_name()]=
    ({cardmapping[this_player()->query_real_name()][0],
      cardmapping[this_player()->query_real_name()][1]-num});
  save_object(SAVEFILE);
  return 1;
} /* pay_player */

static say_some(obj)
{
  say("Teller says something to " + capitalize(obj->query_real_name()) + ".\n", obj);
}

query_polite(gender)
{
  if (!gender)     return "Creature";
  if (gender == 1) return "Sir";
  if (gender == 2) return "Madam";
  return "Unknown";
}

wiz(obj)
{
  object player;

  if (objectp(obj)) player = obj; else player = this_player();
  if (player->query_level() > 19 || player->query_real_name() == "navigator")
    return 1;
 return 0;
}

show_total()
{
  int    i, total;
  mixed values;

  values=m_values(cardmapping);
  total=0;
  for (i = 0; i < sizeof(values); i++) {
    total = total + values[i][1];
  }
  write("Total amount stored is " + total + " coins.\n");
  return 1;
} /* show_total */

report()
{
  mixed values,types;
  int    i,empty;


  values=m_values(cardmapping);
  write("There are " + sizeof(values) + " members of this bank.\n");

  types=({0,0,0,0,0}); 
  empty=0;
  for (i = 0; i < sizeof(values); i++) {
    types[values[i][0]]++;
    if (values[i][1]==0) empty++;
  }

  write("The types have this distribution:\n");
  for (i = 0; i < sizeof(types); i++)
    write("  " + i + " = " + types[i] + "\n");
  write("There are " + empty + " empty accounts.\n");
  return 1;
} /* report */


remove() {
  string *persons;
  int i;
  persons=m_indices(cardmapping);
  remove_some(({0,persons,0}));
  return 1;
}

remove_some(arr) {
  int start,i,stop,number;
  string name;
  mixed persons;
  start=arr[0];
  persons=arr[1];
  number=arr[2];
  stop=start+100;
  if (stop>sizeof(persons)) stop=sizeof(persons);

  write("Remove doing "+start+" to "+stop+".\n");

  for (i=start; i < stop; i++) {
    name=persons[i];
    if (!FINGERD->finger(name)) {     
      _m_delete(cardmapping,name);
      write(name+" doesn't exist.\n");
      number++;
    }
    else {
      if (FINGERD->get_player_stats(name)[5]>20) {
	_m_delete(cardmapping,name); 
	write(name+" is a wizard.\n");
	number++;
      }
    }
  }
  if (stop==sizeof(persons)) {
    write("Removed "+number+" persons.\n");
    return 1;
  }
  write("Waiting.\n");
  call_out("remove_some",4,({stop,persons,number}));
  return 1;
}


/* How much money does a player have? */

query_money_bank(object who)
{
  mixed arr;
  if(!who||!objectp(who))
    return 0;
  arr=cardmapping[who->query_real_name()];
  if(arr&&pointerp(arr))
    return arr[1];
  return 0;
}
