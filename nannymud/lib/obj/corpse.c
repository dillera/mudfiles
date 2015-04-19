/*
 * This is a decaying corpse. It is created automatically
 * when a player or monster die.

object query_i_was()             returns the "owner" of the corpse   
   str query_old_name()          returns the "owner" of the corpse name
   str query_old_real_name()     returns the real name of the owner
   int query_old_dex()           returns the old dex of the owner
   int query_old_int()           returns the old int of the owner
   int query_old_con()           returns the old con of the owner
   int query_old_str()           returns the old str of the owner
   int query_old_exp()           returns the old exp of the owner
   int query_old_level()         returns the old level of the owner
   int query_old_maxhp()         returns the old Hp of the owner
   int query_was_npc()           returns 1 of the owner was a Npc
   int query_old_gender()        returns the old gender of the owner

object query_killed_by()         returns the killer
   str query_killers_name()      returns the killers name
   str query_killers_real_name() returns the killers real name

    Made by Taren /941002
*/


inherit "/obj/property";

#define DECAY_TIME	120

string name;
int decay;
int weight=5;

object killed_by_who,before;
int old_int,old_str,old_dex,old_con,old_exp,old_level,old_maxhp,was_npc,old_gender;
string old_name,old_real_name,killers_name,killers_real_name;

prevent_insert() {
    write("The corpse is too big.\n");
    return 1;
}

init() {
    add_action("search", "search");
}

reset(arg)
{
  if (arg)
    return;

  decay = 2;
}

set_name(n)
{
  name = n;
  call_out("decay", DECAY_TIME);
}

short()
{
  if(!name) return "a corpse";
  if (decay < 2)
    return "The somewhat decayed remains of " + capitalize(name);
  return "corpse of " + capitalize(name);
}

long() {
  if(!name)
    write("It is a dead body.\n");
  else
    write("This is the dead body of " + capitalize(name) + ".\n");
}

id(str)
{
  return str == "corpse" ||
    str == "remains" ||
      (name && str == "corpse of " + lower_case(name)) ;
}

decay()
{
  if (this_object() -> query_property("preserved"))
    return;

  decay --;
  if (decay > 0) {
    call_out("decay", 20);
    return;
  }
  destruct(this_object());
}

can_put_and_get() { return 1; }

search(str)
{
  object ob;
  if (!str || !id(str))
    return 0;
  ob = present(str, environment(this_player()));
  if (!ob)
    ob = present(str, this_player());
  if (!ob)
    return 0;
  write("You search " + str + ", and find:\n");
  say(call_other(this_player(), "query_name") + " searches " + str + ".\n");
  if (!search_obj(ob))
    write("\tNothing.\n");
  else
    write("\n");
  return 1;
}

search_obj(cont)
{
  string *descs;

  if (!call_other(cont, "can_put_and_get"))
    return 0;

  /* Get all descs and hide invisibles in one fell sweep. 
   * Brom 951219.
   */
  descs = (all_inventory(cont) -> short()) - ({ 0 });
  if (sizeof(descs) == 0)  return;

  write(capitalize(implode(descs, ", ")));
  return sizeof(descs);
}

get()
{
  string *shorts;
  if (this_player() -> query_level() > 19) return 1;
  /* Get all descs and hide invisibles in one fell sweep.
   * Brom 951219
   */
  shorts = (all_inventory(this_object()) -> short()) - ({ 0 });
  if (sizeof(shorts))
    switch(query_verb())
    {
    case "take" :
    case "get"  :
    case "pick" :
      write("You fail to pick up the corpse because of the things it contains.\n");
      return 0;
      break;
    default :
      break;
    }
  return 1;
}

query_name() { return "corpse of "+ capitalize(name); }
query_decay() { return decay; }

query_weight() {
    return weight;
}

/******************************** NEW ***********************************/

set_up(object victim,object killer)
{
  if (victim) i_was(victim);
  if (killer) killed_by(killer);
}

killed_by (object who)
{
  if (!who) return;
  killed_by_who=who;
  killers_name=who->query_name();
  killers_real_name=who->query_real_name();
}

i_was(object who)
{
  if(!who) return;
  before=who;
  old_name=before->query_name();
  old_real_name=before->query_real_name();
  old_dex=before->query_dex();
  old_int=before->query_int();
  old_con=before->query_con();
  old_str=before->query_str();
  old_exp=before->query_exp();
  old_level=before->query_level();
  old_gender=before->query_gender();
  old_maxhp=before->query_max_hp();
  was_npc=before->query_npc();
  weight= ((old_con+old_level) / 8)+1;
  if(weight<0) weight=1;
  if(weight>6) weight=6; /* Just in case...*/
}

query_i_was(){
  return before;
}

query_old_gender()
{
 return old_gender;
}

query_old_name(){
  return old_name;
}

query_old_real_name(){
  return old_name;
}

query_old_dex() {
  return old_dex;
}

query_old_int() {
  return old_int;
}

query_old_con() {
  return old_con;
}

query_old_str() {
  return old_dex;
}

query_old_exp() {
  return old_exp;
}

query_old_level(){
  return old_level;
}

query_old_maxhp(){
 return old_maxhp;
}

query_was_npc(){
 return was_npc;
}

query_killed_by(){
  return killed_by_who;
}

query_killers_name(){
  return killers_name;
}

query_killers_real_name(){
  return killers_real_name;
}

