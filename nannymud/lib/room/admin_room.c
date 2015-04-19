/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "room/room";

#define  BLANKS  "                                          "

reset(arg) {
  if(arg) return;
  set_light(1);
  set_short("The admin room");
  set_long("The administration room. In this room you can get\n" +
	   "some info about the game. Available commands are:\n" +
	   "debug, rusage, callinfo and Trace.\n");
  add_exit("room/inner3", "north");
}

init() {
   ::init();
   add_action("debug", "debug");
/*   add_action("r_usage", "rusage"); */
   add_action("call_info", "callinfo");
   add_action("trace_fun", "Trace");
}

move() {
  if(this_player()->query_level() < 22) {
    this_player()->move_player("north#room/adv_guild");
    return 1; }
  return ::move();
}

debug(str) {
  object ob;
  if(!str) {
    write("Usage: debug <object>\n");
    return 1; }
  ob = present(str, this_player());
  if(!ob)
    ob = present(str, environment(this_player()));
  if(!ob)
    ob = find_living(str);
  if(!ob) 
    ob = find_object(str);
  if(!ob)
    write("Couldn't find " + str + ".\n");
  else
    debug_info(0,ob);
  return 1;
}

#if 0
r_usage() {
  write(rusage() + "\n");
  return 1;
}
#endif

trace_fun(arg) {
  int t;
  if(sscanf(arg, "%d", t) != 1) {
    write("Usage: Trace <nr>\n");
    return 1; }
  write("You changed your trace level from ");
  trace(t);
  write(" to " + t + ".\n");
  return 1;
}

trace_pre(str) {
  if(!str)
    write("Usage: Tracepre <str>\n");
  else
    traceprefix(str);
  return 1;
}

call_info(arg) {
  mixed *calls, *al, more;
  string name, temp;
  int i, j, antal;
  calls = call_out_info();
  if(!calls || !sizeof(calls)) {
    write("No call_outs found.\n");
    return 1; }
  if(!arg || arg == "")
    arg = this_player()->query_real_name();
  arg = lower_case(arg);
  antal = 0;
  if(arg == "stats") {
    al = order_alist( ({ }), ({ }) );
    for(i = 0; i < sizeof(calls); i++) {
      if(calls[i] && calls[i][0]) {
	name = creator(calls[i][0]);
	if(!name)
	  name = "!creator";
	antal++;
	j = assoc(name, al[0]);
	if(j < 0)
	  al = order_alist(al[0] + ({ name }), al[1] + ({ ({ 1, calls[i][2] }) }));
	else
	  {
	    al[1][j][0]++;
	    al[1][j][1] += calls[i][2];
	  }
      }
    }
    write("  Name          Calls  Average  Percent\n");
    write("---------------------------------------\n");
    for(i = 0; i < sizeof(al[0]); i++)
      {
	write(extract(al[0][i] + BLANKS, 0, 15));
	temp = BLANKS + al[1][i][0];
	write(extract(temp, strlen(temp) - 4) + "   ");
	j = al[1][i][1] * 10 / al[1][i][0];
	temp = BLANKS + (j / 10) + "." + (j % 10);
	write(extract(temp, strlen(temp) - 6) + "   ");
	j = al[1][i][0] * 1000 / antal;
	temp = BLANKS + (j / 10) + "." + (j % 10);
	write(extract(temp, strlen(temp) - 6) + "\n");
      } 
    write("---------------------------------------\n");
    write("        Totally: " + antal + " call_outs.\n");
    return 1;
  }
  al = ({ });
  for(i = 0; i < sizeof(calls); i++)
    {
      if(!calls[i] || !calls[i][0])
	continue;
      name = creator(calls[i][0]);
      name = name + "";
      if(arg != "all" && arg != name)
	continue;
      al += ({ calls[i] });
    }
  if(!al || !sizeof(al))
    {
      write("No call_outs found for " + arg + ".\n");
      return 1;
    }
  write("Object                              Function              Time  Arg\n");
  write("--------------------------------------------------------------------------\n");
  for(i = 0; i < sizeof(al); i++)
    {
      if(sizeof(al[i]) < 4)
	al[i] += ({ 0 });
      temp = BLANKS + al[i][2];
      if(stringp(al[i][3]))
	more = al[i][3];
      else if(intp(al[i][3]))
	more = al[i][3] + "";
      else if(objectp(al[i][3]))
	more = short_file_name(file_name(al[i][3]));
      else if(mappingp(al[i][3]))
	more = "<MAPPING>";
      else if(pointerp(al[i][3]))
	more = "<ARRAY>";
      else
	more = "<UNKNOWN>";
      write(extract(short_file_name(file_name(al[i][0])) + BLANKS, 0, 34) + " " +
            extract(al[i][1] + BLANKS, 0, 20) + " " +
            extract(temp, strlen(temp) - 4) + "  " +
            extract(more, 0, 13) + "\n");
    }
  write("--------------------------------------------------------------------------\n");
  write(sizeof(al) + " call_outs found for " + capitalize(arg) + ".\n");
  return 1;
}

short_file_name(arg) {
  string temp, dummy;
  if(sscanf(arg, "players/" + this_player()->query_real_name() + "/%s", dummy) == 1)
    return dummy;
  if(sscanf(arg, "players/%s", dummy) == 1)
    return "~" + dummy;
  return arg;
}
