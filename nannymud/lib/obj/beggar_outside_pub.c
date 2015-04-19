inherit "obj/smartmonster";

int giving_time,i;
object *eq;

void reset(int arg) {
  ::reset(arg);
  if(arg) {
    eq=all_inventory();
    for (i=0; i<sizeof(eq); i++) 
      transfer(eq[i],"room/void");
    return;
  }
  giving_time = time() - random(3600); /* Not desperate when created. */
  set_name("beggar");
  set_male();
  set_short("A filthy beggar");
  set_long("A really filthy looking poor beggar.\n");
  set_level(3);
  set_al(200);
  set_ac(1);
  set_wc(5);
  set_chat_chance(35);
  set_chats(({"Beggar says: Please, give some money to a poor beggar!\n",
	      "Beggar says: Why can't I find any money?\n",
	      "Beggar says: two coins please!\n"}));
  set_attacks(({"Beggar cries","Beggar says: Why do you do this to me?\n",0,35}));
  set_give_money_handler(this_object(), "handle_give_money"); 
  this_object()->add_hook("kill_hook", this_object());
} /* reset */

handle_give_money(who_obj, who_name, nr_coins) {
  say("Beggar smiles happily and says: Thank you, "+who_name+".\n");
  giving_time = time();
  drink_beer();
}

drink_beer()
{
  if (!objectp(environment())) return;
  if (query_money()>=12)
  {
    if (file_name(environment()) == "room/yard")
      init_command("east");
    if (file_name(environment()) == "room/pub2")
    {
      init_command("buy beer");
      init_command("drink beer");
      remove_call_out("init_command");
      call_out("init_command", 2, "west");
    }
    return 1;
  }
}

init()
{
  :: init();
  if (beggar_attack(this_player()))
  {
    say("In desperation, the beggar attacks " +
	this_player() -> query_name() +
	"!\n");
    write("In desperation, the beggar attacks you!\n");
    attack_object(this_player());
  }
}

beggar_attack(o) {
  return (!random(10) &&
	  objectp(o) && interactive(o) &&
	  !drink_beer() &&
	  o -> query_level() < 20 &&
	  (time() - giving_time) > 3600 &&
	  o -> query_hp() < o -> query_max_hp()/20);
}

kill_hook(arr, killer) {
  object corpse;
  if (killer != this_object() ||
      !objectp(corpse = arr[2])) return;
  if (money = present("money", corpse) &&
      money -> is_money())
  {
    transfer(money, this_object());
    tell_room(environment(this_object()),
	      "As the corpse hits the ground, the beggar snatches something from it!\n",
	      ({ this_object() }) );
  }
  remove_call_out("drink_beer");
  remove_call_out("drink_beer"); /* ... and again for safety! */
  call_out("drink_beer", 2);
}



