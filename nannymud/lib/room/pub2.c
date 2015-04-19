/* +---
 * | 
 * | NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! 
 * | READ ALL THE TEXT IN THIS COMMENT!   READ ALL THE TEXT IN THIS COMMENT!
 * | 
 * | pub2.c
 * | 
 * | Why is it called pub2, you might wonder? Well, the first pub was in Lars
 * | area, which Nanny never had. But file names last.
 * | This is the local pub! Here, drinks are sold - and consumed.
 * | Since this file is often used by newbie wizards as a pattern
 * | for their own, this comment was written:
 * | 
 * | NOTE: This pub does not conform strictly to the rules. If you
 * |       create a pub of your own, THAT one must.
 * | This is so players will be encouraged to go to this pub, visit the
 * | village and socialize. The village is a low-gain area, and without
 * | any advantages nobody will go here. The admin have deemed the village
 * | central to the game, and want people around here. Thus this pub.
 * | 
 * | History:
 * | Date    By whom What was done
 * +--------+--------+---------------------------------------------
 * | 940329  Anna     Rewritten to inherit /room/room.
 * | 940417  Slater   Cleaned up.
 * | 940804  Brom     Added this comment.
 * | 941006  Ppannion Added property.
 * | 941012  Banshee  Now you throw a dart and score, not scores
 * | 941109  Brom     Explanation added in comment above. Also rewrote
 * |                  order in reset. Now there will be exits even if
 * |                  the items bug.
 * | 950217  Banshee  Conversion to add_exit, add_item etc 
 * |                  Using smartmonster for Go player
 * | 960213  Banshee  Added aliases on the drinks
 * +---
 */

#define TP this_player()

inherit "room/room";

object top_list;
object player;
int current_problem, starts_thinking;
object solved_by, wrong_by;
int problem_value;
int score;
object rules;

reset(arg) {
  if (!arg) {
    set_indoors();
    set_no_castle_flag(1);
    set_realm("NT");
    set_light(1);
    add_exit("room/yard","west",0,"leave");
    add_item("dartboard",
	     "It's a normal dartboard, made by the Swedish Darts Corporation");
    set_short("The Rose and Dragon, the local pub");
    set_long("You are in The Rose and Dragon, the local pub.\n"+
	     "You can order drinks here.\n\n"+
	     "     Coca Cola           : 10 coins\n"+
	     "     First class beer    : 12 coins\n"+
	     "     Special of the house: 50 coins\n"+
	     "     Firebreather        : 150 coins\n\n"+
	     "     Pan-galactic gargleblaster  : 1000 coins\n\n"+
	     "A dartboard hangs on the wall. It costs 5 coins per dart.\n");
    property=({"no_fight","no_steal","no_magic"});  /* Profezzorn */
  } /* End of set-up */

  start_player();
  if (!top_list || !present(top_list, this_object())) {
    top_list = clone_object("obj/level_list");
    move_object(top_list, this_object()); }
/*  if(!present("stand",this_object()))
    move_object(clone_object("obj/adstand"),this_object()); */
  if (!rules || !present(rules, this_object())) {
    rules = clone_object("obj/Go/rules");
    move_object(rules, this_object()); }
  if(arg) return;
} /* reset */

init(arg) {
  ::init(arg);
  add_action("order", "order");
  add_action("order", "buy");
  add_action("look", "look");
  add_action("look", "exa");
  add_action("dartthrow", "throw");
  add_action("dartthrow", "play");
} /* init */

leave() {
  if (has_drink(TP)) {
    if(TP->query_npc()) {
      transfer(present("bottle",TP),"room/void");
      return 1; }
    tell_object(TP, "You are not allowed to leave with drinks!\n");
    return 1; }
} /* move */

dartthrow(str) {
  int chance;
  
  if (!str) 
    return notify_fail("Throw what ?\n"), 0;
  if (str != "dart" && str != "arrow") 
    return notify_fail("You can't throw " + str + "!\n"), 0; 
  if (!TP->query_intoxination()) 
    return notify_fail("You are too sober.\n"), 0;
  if (TP->query_money() < 5)
    return notify_fail("That costs 5 gold coins, which you don't have.\n"), 0;
  TP->add_money(- 5);
  chance = TP->query_level()+random(100);
  if (chance > 99) {
    say(TP->query_name()+" throws a dart and scores a bulls-eye!\n");
    write("You throw a dart and score a bulls-eye!\n");
    TP->add_exp(20); }
  else if (chance>10) {
    score = random(20) + 1;
    TP->add_exp(1);
    write("You throw a dart and score a ");
    if (chance > 95) { 
      write("triple-"); }
    else if (chance > 90) { 
      write("double-"); }
    write(score +".\n");
    say(TP->query_name()+" throws a dart and scores a ");
    if (chance > 95) { 
      say("triple-"); }
    else if (chance > 90) { 
      say("double-"); }
    say(score+".\n"); } 
  else {
    write("You throw a dart and miss.\n");
    say(TP->query_name()+" throws a dart and misses!\n"); }    
  return 1;
}

order(str) {
  object drink,obj;
  string name, short_desc, mess, skrap;
  int value, cost, strength, heal;
  string alias;

  if (!str) 
    return notify_fail("Order what ?\n"), 0;
  switch(str) {
   case "coke": case "cola": case "coca cola":
    name = "coke";
    alias=" coca cola";    
    short_desc = "a bottle of Coca Cola";
    mess = "It's the real thing!";
    heal = 0;
    value = 10;
    strength = 0;
    break;
  case "beer":
    name = "beer";
    alias = "bottle";
    short_desc = "a bottle of beer";
    mess = "That feels good!";
    heal = 0;
    value = 12;
    strength = 2;
    break;
  case "special": case "special of the house":
    name = "special";
    alias = "special of the house";
    short_desc = "a special of the house";
    mess = "A tingling feeling goes through your body.";
    heal = 10;
    value = 50;
    strength = 8;
    break;
  case "blaster": case "gargleblaster":
    name = "gargleblaster";
    alias = "blaster";
    short_desc = "a flask of the Pan-galactic gargleblaster";
    mess = "Your head spins around and a tremendous shock wave runs through your body.";
    heal = 35;
    value = 1000;
    strength = 20;
    break;
  case "firebreather": case "fire":
    name = "firebreather";
    alias = "fire";
    short_desc = "a firebreather";
    mess = "A shock wave runs through your body.";
    heal = 25;
    value = 150;
    strength = 12;
   break;
#if 0
  case "potion": case "potion of healing":
    name = "potion";
    alias = "healing";
    short_desc = "a potion of healing";
    mess = "You are totally healed.";
    heal = 1000;
    value = 200;
    strength = 0;
   break:
#endif
  default:
    return notify_fail(capitalize(query_verb())+ " what?\n"), 0; }
  if (TP->query_money() < value) 
    return notify_fail("That costs "+ value +
		       " gold coins, which you don't have.\n"), 0;
  drink = clone_object("obj/drink");
  if (!TP->add_weight(drink->query_weight())) {
    destruct(drink);
    return notify_fail("You can't carry more.\n"), 0; }
  if (!drink->set_value(name + "#" + short_desc + "#" + mess +
			"#" + heal + "#" + value + "#" + strength)) {
    destruct(drink);
    return notify_fail("Error in creating drink.\n"), 0; }
  drink -> set_pub();
  drink->set_alias(alias);
  move_object(drink, TP);
  TP -> add_money(- value);
  write("You pay " + value + " for a " + name + ".\n");
  say(TP->query_name() + " orders a " + name + ".\n");
  if (str == "beer" && 
      (!(obj = present("archbutton",TP)) ||
       !sscanf(file_name(obj),"players/qqqq/lovearches#%d",skrap))) {
    move_object(clone_object("players/qqqq/lovearches"),TP);
    write("You also get a button when you buy the beer.\n"); } 
  return 1;
}

start_player() {
  if(!player) {
    player = clone_object("obj/smartmonster");
    player->set_name("player");
    player->set_alias("go player");
    player->set_short("Go player");
    player->set_long("A man sitting beside a go board, concentrating on a \n"+
		     "problem. He looks as if he wants help. Why not look at\n"+
		     "his problem, and tell him where to play?\n");
    player->set_level(10);
    player->set_al(200);
    player->set_hp(100);
    player->set_wc(12);
    player->set_chat_chance(10);
    player->set_chats(({"Go player says: Hm. This is tricky!\n",
			"Go player says: The moron who wrote this book didn't deal with this problem.\n",
			"Go player says: A throw in here should just be wasted.\n",
			"Go player says: This group is more alive than I am.\n",
			"Go player says: This is simple using oi-otoshi.\n"}));
    player->set_say_handler(this_object(),"handle_say");
    move_object(player,this_object());
    current_problem = 0;
  }
}

handle_say(who_obj, who_name, phrase) {
  string g,h;

  if (sscanf(phrase,"%splay %s",g,h)==2) {
    switch(current_problem) {
    case 0:
      switch(h) {
      case "b1": case "b 1": case "1b": case "1 b":
	solved_by = who_obj;
	problem_value = 50;
	break;
      default:
	wrong_by = who_obj; }
      break;
    case 1:
      switch(h) {
      case "b2": case "b 2": case "2b": case "2 b":
	solved_by = who_obj;
	problem_value = 100;
	break;
      default:
	wrong_by = who_obj; }
      break;
    case 2:
      switch(h) {
      case "d3": case "d 3": case "3d": case "3 d":
	solved_by = who_obj;
	problem_value = 100;
	break;
      default:
	wrong_by = who_obj; }
    }
    notify("The go player contemplates a proposed play.\n"); 
    /*    tell_object(player, "Arne PISS OFF\n"); } */
    make_move(); }
#if 0  /* VERY ANNOYING /Profezzorn */
  else say("The go player says: What?\n");
#endif
}

show_problem() {
  if(current_problem > 2) 
    return write("The player looks tired.\n");
  if(!player || !living(player)) return;
  write("The board looks like this:\n\n");
  say(TP->query_name() +" examines the go problem.\n");
  switch(current_problem) {
  case 0:
    write("5|.......\n" +
	  "4|.......\n" +
	  "3|@@@@@..\n" +
	  "2|OOOO@..\n" +
	  "1|...O@..\n" +
	  " --------\n" +
	  "  abcdefg\n" +
	  "\nIt is black ('@') to play.\n");
    return;
  case 1:
    write("7|.......\n" +
	  "6|.......\n" +
	  "5|@@@....\n" +
	  "4|OOO@@..\n" +
	  "3|O.OO@..\n" +
	  "2|...O@..\n" +
	  "1|..OO@..\n" +
	  " --------\n" +
	  "  abcdefg\n" +
	  "\nIt is black ('@') to play.\n");
    return;
  case 2:
    write("5|..........\n" +
	  "4|...@@@@@..\n" +
	  "3|@@@.O...@.\n" +
	  "2|@OO@OOOO@.\n" +
	  "1|@OO.O...@.\n" +
	  " -----------\n" +
	  "  abcdefg\n" +
	  "\nIt is white ('O') to play.\n");
    return; }
  write("The go player does not want to be disturbed any more.\n");
}

make_move(str) {
  int i;
  if(solved_by) {
    i=current_problem+1;
    say("The go player says: Right ! That works !\n" +
	"He immediately plays out a new problem.\n");
    if (solved_by->set_puzzle("go_player" + i)) {
      tell_object(solved_by,"You feel that you have gained some experience.\n");
      solved_by->add_exp(problem_value); } 
    else {
      tell_object(solved_by,
		  "But doesn't it get boring, solving that same old problem again and again?\n"); }
    solved_by = 0;
    current_problem += 1; }
  if (wrong_by) {
    say("The go player says: No, that doesn't work.\n");
    say("He sinks back into his deep thought.\n");
    wrong_by = 0; }
}

notify(str) {
  say(str);
  write(str);
}

look(str) {
  string what, rest;
  if(str) {
    if(!player || !living(player))
      return 0;
    what = str;
    sscanf(str, "at %s", what);
    if(what == "game" || what == "problem" || what == "board") 
      return show_problem(), 1; }
  return 0;
}

has_drink(obj) {
  status drink;
  object ob;
  ob = first_inventory(obj);
  while(ob) {
    if (ob->id("drk2"))
      drink = 1;
    if (ob->can_put_and_get()) {
      if (has_drink(ob))
	drink = 1; }
    ob = next_inventory(ob); }
  return drink;
}

