inherit "/obj/monster";

void reset(int arg) {
  ::reset(arg);
  if( !arg ) {
    object crown;

    set_name("frog");
    set_alias("toad");
    set_short("a frog");
    set_long("It is your typical frog: small, green, and warty.  However,\n"+
	"perched atop its head is a tiny crown.  Perhaps it is a prince\n"+
	"or princess enchanted by some witch?\n");
    set_race("frog");
    set_level(1);
    set_frog(1);
    set_wc(4);

    crown = clone_object("/std/simple_armour");
    crown->set_name("crown");
    crown->add_alias("iron crown");
    crown->set_short("An iron crown");
    crown->set_long("Actually, it is made of iron and seems to only be "+
	"painted gold.\nIt is tiny and would barely fit you.\n");
    crown->set_class(1);
    crown->set_weight(2);
    crown->set_type("helmet"); /* Added by Banshee. 960118 */
    crown->set_value(18);
    move_object(crown, this_object());
    command("wear crown");
  }
}

void init() {
  add_action("_kiss_verb", "kiss");
  add_action("_lick_verb", "lick");
  add_action("_lick_verb", "suck");
}

int _kiss_verb(string str) {
  if( id(str) ) {
    write("You kiss the frog hopefully.\n");
    say(this_player()->query_name()+" kisses the frog.\n", this_player());
    if( this_player()->query_frog() ) 
      write("However, nothing happens.\n");
    else {
      this_player()->frog_curse(1);
    }
    return 1;
  }
  notify_fail("Kiss whom?\n");
}

int _lick_verb(string str) {
  if( id(str) ) {
    write("You "+(query_verb()=="lick" ? "lick" : "suck on")+
	" the frog.  You feel a brief euphoric high.\n");
    say(this_player()->query_name()+" "+
	(query_verb()=="lick" ? "licks" : "sucks on")+" the frog, and "+
	"looks high for a moment.\n");
    return 1;
  }
  notify_fail(capitalize(query_verb())+" whom?\n");
}
