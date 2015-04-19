/* A Beggar for /std/lib.c, made by Bive 950716 */

object make() {

   object beggar;
   beggar=clone_object("/obj/smartmonster");
   beggar->set_name("beggar");
   beggar->set_race("human");
   beggar->set_short("a beggar");
   beggar->set_long("It's a filthy, dirty beggar.\n");
   beggar->set_level(random(2) +2);
   beggar->set_wc(3);
   beggar->set_gender(1);
   beggar->set_wimpyness(20);
   beggar->set_chat_chance(10);
   beggar->set_chat_in_fight(0);
   beggar->add_chat("The beggar snores.\n");
   beggar->add_chat("The beggar says: Please, give me some coins.\n");
   beggar->set_response_object(file_name(this_object()));
   beggar->set_give_money_handler(file_name(this_object()),
				  "handle_give_money");
   beggar->set_soul();
   
   return beggar;
}

void handle_give_money(object who_obj,string who_string, int number_of_coins)
{
  if(!who_obj) return;
  tell_object(who_obj, "The beggar thanks you for the gift.\n");
  tell_room(environment(previous_object()), 
	    "The beggar thanks " +capitalize(who_obj->query_real_name())+ "for the gift.\n",  ({ who_obj }));
}






