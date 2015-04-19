/* 
   The last thing you can accuse this room for is being efficient or
   even being clerely coded...
   But it is meant to make shareroom coding more easily done.
   Plus that this system uses an environment variable to make
   looting a smaller problem. That is if player X loots a guilds shareroom
   and then leaves the guild and joins another one and both uses this
   shareroom the result will be that the player won't be able to use
   the new guilds shareroom and will keep the same status as before.

   These actions are defined:
   Command: list     Function: list
   It shows a list of all items in the shareroom that matches a 
   specific pattern.
   These are the diffrent patterns:
   none: Show all items in the shareroom
   weapons: Show all weapons in the shareroom
   armours: Show all armours in the shareroom
   others: Shows all items that are not a weapon or an armour
   <id> : Show all items with the id <id>.

   Command: store    Function: store
   Command: store    Function: deposit
   Store items into the shareroom and update your credits.
   You can only get max 1000 credits for an item.
   Syntax:
   store sword 
   -> You store the first sword in you inventory.
   store sword {in,from} <container> 
   -> You store the first sword in <container>.
   store all 
   -> You store all objects in your inventory.
   -> This only shows the number of items stored.
   store all {in,from} <container> 
   -> You store all in the container.
   -> This only shows the number of items stored.
   
   The store fails if:
   * There is more than max_items of the same id in the shareroom.
   * The objects has one of there properties: 
     special_item, fragile, unique_item, quest_item
   * If the value of the object is less than lowest_value.
   * If the object is wielded or worn.
   * If your credit is lower than lower_limit.
   * The object can be moved from its current position.
 
   Command: withdraw Function: withdraw
   You with draw an object from the shareroom and your credits
   is updated.
   Syntax:
   withdraw sword <number>
   -> You take sword <number> from the shareroom.
   withdraw sword to backpack
   -> You take the sword from the shareroom and put it in the backpack.
   withdraw item <number>
   -> you take item <number> from the shareroom.
   
   The withdraw fails if:
   * It can't be moved.
   * It is to heavy for its destination.
   * the number is higher than the number of that items in the shareroom

   Command: credits  Function: credits
   Show your current credit in the Shareroom.

   Command: top      Function: top
   Shows the max_on_top_list givers and takers.

   Changelog:
     Date     By                           What
   -----------------------------------------------------------------------
   | 950915  Taren  Moved this room to /std/special and added more comments.
   | 950917  Taren  Made it possible to not alter credits during reboot.
   | 951022  Taren  Added 'decay' it works abit like the wizlist.
   | 960313  Taren  Made it possible for the shareroom NOT to use the env var.

 */


static inherit "std/room";
static inherit "obj/simplemore";

static string log_store="/log/SHARE";
static string log_withdraw="/log/SHARE";
static string save_file;
static string storage="room/void";
static string list_obj;
static int time_between_stamp=120;
static int store_time;
static int withdraw_time;
static int lower_limit=-15000;
static int lowest_value=50;
static int log_mode=1; /*Log everything*/
static int max_on_top_list=10;
static int max_items=5; /*One less all the time */
static int give_credits_for_others=1;
static int alter_credits_when_reboot=1;
static int use_decay=0;
static int decay_percent=1;
static int use_env_var=1;
static mapping _items;

mapping top=([]);

/*
  Set this if you want an honor type shareroom, ie NO credits are used.
 */
set_honor_system(int i) {
  use_env_var=!i; 
}

/*
   Set this if you want the shareroom list to decay.
*/

set_use_decay(int i)
{
  use_decay=i;
}

/*
   Set the amount of percent to decay.
 */
set_decay_percent(int i)
{
  if(i>0&&i<100)
    decay_percent=i;
}

/*
   If you don't want credits to be taking during reboot set
   this to 0.
*/
int set_alter_credits_when_reboot(int i)
{
  alter_credits_when_reboot=i;
}

/*
   If you don't want others (not weapons and not armours)
   to give credit set this to 0.
*/

int set_give_credits_for_others(int i)
{
  give_credits_for_others=i;
}

/*
   Set the maximal number of items that can be stored of one id.
   That is if you have set this to 5 and there is 4 things
   that ids on 'sword' and try to store another 'sword' it
   will fail.
*/

void set_max_items(int i)
{
  max_items=i;
}

/*
   Set the maximal numbers of players to be showed by the default
   top list function.
*/

void set_max_top_list(int i)
{
  max_on_top_list=i;
}

/*
   Set the type of log mode, these log modes are available:
   type 1: Log everything.
   type 2: Only log weapons and armours.
 */

void set_log_mode(int i)
{
  log_mode=i;
}

/*
   Where do we save our files that need to be saved?
*/

set_save_file(string st)
{
  save_file=st;
}

/*
   Set The lower limit of credits you can have in this Shareroom
*/

void set_lower_limit(int i)
{
  lower_limit=i;
}

/* Set the lowest value that can be stored */
void set_lowest_value(int i)
{
  lowest_value=i;
}

/* 
   Set an object to do the listing of things in the Share Room
*/

int set_list_obj(string ob)
{
  list_obj=ob;
  return 1;
}

/*
   Set the storage room.
   Example: set_storage("/guilds/khorne/rooms/storage");
*/

int set_storage(string st)
{
  if(stringp(st))
    {
      storage=st;
      return 1;
    }
  return 0;
}

/*
   Set where we log all stores.
   Example: set_log_store("/guilds/khorne/log/STORE");
*/

int set_log_store(string fname)
{
  if(stringp(fname))
    {      
      log_store=fname;
      return 1;
    }
  return 0;
}

/*
   Set where we log all withdraws.
   Example: set_log_store("/guilds/khorne/log/WITHDRAW");
*/
set_log_withdraw(string fname)
{
  log_withdraw=fname;
}

/*
   This function resets the list...
*/
void reset_list()
{
  top=([]);
}

decay()
{
  mixed ind;
  mapping tmp;
  int i,val;

  if(!use_decay)
    return 1;
  
  tmp=([]);
  ind=m_indices(top);
  for(i=0;i<sizeof(ind);i++)
  {
    val=top[ind[i]];
    val=val-(val*decay_percent)/100;
    tmp[ind[i]]=val;
  }
  top=tmp;
}

/*
   Remove a player from the list.
*/

remove_player(object who)
{
  top=m_delete(top,who->query_real_name());
  save();
}

/* 
   Below is just the functions that does things... 
   If you really don't have to do read it...
*/

reset(arg)
{
  if(arg) return;
  call_out("restore",2);
  call_out("decay",4);
}

restore()
{
  if(save_file&&stringp(save_file))
    restore_object(save_file);
}

save()
{
  if(save_file&&stringp(save_file))
    save_object(save_file);
}

void _exit()
{
  catch(save());
}

void init()
{
  ::init();
  add_action("list","list");
  add_action("store","store");
  add_action("store","deposit");
  add_action("withdraw","withdraw");
  if(use_env_var) {
    add_action("credits","credits");
    add_action("top","top");
  }
}

describe(object ob,int value)
{
  string s;
  s=ob->short();
  if(value) 
    s+=" ["+ob->query_value()+"]";
  if(s)
    return capitalize(s);
  else 
    return "(Invisible)";
}

void do_error_log(string str)
{
  /* Nah, no more mess, /T  Date: Sun Mar 17 14:05:46 1996
    if(find_player("taren"))
    tell_object(find_player("taren"),"*** "+str);
    */
}

void do_log(object ob,string type)
{
  int value;
  
  value=ob->query_value();

  switch(log_mode)
    {
    case 1:
    default:
      break;
    case 2:
      if(ob->is_weapon()||ob->is_armour())
	return;
      break;
    }

  if(type=="store"||type=="deposit")
    {
      if(time()>(store_time+time_between_stamp))
	{
	  store_time=time();
	  write_file(log_store,ctime(time())+"\n");
	}
      write_file(log_store,this_player()->query_real_name()+" stored '"+describe(ob)+"' Value: "+value+" Creator: "+creator(ob)+" Fname: "+file_name(ob)+"\n");
    }
  else
    {
      if(time()>(withdraw_time+time_between_stamp))
	{
	  withdraw_time=time();
	  write_file(log_withdraw,ctime(time())+"\n");
	}
      write_file(log_withdraw,this_player()->query_real_name()+" withdrew '"+describe(ob)+"' Value: "+value+" Creator: "+creator(ob)+" Fname: "+file_name(ob)+"\n");
    }
}

int get_credits()
{
  return this_player()->getenv("STORE");
}

void set_credits(int i)
{
  if(use_env_var)
    {
      this_player()->setenv("STORE",i);
      top[this_player()->query_real_name()]=i;
    }
}

mapping query_top()
{
  return top;
}

int in_mapping(object who)
{  
  if(!who||!objectp(who))
    return 0;
 return member_array(who->query_real_name(),m_indices(top))>-1;
}

void add_2_mapping(object who)
{
  int cur;
  cur=who->getenv("STORE");
  top[who->query_real_name()]=cur;
  do_error_log("shareroom : added "+who->query_real_name()+" to the map "+
	       cur+" ("+file_name(this_object())+")\n");
}

fix()
{
  int cur;
  cur=get_credits();
  if(!in_mapping(this_player()))
    add_2_mapping(this_player());
  
  if(cur!=top[this_player()->query_real_name()])
  {
    do_error_log("shareroom:"+this_player()->query_real_name()+
		 " diffrent mapping and env\n"+
		 " Map:"+top[this_player()->query_real_name()]+
		 " Env: "+cur+" ("+file_name(this_object())+")\n");
    /*
      this_player()->setenv("STORE",top[this_player()->query_real_name()]);
      cur=get_credits();
      */
    if (stringp(cur)) /* patch by Brom 960329 */
      top[this_player()->query_real_name()]=atoi(cur);
  }  
  
}

int credits()
{
  fix();
  write("Your current credit is: "+get_credits()+".\n");
  return 1;
}

int top()
{
  int i,o,p,wiz;
  mixed val,ind;
  mapping map;

  fix();
  
  val=m_values(top);
  ind=m_indices(top);
  map=mkmapping(val,ind);
  ind=m_values(map);
  val=m_indices(map);  
  
  o=sizeof(ind);
  p=o-1;
  if(o>max_on_top_list) o=max_on_top_list;
  wiz=this_player()->query_level()>20;
  write(sprintf("%=25s %-1s %=30s","Top "+o+" Takers"," ","Top "+o+" Givers")+"\n");
  for(i=0;i<o;i++)
    {      
      if(wiz)
	write(sprintf("%2d: %-20s%10d %2d: %-20s%10d\n",(i+1),capitalize(ind[i]),val[i],(i+1),capitalize(ind[p]),val[p]));
      else
  	write(sprintf("%2d: %-30s %2d: %-30s\n",(i+1),capitalize(ind[i]),(i+1),capitalize(ind[p])));     
      p--;
    }  
  return 1;
}


void update_credits(object ob, string type)
{
  int cur,val,i,other;
  mixed ind;

  cur=get_credits();

  fix();
  
  if(!alter_credits_when_reboot)
    if(file_size("/etc/NOLOGIN") > 0)
      return;

  val=ob->query_value();
  if(ob->is_weapon()||ob->is_armour())
     other=0;
   else 
     other=1;

  if(val>1000) val=1000;
  if(type=="store"||type=="deposit")
    {
      if(!other)
	cur+=val;
      else
	if(give_credits_for_others&&other)
	  cur+=val;
    }
  else
    {
      if(!other)
	cur-=val;
      else
	if(give_credits_for_others)
	  cur-=val;
    }  
  if(!give_credits_for_others&&other) return;
  this_player()->setenv("STORE",cur);
  top[this_player()->query_real_name()]=cur;
  save();
}

count_items(string st)
{
  mixed all;
  int i,no;
  no=0;
  all=all_inventory(load_object(storage));
  for(i=0;i<sizeof(all);i++)
    if(all[i]->id(st)) no++;
  return no;
}

int my_transfer(object ob, object to,int silent_error)
{
  int res,val;


  if(ob->query_property("special_item")||ob->query_property("fragile")||ob->query_property("unique_item")||ob->query_property("quest_item"))
    {
      write("You can't "+query_verb()+" that item.\n");
      return 0;
    }

  val=ob->query_value();
  if(val<lowest_value)
    {
      if(!silent_error)
	write("You can't "+query_verb()+" so cheap items.\n");
      return 0;
    }

  if(use_env_var) {
  if(query_verb()=="withdraw"&&((get_credits()-val)<lower_limit))
    {
      write("You can't withdraw more from the Share Room.\n");
      return 0;
    }
  }


  if(ob->query_worn())
    {
      write("You can't "+query_verb()+" worn items.\n");
      return 0;
    }

  if(ob->query_wielded())
    {
      write("You can't "+query_verb()+" wielded items.\n");
      return 0;
    }

  if(query_verb()=="store"&&count_items(ob->query_name())>max_items)
    {
      write("There is already to many of "+ob->query_name()+".\n");
      return 0;
    }

  res=transfer(ob,to);
  switch(res)
    {
      default:
      case 0: 
        write("Ok, you "+query_verb()+" '"+describe(ob)+"'.\n");
	tell_room(this_object(),this_player()->query_name()+" "+query_verb()+"s '"+describe(ob)+"'.\n",({this_player()}));
	_items=0;
	do_log(ob,query_verb());
	if(use_env_var) {
	  update_credits(ob,query_verb());
	}
        return 1;
      case 1: 
	if(!silent_error)
	  write("Too heavy for destination.\n");
	break;
      case 2:
	if(!silent_error)
	  write("Can't be dropped.\n"); 
	break;
      case 3: 
	if(!silent_error)
	  write("Can't take it out of it's container.\n"); 
	break;
      case 4: 
	if(!silent_error)
	  write("The object can't be inserted into bags etc.\n"); 
	break;
      case 5: 
	if(!silent_error)
	  write("The destination doesn't allow insertions of objects.\n");
	break;
      case 6: 
	if(!silent_error)
	  write("The object can't be picked up.\n");
	break;	
    }
  return 0;
}

store_all(object all)
{
  int i,res;
  object store;
  store=load_object(storage);
  res=0;
  for(i=0;i<sizeof(all);i++)	
    res+=my_transfer(all[i],store,1);	
  if(res==0)
    write("No objects stored.\n");
  else
    write("Ok, stored "+res+" objects.\n");
  return 1;
}

store(string str)
{
  object ob,cont;  
  string s1,s2;
  
  if(!str) return write("Store what?\n"),1;
  if(str=="all") return store_all(all_inventory(this_player()));
  if(sscanf(str,"%s in %s",s1,s2)||sscanf(str,"%s from %s",s1,s2))
  {
    cont=present(s2,this_player());
    if(!cont) return write("You don't have any "+s2+".\n"),1;
    if(s1=="all") return store_all(all_inventory(cont));
    ob=present(s1,cont);
    if(!ob)
      return write("You don't have "+s1+" in "+s2+".\n");
    my_transfer(ob,load_object(storage));
    return 1;
  }
  ob=present(str,this_player());
  if(!ob)
    return write("You don't have that item.\n");
  my_transfer(ob,load_object(storage));
  return 1;
}

withdraw(string str)
{  
  string s1,s2;
  object ob,store,cont;  
  int i,t,o;
  mixed all;
  mapping map;

  if(!str)
    return write("Withdraw what?\n"),1;

  /*
     withdraw 1
     withdraw sword
   */
  
  cont=this_player();
  if(sscanf(str,"%s to %s",s1,s2)==2)
  {
    cont=present(s2,this_player());
    str=s1;
  }
  
  if(!str)
    return write("Withdraw what?\n"),1;
  
  store=load_object(storage);
  
  
  sscanf(str,"%s %d",s2,i);
  if(s2!="item")
  {
    ob=present(str,store);
  }
  else
  {
    if(sscanf(str,"%s %d",s2,t)==2)
    {
      map=get_items();
      all=m_indices(map);
      for(i=0;i<sizeof(all);i++)
      {
	sscanf(all[i],"%d:%*s",o);
	if(t==o)
	{
	  ob=map[all[i]];
	  break;
	}
      }
      
    }
  }
  if(!ob)
    return write("There is no such object in the Share room.\n");  
  my_transfer(ob,cont);
  return 1;  
}

filter(object o, mixed data)
{
  switch(data[0])
    {
    case "armour":
      return o->is_armour();
    case "weapon":
      return o->is_weapon();
    case "others":
      if(o->is_weapon()) return 0;
      if(o->is_armour()) return 0;
      return 1;
    default:
      if(stringp(data[0]))
	return o->id(data[0]);
      return 0;
    }
}


int scale_fun(string a) 
{
  a+="\n\n\n\n\n";
  return a[0]*0x200000+a[1]*0x4000+a[2]*0x80+a[3]+0x10000000;
}

mapping get_items(int force)
{
  mapping items,nums,tmp;
  int i,o;
  string s;
  mixed all,val;

  if(_items&&!force)
    return _items;
  all = all_inventory(load_object(storage));
  i=sizeof(all);
  items=([]);
  nums=([]);
  tmp=([]);
  for(i=0;i<sizeof(all);i++)
  {
    s=(string)describe(all[i],use_env_var);
    items[s]=all[i];
    o=nums[s];
    o=o+1;
    nums[s]=o;
  }
  all=m_indices(items);
  for(i=0;i<sizeof(all);i++)
  {
    s=sprintf("%2d: [%d] %s",(i+1),nums[all[i]],all[i]);
    tmp[s]=items[all[i]];
  }
  _items=tmp;
  return tmp;
}


show_list(mixed all)
{
  string out;
  int i;
  mapping map;
  mixed val,ind,tmp;

  map=get_items();  
  val=m_values(map);
  ind=m_indices(map);
  map=mkmapping(val,ind);  
  tmp=({});
  for(i=0;i<sizeof(all);i++)
    tmp+=({ map[all[i]] });
  all=tmp-({0});
  all=m_values(mkmapping(map_array(all, "scale_fun",this_object()),
			 all));  
  i=sizeof(all);
  out="";
  if(i>1)
    out+=sprintf("%|'='75s"," The Share Room contains: "+i+" items of this class ")+"\n";
  else
    out+=sprintf("%|'='75s"," The Share Room contains: "+i+" item of this class ")+"\n";
  out+=sprintf("%-*#s",78,implode(all,"\n"));
  out+=sprintf("\n%'='75s"," ")+"\n";
  simplemore(out);
  flush();
}

int list(string str) 
{
  mixed *all;
  int i;
 
  
  if(list_obj)
    {
      all = all_inventory(load_object(storage));
      return list_obj->show_list(all,str);
    }

  all = all_inventory(load_object(storage));
  if(!all) 
    return write("There is nothing in the Share Room.\n"),1;

  switch(str) 
    {
    case "armour":
    case "armours":
      all = filter_array(all,"filter",this_object(),({"armour"})); 
      break;
    case "weapon":
    case "weapons":
      all = filter_array(all,"filter",this_object(),({"weapon"})); 
      break;
    default:
      if(str)
	all=filter_array(all,"filter",this_object(),({str}));
      break;
    }

  if(sizeof(all)<1)
    return write("There is nothing in the Share Room.\n"),1;
  
  show_list(all);
  return 1;
}

_dest(object ob)
{
  write("A small servent appears and takes the '"+describe(ob)+
	"' and hides it under his coat.\n");
  transfer(ob,load_object("room/void"));
  destruct(ob);
}

valid_object(object ob)
{
  if(interactive(ob)) return;
  if(ob->id("bin")) return;
  if(ob->query_value()<lowest_value)
    return _dest(ob);
  if(ob->query_property("special_item"))
    return _dest(ob);  
}

void clean_up(int refs)
{
  mixed all;
  int i;
  all=all_inventory(this_object());
  for(i=0;i<sizeof(all);i++)
    valid_object(all[i]);
  ::clean_up(refs);
}







