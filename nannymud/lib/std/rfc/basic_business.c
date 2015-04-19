/* Written by Mats */

#pragma strict_types
#pragma save_types

inherit "/std/room";

/*** variables ***/

private mixed *sell_list = ({});      /* all these could be in one array */
private int *numbers = ({});
private int *current = ({});
private mixed in_store;

private string *s_descs = ({});
private string *ids = ({});
private int *prices = ({});


private object storeroom;
private object *restored;
private string the_list;

/*** useful defines ***/

#define ERROR(X) tell_object(find_player(creator(this_object())||"")||this_player()||this_object(),"ERROR "+(X))

/*** Setup functions ***/

void store(object o) {
  if (!storeroom) {
    storeroom=clone_object("/std/rfc/store");
    storeroom->set_master(this_object());
  }
  move_object(o,storeroom);
}

varargs void add_sell(mixed ob, int n, int p, string i, string s)
{
  mixed o;
  string m;
  if(stringp(ob)) {
    if(sscanf(ob,"a %s",m) || sscanf(ob,"an %s",m)) {
      if (!s) s = ob;
      ob="@/std/lib->make("+m+")";
    } else
      ob=expand_file_name(ob);
  }
  sell_list += ({ob});
  numbers += ({n});
  current += ({n || 1});

  prices += ({p});
  ids += ({i});
  s_descs += ({s});
  
  if(objectp(ob))
    store(o);
}

string query_list() {
  int i,n;
  mixed o;
  object *all;

  if (!the_list) {
    the_list = "";
    in_store = ({});
    if (!restored) restored = allocate(sizeof(sell_list));
    for(i=0;i<sizeof(sell_list);i++)
      if (current[i]) {
	if (ids[i]) {
	  n++;
	  in_store += ({i});
	  the_list += sprintf("%2d. %-20'.'s%4d\n",n,s_descs[i],2*prices[i]);
	} else {
	  if (!restored[i] && !objectp(sell_list[i])) {
	    o=eval(sell_list[i]);
	    if(stringp(o)) o=clone_object(o);
	    restored[i] = o;
	    store(o);
	  }
	}
      }
    if (storeroom) { /* is storeroom needed? */
      all = all_inventory(storeroom);
      for(i=0;i<sizeof(all);i++) {
	n++;
	in_store += ({all[i]});
	the_list += sprintf("%2d. %-20'.'s%4d\n",n,all[i]->short(),2*all[i]->query_value());
      }
    }
  }
  return the_list;
}

int list(string arg, string what) {
  write(query_list());
  return 1;
}

int buy(string arg, string what) {
  mixed o;
  int i,v;

  if (!what) return notify_fail("Buy what?\n"),0;
  if (!the_list)
    query_list();

  i = -1;
  if (sscanf(what,"%d",v) && v>0 && v<=sizeof(in_store)) {
    if (intp(o = in_store[v-1])) {
      i = o;
      o = 0;
    }
  } else
    if ((!storeroom || !(o = present(what,storeroom))) && (i=member_array(what,ids))==-1) {
      write("There is no "+what+" in store.\n");
      return 1;
    }
  if (o) {
    v = o->query_value();
  } else {
    if (current[i]) {
      o = eval(sell_list[i]);
    } else {
      write("There is no "+what+" in store.+\n");
      return 1;
    }
    v = prices[i];
  }
  
  if (this_player()->query_money() < 2*v) {
    write("You can't afford "+(objectp(o)?o->short():s_descs[i])+".\n");
    return 1;
  }
  this_player()->add_money(-2*v);
  if (stringp(o)) o = clone_object(o);
  if( transfer(o, this_player()) ) {
    write("You cannot carry any more. The "+o->short()+" is laid\n"+
          "on the ground in front of you.\n");
    move_object(o, this_object());
  }
  
  if (i == -1 && (i=member_array(o,restored))!=-1) {
    the_list = 0;
    restored[i] = 0;
  }
  if (i != -1)
    if (--current[i] == 0)
      the_list = 0;

  write("Ok.\n");
  return 1;
}

/* should be changed
void remove_sell(mixed ob)
{
  int i;
  if(stringp(ob))
  {
    if(sscanf(ob,"a %s",ob) || sscanf(ob,"an %s",ob))
    {
      ob="@/std/lib->make("+ob+")";
    }else{
      ob=expand_file_name(ob);
    }
  }

  i=member_array(ob,local_objects);
  if(i==-1) return;
  i&=~1;
  local_objects=local_objects[0..i-1]+local_objects[i+2..0x7ffffff];
}

*/
/*** functions called by mudlib / driver ***/
mixed *query_sell_list() { return sell_list; }
object query_storeroom() { return storeroom; }
int *query_current() { return current; }
object *query_restored() { return restored; }
string *query_s_descs() { return s_descs; }
string *query_ids() { return ids; }
int *query_prices() { return prices; }
mixed query_in_store() { return in_store; }

/* All commands go through here */
int do_cmd(string what)
{
  if(::do_cmd(what)) return 1;

  if (query_verb()=="buy") return buy("",what);
  if (query_verb()=="list") return list("",what);
}

/* Don't forget to call ::reset(arg), or add_sell
 * will not work properly
 */
void reset(int arg)
{
  int i;

  ::reset(arg);
  for(i=0;i<sizeof(numbers);i++)
  {
    current[i] = numbers[i];
    if (restored[i]) destruct(restored[i]);
  }
  restored = 0;
}

/* Clean up takes into account which objects where cloned
 * in this room and which were not.
 */
int clean_up(int refs)
{
  return ::clean_up(refs);
}
