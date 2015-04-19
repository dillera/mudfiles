/* registration_office.c
 *
 * The registration office.
 * Written by Brom to make registration of 2nds automatic.
 *
 * Functions for your use (level 21 and 22 wizards):
 * query_is_second(who)    If 'who' is a 2nd, return 1 else 0.
 * query_has_seconds(who)  If 'who' has seconds, return 1 else 0.
 * query_aliases()         Returns an empty mapping, ([]).
 *
 * For 23++:
 * query_is_second(who)    If 'who' is a 2nd, return the name of the wizard, else 0.
 * query_has_seconds(who)  Returns what 2nds 'who' has.
 * query_aliases()         Returns the database.
 * 
 */

static inherit "std/room";

#define FINGERD "obj/daemon/fingerd"
#define SAVE_FILE "etc/SAVES/wiz_aliases"
#define LOG_FILE  "etc/SAVES/alias_log"
#define LB(X) sprintf("%-=78s\n", X)

static status setup_done;
static string wiz_name = "";
static string mortal_name = "";
static string help_text = 0;
static string type;
static string *tmp_al;
static int tos;
static int time_stamp;
mapping aliases = ([]);
string password, testchar, secondchar;
int level;

reset(arg)
{
  if (!arg)
  {
    if (!setup_done) set_up();
    help_text =
      ("+-------------------------------------------------------------------+\n"+
       "|             Welcome to the registration office!                   |\n"+
       "|                                                                   |\n"+
       "| Wizards can register their second characters and test characters  |\n"+
       "| here. Just do 'register test|2nd name'. Only existing players can |\n"+
       "| be registered. You will be prompted for the players password.     |\n"+
       "|                                                                   |\n"+
       "+-------------------------------------------------------------------+\n"+
       "                              .....");
    add_property("indoors");
    add_property("no_clean_up");
    set_light(1);
    set_short("The registration office");
    set_long(LB("The registration office where you can register your test-characters "+
		"and seconds. It is a cool room with bare marble walls and a very high "+
		"ceiling. Light filters in through the painted glass panes set into "+
		"the walls up there. There is a presence here, and you can imagine it "+
		"listening, waiting...\n"+
		"There's also a sign on the wall."));
    add_exit("west", "/room/kontor");

    add_item( ({ "wall", "walls" }),
	     "Ah. Smooth, cool green marble.\n");
    add_item("marble",
	     "It is green marble, imported from a far away land.\n");
    add_item("ceiling", "It is very high.\n");
    add_item( ({ "glass", "glass panes", "panes", "windows" }),
	     "The windows are set high up on the walls. The panes have been\n"+
	     "painted in a chaos of colours.\n");
    add_item("sign", help_text);
    add_command("read %s", "@cmd_read()");
  }

  /* Sometimes, 2nds have been rm'd just like ordinary characters.
   * We have a clean up function checking this; it should not be run
   * the first time this object is loaded, since that would add to the
   * lag right after a reboot.
   */
  if (arg)
  {
    if (!random(10))
      start_check_data();
  }
} /* reset */

nomask set_up()
{
  if (catch(restore_object(SAVE_FILE)))
    aliases = ([]);
  setup_done = 1;
} /* set_up */

cmd_read(foo, str)
{
  if (sscanf(str, "%*ssign%*s") == 2)
  {
    this_player()->simplemore(help_text+".\n");
    this_player()->flush();
    return 1;
  }
} /* cmd_read */

init()
{
  ::init();
  add_action("cmd_register", "register", 1);
}

static nomask cmd_register(str) {
  int lvl;
  
  wiz_name = this_player(1) -> query_real_name();
  wiz_name = lower_case(wiz_name);
  lvl = this_player(1) -> query_level();
  

  if (this_player(1)->query_level() < 20)
  {
    write("Only wizards may register second characters and test caracters.\n");
    return 1;
  }

  if ((time() - time_stamp) < 60)
  {
    write("Due to security, there must be at least 60 seconds between\n"+
	  "registration attempts.\n");
    return 1; 
  }

  if (!str || sscanf(str, "%s %s", type, mortal_name)!=2)
  {
    write("Syntax: register 2nd|test name\n");
    return 1;
  }

  mortal_name = lower_case(mortal_name);
  type = lower_case(type);
  if (sscanf(type, "%*stest%*s")==2) tos=1;
  if (sscanf(type, "%*s2nd%*s")==2 ||
      sscanf(type, "%*ssecond%*s")==2) tos=2;
  if (tos==0)
  {
    write("Syntax: register 2nd|test name\n");
    return 1;
  }
  if (tos==1) type = "test"; else type = "second";

  if (mortal_name==wiz_name)
  {
    write("But that's you?\n");
    return 1;
  }
  if (!restore_object("players/" + mortal_name)) {
    write("There is no such player.\n");
    return 1;
  }
  if (level>19)
  {
    write("Wizards can not be registered as test characters or 2nds.\n");
    return 1;
  }
  tmp_al = aliases[wiz_name];
  if (testchar == wiz_name ||
      secondchar == wiz_name ||
      tmp_al && (-1<member_array(mortal_name, tmp_al)))
  {
    write("That name is already registered on you.\n");
    return 1;
  }
  if ((testchar && testchar != wiz_name) ||
      (secondchar && secondchar != wiz_name)) {
    write("That name is already registered - but not on you.\n");
    return 1;
  }
  write("Now give password for the mortal character.\n"+
	"Password: ");
  input_to("reg2", 1);
  return 1;
} /* cmd_register */

static nomask reg2(gpwd)
{
  string rpwd, old, *player_file;
  int i, s;
  object victim;

  if (!gpwd) {
    write("Empty password?\n");
    return 1;
  }

  if (crypt(gpwd, password) != password)
  {
    write("Wrong password.\n");
    /* Set back time_stamp. */
    time_stamp = time() - 60;
    return 1;
  }
  if (tmp_al) 
    tmp_al = tmp_al + ({mortal_name});
  else
    tmp_al = ({mortal_name});

  aliases[wiz_name] = tmp_al;

  victim = find_player(mortal_name);
  if (victim)
  {
    if (tos == 1)
      victim -> set_testchar();
    else
      victim -> set_secondchar();
    victim -> save_me();
  }
  else
  {
    player_file = explode(read_file("players/"+ mortal_name + ".o"), "\n");
    s = sizeof(player_file);
    for (i=0; i<s; i++)
      if (sscanf(player_file[i], type + "char %s", old)==1) break;
    
    if (old && old!="0") {
      write(capitalize(mortal_name) +
	    " is already registered as a " + type + " character.\n");
      time_stamp = time() - 60;
      return 1;
    }
    player_file[i] = type + "char \"" + wiz_name + "\"";
    if (!rm("players/" + mortal_name + ".o")) {
      write("Error when trying to replace old save file!\n"+
	    "Registration aborted!\n");
      password = testchar = secondchar = "";
      time_stamp = time() - 60;
      return 1;
    }
    write_file("players/" + mortal_name + ".o",
	       implode(player_file, "\n"));
  }
  password = testchar = secondchar = "";
  catch(save_object(SAVE_FILE));
  catch(write_file
	(LOG_FILE,
	 ctime(time()) + " " + wiz_name + " == " + mortal_name + "\n"));
  write("Registration done.\n");

  return 1;
} /* reg2 */


query_is_second(name)
{
  /* Replace this with some easy code in fingerd. */

  string *wizs;
  mixed *secs;
  int i, s;

  if (aliases == ([])) return 0;
  wizs = m_indices(aliases);
  secs = m_values(aliases);
  s = sizeof(wizs);
  name = lower_case(name);
  for (i=0; i<s; i++)
    if (member_array(name, secs[i]) != -1)
      if (interactive(this_player(1)) &&
	  this_player(1) -> query_level() > 22)
	return wizs[i];
      else
	return 1;
  return 0;
} /* query_is_second */


query_has_seconds(name) {
  if (interactive(this_player(1)) &&
      this_player(1) -> query_level() > 22)
    return aliases[lower_case(name)];
  else
    return !! aliases[lower_case(name)];
} /* query_has_seconds */


query_aliases()
{
  if (interactive(this_player(1)) &&
      this_player(1) -> query_level() > 22)
    return aliases;
  else
    return ([]);
} /* query_aliases */


start_check_data()
{
  call_out("check_data", 2, ({ m_indices(aliases), 0 }) );
} /* start_check_data */


check_data(arr)
{
  int i, index;
  string *names, *nlist;

  nlist = arr[0];
  index = arr[1];
  names = aliases[nlist[index]];
  for (i=0; i<sizeof(names); i++)
    if (!FINGERD -> playerp(names[i]))
      names[i] = 0;
  aliases[nlist[index]] = names - ({ 0 });
  index++;

  if (index < sizeof(nlist))
    call_out("check_data", 2, ({ arr[0], index }) );
  else 
  {
    password = testchar = secondchar = "";
    save_object(SAVE_FILE);
  }
} /* check_data */

_exit() 
{ 
  catch(__exit());
} /* _exit */

__exit()
{
  if (!(_isclone(this_object())))
  {
    password = testchar = secondchar = "";
    save_object(SAVE_FILE); 
  }
} /* __exit */
