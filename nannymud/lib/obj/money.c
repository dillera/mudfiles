/* /obj/money.c - the generic money object 
   Rewritten by Slater, 940627
   Cursed by Profezzorn, 940715
   */

#define PARANOIA
/* #define HLD "obj/daemon/hld" */

static int money;
int volatile;
string name;

#define Sp (money == 1 ? "" : "s")


void join_money()
{
  object o;
  if(!environment()) return;
  if(environment()->query_living() && volatile)
  {
    environment()->add_money(money);
    destruct(this_object());
  }
  else if((o=present("money",environment())) && o!=this_object() ||
	  ((o=present("money 2",environment())) && o!=this_object()))
  {
    o->set_money(o->query_value()+money);
    destruct(this_object());
  }
}

reset(arg)
{
  if(arg) return;
  money = 1;
  call_out("join_money",1);
}

query_weight() { return 0; }

short() { return money ? (money +" gold coin"+ Sp) : 0; }

int set_volatile() { volatile++; }
query_name() { return name; }

get()
{
  if(this_player() && !interactive(this_player()))
  {
    call_out("destruct_later", 0);
    this_player() -> add_money(money);
    money = 0;
  }
  return 1;
}

#ifndef OLD_SYSTEM
init()
{
  if(this_player() &&
     this_player()==environment() &&
      sscanf(file_name(this_player()),"obj/player#%*s"))
  {
    call_out("destruct_later", 0);
    this_player() -> add_money(money);
    money = 0;
  }
}
#endif

_signal_given()
{
  if(environment() && environment()->query_npc() && money)
  {
    environment()->add_money(money);
    call_out("destruct_later", 0); 
    /* destruct(this_object()); */
    /* Changed by Banshee 950217. Smartmonster depending on it. */
  }
}

add_value(a) { money+=a;   name=money+" coin"+Sp;  }

set_money(m) {
  int old_money, lvl;
  string who1,who2,where;
  if(!intp(m) || money < 0)
    return 0;

  old_money = money;
  money = m;
  name=m+" coin"+Sp;
#ifdef PARANOIA
  /* this is temporary. Anvil 930212 */
  if (this_player())
    lvl = this_player() -> query_level();
  else
    lvl = 0;
  if(  (this_player() && !this_player()->query_npc())
     && ((19 < lvl && lvl < 22) || (965 < money)))
  {
    who1 = this_player()->query_real_name();
    if(previous_object()) {
      who2 = file_name(previous_object());
      if(environment(previous_object()))
	where = file_name(environment(previous_object())); 
    }
# ifdef HLD
    HLD -> logs(this_player(), this_player(1), this_object(),
		previous_object(), money, 1);
# endif
    log_file("MONEY",ctime(time())+" ["+file_name(this_object())+
	     "]: "+money+" : "+who1+"("+lvl+"): "+who2+ " ("+where+")\n"); 
  }
#endif
  return old_money;
}

id(str) 
{ 
  return str == "gold coin"+ Sp || str == "coin"+ Sp || str == "money"; 
}

destruct_later(arg) { destruct(this_object()); }

query_value() { return money; }

long()
{
  return write((money == 1 ? "It's a " : "The gold coins are ")+
	       "rather small and shiny"+ (money == 1 ? " gold coin" :"")+".\n"+
	       "There is a picture of Mats on one side of the coin"+ Sp +"\n"+
	       "and a crown on the other side.\n"), 1;
}

/* There are times when query_money() is not enough to see
 * if it is money; it uses it as the value ...
 */
query_money() { return 1; }

/* This makes life easier in /obj/support among other places -
 * without looking at file names.
 * Brom 951016
 */
is_money() { return 1; }
