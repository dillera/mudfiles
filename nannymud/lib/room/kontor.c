/* Rewritten using add and set functions in room.c. Banshee 950217 */
/* put back the cloning of the bailiff and added a few newlines to
 * the long description. /Profezzorn 950601 */

#pragma strict_types
#pragma save_type

#define SKATTMAS "/players/profezzorn/skattmas"

inherit "/room/room"; 

mapping scores=([ ]);
object skattmas;

void create() {
  if(dest_dir && sizeof(dest_dir)) return 0;
  restore_object("etc/kontor");
  dest_dir=({});
  item_commands=({});
  blocked_exits=({});
  items=({});
  add_exit("/room/new/narr_alley2","north");
  add_exit("/room/registration_office", "east", 0, "stop_mas");
  set_short("IRS");
  set_long(sprintf("%-=78s\n",
	   "This is the long awaited Bailiff's office of Nannymud. Here " +
	   "money, generously and enthusiasticly given by the " +
	   "inhabitants of Nannymud is stored and used in well needed " +
	   "charity-work, like well... like paying the Bailiff and " +
	   "his guards. There is a list here.\n" ));
  add_item("list","A list of the top ten contributers");
  set_light(2);
  set_indoors();
}

void restart() {
  if (!skattmas)return;
  tell_room(this_object(),"The Bailiff enters from his office.\n" );
  move_object(skattmas,this_object());
}

void reset(int arg) {
  if (!arg) create();
  if (!skattmas)
  {
    catch(skattmas=clone_object(SKATTMAS)); 
    restart();
  }
  
  /* Fake more frequent resets / Profezzorn */
  remove_call_out("reset");
  remove_call_out("reset");
  call_out("reset",40*60,1);
}

string snott(object victim,int amount)
{
  scores[victim->query_name()]+=amount;
  save_object("etc/kontor");
  catch(log_file("BAILIFF",
	   time() + " " + 
	   victim -> query_real_name() +
	   " " + amount + "\n"));
  return "He he.";
}

string tomm() {
  if (previous_object()!=skattmas) return (string)0;
  skattmas->add_money((int)skattmas->query_money()*-1);
  tell_room(this_object(),"The Bailiff throws some money into his office.\n" );
  if (skattmas->query_attack()) {
    move_object(skattmas,"/room/kontor2");
    call_out( "restart",100+random(400));
    tell_room(this_object(),"The bailiff leaves into his office.\n"); }
  return "He he";
}

int read_list(string str) {
  int c, c1;
  mapping top_scores;

  /* Yes, it should be notify_fail() and return 0, but that bugs out
     in room.c with a too long eval. Until I have had time and fixed
     that bug, this horror factor patch is here...
     Brom 950706
     */
  if (!str || str != "list") {
    write("Try 'read list', please.\n");
    return 1;
  }
  top_scores=mkmapping(m_values(scores),m_indices(scores));
  write("Ten largest contributions:\n");
  for (c=m_sizeof(top_scores)-1;c>=0 &&
       c>=m_sizeof(top_scores)-10;c--)
    write(sprintf("%-14s: %8d gc.\n", 
		  m_values(top_scores)[c],
		  m_indices(top_scores)[c]));
  
  if (this_player())
    write(sprintf("...\n%-14s: %8d gc.\n",
		  "Yours", 
		  scores[this_player() -> query_name()]));
  return 1;
}    

void init() {
  add_action("read_list", "read");
  :: init();
}

int stop_mas()
{
  if (this_player()==skattmas) return 1;
  return 0;
}

int clean_up(int refcount) /*Changed by Qqqq, since pragma_types wont load it otherwise. */
{ return 0; }
