/* /obj/bboard.c - Generic bulletin board object */

#define ARCHIVEROOM "room/inner4"
  
static string new_hd, new_body;
static object who;

string messages="";
int num_messages;
string name;
int write_level;
int read_level;

id(str)
{
  return str == "bulletin board" || str == "board" ||
    str == "bulletinboard"  || str == name;
}

long()
{
  write("You can set up new notes with the command 'note headline'.\n");
  write("Read a note with 'read num', reply to a note with 'reply num',\n");
  write("remove an old note with 'remove num', and list headers with\n");
  write("'head' or 'exa board'.\n");
  if(this_player()->query_level()>21)
    write("23++ can also 'nail/unnail num'.\n");
				  
  if(check_valid_read())
  {
    if (num_messages == 0)
    {
      write("It is empty.\n");
      return;
    }
    write("The bulletin board contains " + num_messages);
    if (num_messages == 1)
      write(" note:\n\n");
    else
      write(" notes:\n\n");
    say(call_other(this_player(), "query_name") +
	" studies the bulletin board.\n");
    headers();
  }
  return 1;
}

short() {
    return ("a bulletin board with " + num_messages + " notes");
}

get() {
    write("It is secured to the ground.\n");
    return 0;
}

init()
{
  add_action("nail", "nail");
  add_action("unnail", "unnail");
  add_action("new", "note");
  add_action("read", "read");
  add_action("mread", "mread");
  add_action("reply", "reply");
  add_action("remove", "remove");
  add_action("headers", "head");
  add_action("archive", "archive");
}

reset(arg)
{
  if (arg || previous_object())
    return;

  write_level = -1;
  read_level = -1;
  name = 0;
  call_other(ARCHIVEROOM, "short"); /* Just to make sure it is loaded */
}

set_write_level(lev)
{
  /* May only be set once - or changed by archies */
  if (write_level != -1)
    if (!this_player(1) || this_player(1)->query_level() < 25)
      return 0;
  
  write_level = lev;
  return 1;
}

set_read_level(lev) {
  if(read_level != -1) /* May only be set once - or changed by archies */
    if(!this_player(1) || this_player(1)->query_level() < 25)
      return 0;
  read_level = lev;
  return 1;
}

check_valid_read()
{
  if(!this_player() ||
     !interactive(this_player()) ||
     this_player()->query_level() < read_level)
  {
    write("You may not read on this board.\n");
    return 0;
  }else{
    return 1;
  }
}

set_name(n)
{
  if(name)  return 0;
  name = n;
  restore_object(name);
}

headers()
{
  string hd, body, rest,by;
  int i, tmp;
  if(!check_valid_read()) return 1;
  i = 0;
  rest = messages;
  while(rest != 0 && rest != "")
  {
    i++;
    tmp = sscanf(rest, "%s:\n**\n%s\n**\n%s", hd, body, rest);
    if (tmp<2)
    {
      write("Corrupt.\n");
      return;
    }
    by=0;
    sscanf(hd,"%s\n(nailed by %s)",hd,by);
    if(by)
      write(i + ":\t" + hd + " [secured]\n");
    else
      write(i + ":\t" + hd + "\n");
  }
  return 1;
}

static display(msg)
{
  string line, rest;
  int i;

  i = 1;
  while (sscanf(msg, "%s\n%s", line, rest) > 0)
  {
    if (i < 10)
      write(" ");
    write(i+":"+line+"\n");
    i++;
    msg = rest;
  }
}

new(hd)
{
  if (!hd)
    return 0;

  if (!this_player() ||
      !interactive(this_player()) ||
      this_player()->query_level() < write_level)
  {
    write("You are not allowed to write notes on this board.\n");
    return 1;
  }

  if (who && environment(who) == environment(this_object()))
  {
    write(call_other(who, "query_name") + " is busy writing.\n");
    return 1;
  }

  if (num_messages == 30)
  {
    write("You have to remove an old message first.\n");
    return 1;
  }

  if (strlen(hd) > 50)
  {
    write("Too long header to fit the paper.\n");
    return 1;
  }
  new_hd = hd;
  who = this_player();
  input_to("get_body");
  write("Give message, and terminate with '**', or '>?' for help.\n");
  write("]");
  new_body = "";
  return 1;
}

static get_body(str)
{
  string filename;
  
  if (str == ">H" || str == ">h" || str == ">?")
  {
    write("** Commands available while writing notes:\n\n");
    if (this_player()->query_level() >= 21)
      write("   >I file ....... Insert a file into this note\n");
    write("   >H, >? ........ Display this help message\n");
    write("   >L ............ Display the note\n");
    write("   >Q ............ Abort writing the note\n");
    write("   ** ............ Put the note on the bulletin board\n");
  }else if (this_player()->query_level() >= 21 && 
	    (sscanf(str,">I %s",filename) || sscanf(str,">i %s",filename))
	    && this_player()->query_valid_read(filename))
  {
    filename=this_player()->query_wiztool()->mk_path(filename);
    write("Reading "+filename+".\n");
    filename=read_file(filename);
    if(!filename)
      write("Couldn't read file.\n");
    else
      get_body(filename);
  }else if (str == ">L" || str == ">l") {
    write("** Subject: "+new_hd+"\n");
    display(new_body);
  }else if (str == ">Q" || str == ">q") {
    write("** Aborted.\n");
    who = 0;
    return 1;
  }else if (str == "**") {
    inote(new_hd, new_body);

    say(capitalize(this_player()->query_name()) +
	" just attached a note to the bulletin board.\n");
    write("Ok.\n");
    who = 0;
    return 1;
  }else{
    new_body = new_body + str + "\n";
  }

  write("]");
  input_to("get_body");
}

static nomask save() { save_object(name); }

inote(new_hd, new_body)
{
  if(previous_object()!=this_object() &&
     previous_object()!=this_player(1) &&
     file_name(previous_object())[0..10]!="obj/bboard#")
    return;

  new_hd = new_hd + " (" + capitalize(this_player()->query_real_name()) +
    ", " + extract(ctime(time()), 4, 9) + ")";
  messages = messages + new_hd + ":\n**\n" + new_body + "\n**\n";
  num_messages ++;
  new_body = 0;
  new_hd = 0;
  save();
  return 1;
}


reply(str)
{
  string hd, wd, body, rest, rest2;
  int i, tmp;

  if (!this_player() ||
      !interactive(this_player()) ||
      this_player()->query_level() < write_level)
  {
    write("You are not allowed to write notes on this board.\n");
    return 1;
  }

  if (str == 0 || (sscanf(str, "%d", i) != 1 &&
		   sscanf(str, "note %d", i) != 1))
    return 0;

  if (i < 1 || i > num_messages)
  {
    write("Not that many number of messages.\n");
    return 1;
  }
  
  rest = messages;

  while(rest != 0 && rest != "")
  {
    i--;
    tmp = sscanf(rest, "%s:\n**\n%s\n**\n%s", rest2, body, rest);
    if (tmp <2)
    {
      write("Corrupt.\n");
      return;
    }
    sscanf(rest2,"%s\n",rest2);
    if (i == 0)
    {
      if (sscanf(rest2, "%s (%s)", hd, wd) != 2)
      {
	if (sscanf(rest2, "%s(%s)", hd, wd) != 2)
	{
	  write("FOOO!\n");
	  return;
	}
      }
      write("** Replying to: "+hd+" ("+wd+")\n");
      sscanf(hd, "Re:%s", hd);
      new("Re:"+hd);
      return 1;
    }
  }
  write("Hm. This should not happen.\n");
}

#define MORE(X) this_player()->simplemore(X)

read(str,noflush)
{
  string hd, body, rest, *notes,by;
  int i, tmp;
  if(!str) return notify_fail("Read what?\n"),0;

  if(!check_valid_read()) return 0;

  if(stringp(str))
  {
    if((notes = explode(str, " ")) && sizeof(notes) > 1)
    {
      map_array(notes, "read", this_object(), 1);
      this_player()->flush();
      return 1;
    }
    tmp=100000000;
    if(sscanf(str,"%s-",i) && sscanf(str,"%d-%d",i,tmp))
    {
      if(i<0)
      {
	tmp=-i;
	i=1;
      }
      for(;i<=tmp && i<=num_messages;i++)
	read(i,1);
      this_player()->flush();
      return 1;
    }
  }
  
  if (intp(str)) i=str;
  else if ((sscanf(str, "%d", i) != 1 &&
	    sscanf(str, "note %d", i) != 1))
    return 0;
  if (i < 1 || i > num_messages) {
    write("Not that many number of messages.\n");
    return 1;
  }
  rest = messages;
  while(rest != 0 && rest != "")
  {
    i --;
    tmp = sscanf(rest, "%s:\n**\n%s\n**\n%s", hd, body, rest);
    if (tmp != 2 && tmp != 3) {
      write("Corrupt.\n");
      return;
    }
    by=0;
    sscanf(hd,"%s\n(nailed by %s)",hd,by);

    if (i == 0) {
      /*
	 say(call_other(this_player(), "query_name") +
	 " reads a note titled '" + hd + "'.\n");
	 */
      MORE("The note is titled '" + hd + "':\n\n");
      if(by)
      {
	MORE("It was nailed to the board by "+capitalize(by)+".\n");
      }
      MORE(body);
      if(!noflush) this_player()->flush();
      return 1;
    }
  }
  write("Hm. This should not happen.\n");
  return 1;
}

static archive(str)
{
  string hd, body, rest;
  int i, tmp;

  if (str == 0 || (sscanf(str, "%d", i) != 1 &&
		   sscanf(str, "note %d", i) != 1))
    return 0;
  if (i < 1 || i > num_messages)
  {
    write("Not that many number of messages.\n");
    return 1;
  }
  if(!this_player() ||
     !interactive(this_player()) ||
     this_player()->query_level() < 25)
  {
    write("Only archwizards may archive notes!\n");
    return 1;
  }
  rest = messages;
  while(rest != 0 && rest != "")
  {
    i --;
    tmp = sscanf(rest, "%s:\n**\n%s\n**\n%s", hd, body, rest);
    if (tmp<2)
    {
      write("Corrupt.\n");
      return;
    }
    sscanf(hd,"%s\n",hd);
    if (i == 0)
    {
      object bobj;
	  
      bobj = present("board", find_object(ARCHIVEROOM));
      if (!bobj)
      {
	write("Error. Could not find the board for old notes.\n");
	return 1;
      }
	  
      bobj->inote(hd, body);
      return 1;
    }
  }
  write("Hm. This should not happen.\n");
}

static remove(str)
{
  string hd, body, rest,by;
  int i, tmp;

  if(this_player()!=this_player(1) ||
     !this_player() ||
     !interactive(this_player()) ||
     this_player()->query_level()<write_level)
    return 0;

  if (str == 0 || (sscanf(str, "%d", i) != 1 &&
		   sscanf(str, "note %d", i) != 1))
    return 0;

  if (i < 0)
  {
    while (i < 0)
    {
      remove("1");
      i += 1;
    }
    return 1;
  }
    
  if (i < 1 || i > num_messages) {
    write("Not that number of messages.\n");
    return 1;
  }
  rest = messages;
  messages="";
  while(rest != 0 && rest != "")
  {
    i --;
    tmp = sscanf(rest, "%s:\n**\n%s\n**\n%s", hd, body, rest);
    if(sscanf(hd,"%s\n(nailed by %s)\n",hd,by))
    {
      if(this_player(1)->query_level()<=21)
      {
	write("You can't remove it, it's nailed to the board.\n");
	messages = messages + hd + ":\n**\n" + body + "\n**\n"+rest;
	return 1;
      }else{
	say(this_player()->query_name()+" produces a pair of pincers,\n");
      }
    }
    if (tmp<1)
    {
      write("Corrupt.\n");
      return;
    }
    if (i == 0) {
      say(call_other(this_player(), "query_name") +
	  " removed a note titled '" + hd + "'.\n");
      log_file("BBOARD-REMOVE", file_name(this_object()) + ": " +
	       (string) this_player()->query_real_name() +
	       " removed title '" + hd + "'\n");
      write("Ok.\n");
      messages = messages + rest;
      num_messages --;
      save();
      return 1;
    }
    messages = messages + hd + ":\n**\n" + body + "\n**\n";
  }
  write("Hm. This should not happen.\n");
}

static nail(str)
{
  string hd, body, rest;
  int i, tmp;

  if(this_player()!=this_player(1) ||
     !this_player() ||
     !interactive(this_player()) ||
     this_player()->query_level()<write_level)
    return 0;
  if(this_player(1)->query_level()<=21)
    return 0;

  if (str == 0 || (sscanf(str, "%d", i) != 1 &&
		   sscanf(str, "note %d", i) != 1))
    return 0;

  if (i < 1 || i > num_messages)
  {
    write("Not that number of messages.\n");
    return 1;
  }
  rest = messages;
  messages="";
  while(rest != 0 && rest != "")
  {
    i--;
    tmp = sscanf(rest, "%s:\n**\n%s\n**\n%s", hd, body, rest);
    if (tmp <2) {
      write("Corrupt.\n");
      return;
    }
    if (!i)
    {
      if(sscanf(hd,"%*s\n(nailed by %*s\n"))
      {
	write("It is already nailed to the board.\n");
	messages = messages + hd + ":\n**\n" + body + "\n**\n"+rest;
	return 1;
      }
      
      say(this_player()->query_name() +
	  " produces a hammer and secures a note\ntitled '" + hd + "' to the board with soem nails.\n");

      write("Ok.\n");
      messages = messages + hd + "\n(nailed by "+
	this_player()->query_real_name()+")"+ ":\n**\n" + body + "\n**\n"+rest;

      save();
      return 1;
    }
    messages = messages + hd + ":\n**\n" + body + "\n**\n";
  }
  write("Hm. This should not happen.\n");
}

static unnail(str)
{
  string hd, body, rest;
  int i, tmp;

  if(this_player()!=this_player(1) ||
     !this_player() ||
     !interactive(this_player()) ||
     this_player()->query_level()<write_level)
    return 0;
  if(this_player(1)->query_level()<=21)
    return 0;

  if (str == 0 || (sscanf(str, "%d", i) != 1 &&
		   sscanf(str, "note %d", i) != 1))
    return 0;

  if (i < 1 || i > num_messages)
  {
    write("Not that number of messages.\n");
    return 1;
  }
  rest = messages;
  messages="";
  while(rest != 0 && rest != "")
  {
    i--;
    tmp = sscanf(rest, "%s:\n**\n%s\n**\n%s", hd, body, rest);
    if (tmp <2) {
      write("Corrupt.\n");
      return;
    }
    if (!i)
    {
      if(!sscanf(hd,"%s\n(nailed by",hd))
      {
	write("That isn't nailed to the board.\n");
	messages = messages + hd + ":\n**\n" + body + "\n**\n"+rest;
	return 1;
      }
      
      say(this_player()->query_name() +
	  " produces a pair of pincers and removes some nails from the board.\n");

      write("Ok.\n");
      messages = messages + hd + ":\n**\n" + body + "\n**\n"+rest;

      save();
      return 1;
    }
    messages = messages + hd + ":\n**\n" + body + "\n**\n";
  }
  write("Hm. This should not happen.\n");
}

mread(str)
{
  int i;
  object ob;
  if (!str) 
    str = 1;
  if(check_valid_read()) {
    if(intp(str)) i = str;
    if (!i && (sscanf(str, "%d", i) != 1 &&
	       sscanf(str, "note %d", i) != 1))
      return 0;
    if (i < 1 || i > num_messages) {
      write("Not that many number of messages.\n");
      return 1;
    }
    ob = clone_object("obj/board_reader");
    move_object(ob,this_player());
    ob->start_read(i);
  }
  return 1;
}

query_num_mess()
{
  if(check_valid_read())
    return num_messages;
  else
    return 0;
}

/* By Padrone on request from Wing, May 1 1992: */
query_prevent_shadow(ob) {

    log_file("BBOARD-TEMP-LOG", ctime(time()) + ":");
    log_file("BBOARD-TEMP-LOG", file_name(this_object()));
    if (previous_object())
      log_file("BBOARD-TEMP-LOG", " : " + file_name(previous_object()));
    if (this_player())
      log_file("BBOARD-TEMP-LOG", " : " + this_player()->query_name());
    log_file("BBOARD-TEMP-LOG", "\n");

    return 1;
} /* query_prevent_shadow */
