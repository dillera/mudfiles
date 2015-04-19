inherit "/std/room";

#define VAULT      "/room/bank/vault"
 
reset(arg)
{
  if (arg) return;
  
  set_light(1);
  add_property("indoors");
  add_property("no_fight");
  add_exit("south","/room/bank");
  set_short("Head-office of Aurora's first National Bank");
  set_long("Head-office of Aurora's first National Bank. The marble floor combines\n" +
	   "beautifully with the bronze pillars that support the ceiling. A large fresco\n" +
	   "painted on the ceiling pictures the founder of the bank.  On the west wall is a\n"+
	   "golden plate. To the north lies the vault, protected by a Santing & Weston\n"+
	   "magic shield.\n");
  
  add_item(({"fresco","painting"}),
	   "A beautiful fresco depicting the founder of the bank digging for the dilithium\n" +
	   "crystals that made him disgustingly rich.\n");
  add_item("ceiling",
	   "The ceiling looks ordinary except for the large fresco painted on it.\n");
  add_item("vault",
	   "The vault is protected by the strongest magic shield available.\n");
  add_item("shield",
	   "A Santing and Weston magic shield, the strongest magic shield available.\n");
  add_item(({"plate","golden plate"}),
	   "A golden plate, hanging on the west wall, there is some writing on it.\n");
  add_item(({"pillar","pillars","bronze pillar","bronze pillars"}),
	   "Strong bronze pillars, supporting the ceiling.\n");
  add_item(({"floor","marble floor"}),
	   "A beautiful marble floor. It is made out of a single piece of marble and must\n" +
	   "have cost a large fortune.\n");
  
  add_item_cmd("read","plate",
	       "This is Aurora's first fully automated bank. Ask the teller for help,\n" +
	       "type:  'say Teller help'\n");
  add_command("ask","Just say something like 'teller help'\n");

   add_command("north","@north()");  
   add_command("open %s","@open()");
   add_command("revoke %s","@revoke()");
   add_command("new %s","@new()");
   add_command("balance %s","@balance()");
   add_command("deposit %s","@deposit()");
   add_command("withdraw %s","@withdraw()");

   add_command("report!","@report()");
   add_command("balance!","@show_total()");  
   add_command("remove!","@remove()");  
   add_object("/room/bank/robot_bank");
} /* reset */

init() {
  ::init();
  VAULT->check_player(this_player());
}

north()
{
  write("You get a terrible shock as you try to go through the shield. You shouldn't try\n" +
	"this too often, you might get killed.\n");
  say(this_player()->query_name() + " tries to go through the shield but is thrown back by\n" +
      "a bolt of light. " + capitalize(this_player()->query_pronoun()) +
      " seems to have been hurt.\n");
  if (!this_player()->query_npc())
    this_player()->reduce_hit_point(10 + random(10));
  return 1;
} /* north */


wiz(obj)
{
  object player;

  if (objectp(obj)) player = obj; else player = this_player();
  if (player->query_level() > 19 && interactive(player))
    return 1;
 return 0;
} /* wiz */


open(arg,str) {
  if (!present("teller",this_object())) {
    write("There is no teller here that can help you.\n");
    return 1;
  }
  return VAULT->open(str);
} /* open */

new(arg,str) {
  if (!present("teller",this_object())) {
    write("There is no teller here that can help you.\n");
    return 1;
  }
  return VAULT->new(str);
} /* new */

withdraw(arg,str) {
  if (!present("teller",this_object())) {
    write("There is no teller here that can help you.\n");
    return 1;
  }
  return VAULT->withdraw(str);
} /* withdraw */

revoke(arg,str) {
  if (!present("teller",this_object())) {
    write("There is no teller here that can help you.\n");
    return 1;
  }
  return VAULT->revoke(str);
} /* revoke */

deposit(arg,str) {
  if (!present("teller",this_object())) {
    write("There is no teller here that can help you.\n");
    return 1;
  }
  return VAULT->deposit(str);
} /* deposit */

balance(arg,str) {
  if (!present("teller",this_object())) {
    write("There is no teller here that can help you.\n");
    return 1;
  }
  return VAULT->balance(str);
} /* balance */


/* Some special wiz commands */

show_total() {
  if (wiz()) return VAULT->show_total();
  return 0;
} /* show_total */

report() {
  if (wiz()) return VAULT->report();
  return 0;
} /* report */

remove() {
  if (wiz()) return VAULT->remove();
  return 0;
} /* remove */
