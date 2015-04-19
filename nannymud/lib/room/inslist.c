#include <std.h>
#define INSLIST "room/inslist"
string ins_list;
string dead_list;

#undef EXTRA_RESET
#define EXTRA_RESET extra_reset(arg);

#undef EXTRA_INIT
#define EXTRA_INIT extra_init();

extra_reset(arg)
{
  if(!arg) {
    ins_list = "";
    dead_list = "";
    restore_object(INSLIST);
  }
}

extra_init()
{
  add_action("inslist"); add_verb("inslist");
  add_action("deadlist"); add_verb("deadlist");
}


is_on_list(name,list)
{
  string format, foo;

  format = name + "%s";
  if (sscanf(list, format, foo) == 1)
    return 1;
  format = "%s" + name;
  if (sscanf(list, format, foo) == 1)
    return 1;
  return 0;
}

is_insured(name) 
{
  return is_on_list(name,ins_list);
}

add_to_dead(name)
{
  dead_list = dead_list + name + "\n";
  save_object(INSLIST);
}

remove_from_dead(name)
{
  string cdr_list;
  string curr_name;
  int end;

  cdr_list = dead_list;
  dead_list="";
    	
  while (cdr_list!="") {
    end = sscanf(cdr_list,"%s\n%s",curr_name,cdr_list);
    if (end==1) cdr_list="";
    if (curr_name==name)
    { dead_list = dead_list + cdr_list;
      save_object(INSLIST);
      return 1;
    } else
      dead_list = dead_list + curr_name + "\n";
  }
}

pay()
{
  string player_name;

  player_name = call_other(this_player(),"query_name");

  if(player_name=="Someone")	/* player invisible */
  {
    write("Huh? Is there anybody there?\n");
    return 1;
  }

  if(is_on_list(player_name,ins_list))
  {
    write("But you are already insured!\n");
    say(player_name + " tries to get a second insurance, but fails\n");
    return 1;
  }

  if(call_other(this_player(),"query_money",0)<1000)
  {
    write("You don't have 1000 coins.\n");
    say(player_name+" is to poor to buy an insurance.\n");
    return 1;
  }
  write("Congratulations!\n" +
	"You bought a life insurance.\n" +
	"We sincerely hope you'll never have any use of it,\n" +
	"but if you would get yourself killed you have to come\n" +
	"to us in the ghost form and use the command 'spook'\n" +
	"to receive the compensation.\n");
  say(player_name + " bought an insurance. Wise guy!\n");
  call_other(this_player(),"add_money",-1000);
  call_other("players/dz/inn","rumour",
	     "The bartender says: It was very wise of "+this_player()->query_name()+"\n"+
	     "to buy a life insurance.\n");

  ins_list = ins_list + player_name + "\n";
  save_object(INSLIST);
  this_player()->save_me(1);
  log_file("insurance.log",player_name + "\n");
  return 1;
}

spook()
{
  string player_name;
  string foo;

  player_name = capitalize(this_player()->query_real_name());
  if (!call_other(this_player(),"query_ghost"))
  { 
    write("Sorry, as far as I can tell you are alive.\n");
    return 1; 
  }
  if(!this_player()->query_real_ghost())
  {
    write("Hey, we don't like faked ghost on our company. If you\n" +
	  "try this again we will withdraw your insurance.\n");
    return 1;
  }
  if (!is_on_list(player_name,ins_list))
  {
    write("We can't help you: you don't have an insurance here.\n");
    return 1;
  }
  write("Sorry about your accident...\n"+
	"If you return to us in a more solid form you can receive\n" +
	"your recompense. 'Receive money' is all you have to do\n" +
	"Have a nice day and a nice second life!\n");
  add_to_dead(player_name);
  save_object(INSLIST);
  return 1;
}

ONE_EXIT("players/dz/insurance","out",
         "Insurance files",
         "This is where we store lists of insured people",
         1)

recmoney()
{
  string player_name;
  int amount;

  if (this_player()->query_ghost()) return 1;
  player_name = call_other(this_player(),"query_name");
  if (!is_on_list(player_name,dead_list))
  { write("I'm sorry, but you are not on our list of accidents.\n");
    return 1; }
  amount = call_other(this_player(),"query_exp")/12;
  if (amount > 100000)
    amount = 100000;		/* Padrone, May 29, 1993 */
  call_other(this_player(),"add_money",amount);
  write("You receive "+amount+" gold coins.\n" +
	"Remember that your insurance is still valid!\n");
  say(player_name + " receives some money.\n");
  remove_from_dead(player_name);
  save_object(INSLIST);
  log_file("insurance.log","Paid "+amount+" to "+player_name+"\n");
  return 1;
}

inslist() 
{
  write(ins_list + "\n");
  return 1;
}

deadlist()
{
  write(dead_list + "\n");
  return 1;
}










