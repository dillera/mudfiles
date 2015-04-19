/* This dirty hack is made by Milamber. Don't look 
 * (I will replace this with a better one eventually
 */

#define DATABASE_OBJ "players/milamber/database"
#define BOARD_OBJ "players/milamber/board"
#define fail(x) return notify_fail(x),0
#define end(x) return write(x)

#include "/players/milamber/board.h"

mapping months="players/milamber/board"->query_months();
mapping map;

string name; /* Used when there is no database... */
mapping data=([]);

set_name(a)
{
  name=a;
  init_database(a);
}

string my_ctime(int foo)
{
  int date,hour,minute,second,year;
  string month;
  
  sscanf(ctime(foo),"%*s %s %d %d:%d:%d %d",month
	 ,date,hour,minute,second,year);
  return sprintf("%d-%02d-%02d %02d:%02d:%02d",year,
		 months[lower_case(month)],date,hour,
		 minute,second);
}

void init()
{
  init_database(name);
  add_action("enter","enter"); 
  add_action("read","read");
  add_action("note","note");
  add_action("reply","reply");
  add_action("head","head");
  add_action("do_lock", "lock");
  add_action("do_lock", "nail");
  add_action("do_lock", "unlock");
  add_action("remove", "remove");
}

string smart_title(int a)
{
  int k, *com;
  string s;
  k=a;
  s="";
  if(!(com=comments_to(k)) || !sizeof(com))
    s="ERROR, Loose note: ";
  else
    while(k = comments_to(k)[0])
    {
      if(!sizeof(comments_to(k)))
      {
	write("ERROR, Loose note in comments_to:\n"+k+": "+title(k)+"\n");
	break;
      } else {
	s += ">";
      }
    }
  return (strlen(s)?s+" ":s)+title(a);
}

int *read_cache;
object read_cache_person;

void add_read(int note)
{
  string n;
  int *foo;

  if(this_player(1) != this_player())
    return;

  if(!database)
    return;
  
  n=this_player()->query_real_name();

  if(read_cache_person == this_player())
  {
    foo = read_cache + ({ note });
    read_cache = foo; /* I can't remember why I did not use +=.. */
  } else {
    foo = database->query_read(n) + ({ note });
    read_cache = foo;
    read_cache_person = this_player();
  }
  database->set_read( foo , n);
}


int unreadp(int note)
{
  if(read_cache_person != this_player())
  {
    int *old_cache;
    old_cache = read_cache;
    if(read_cache=database->query_read(this_player()->query_real_name()))
      read_cache_person=this_player();
    else
      read_cache = old_cache;
  }
  return member_array(note, read_cache) == -1;
}

int readp(int note)
{
  return !unreadp(note);
}

mapping note_cache=([]);

int enter(string a);


varargs void display_one_head(int a,int only_unread) 
{
  if(a!=0 && !deleted(a))
  {
    if(only_unread && readp(a))
      return;
    if(!note_cache[a])
      note_cache[a]=sprintf("%s (%s %s) ", smart_title(a), author(a),
			    my_ctime(made(a))[5..9]);
    write(sprintf("%d%s %s ", map[a], readp(a)?"|":":", note_cache[a]));
    if(locked(a))
      write("L");
    write("\n");
  } else if(a==0) {
    write(sprintf("%s, Created by %s %s\n%s", title(0), author(0),
		  my_ctime(made(0))[0..9],LINE));
  }
}

void fix_map(int *notes)
{
  int i;
  i=sizeof(notes);
  if(!map)
    map=([]);
  while(--i)
  {
    map[notes[i]]=i;
    map[""+i]=notes[i];
  }
}

int head(string s)
{
  int *notes,i,onlyu;
  init_database(name);
  notes=allnotes();
  fix_map(notes);
  if(s)
  {
    i=atoi(s);
    if(sscanf(s,"%*sread"))
      onlyu = 1;
  } else {
    i=sizeof(notes)-30;
    if(i<0)
      i=0;
  }
  init_database(name);
  write(LINE);
  display_one_head(0);
  if(sizeof(notes) > 1)
    map_array(notes[(1+i)..10000], "display_one_head", this_object(), onlyu);
  return 1;
}

void dct(int num)
{
  if(num)
    if(!deleted(num))
      this_player()->
      simplemore("Comment to note "+map[num]+" by "+author(num)+".\n");
  return;
}

varargs int parse_it(mixed a, int no_0);


int read(mixed a)
{
  int zoo;
  mixed b, *c, d, e;

  if(!a || !atoi(a) && a[0] != '-')
    return notify_fail("Read what?\n"), 0;

  if(!a)
    return notify_fail("Read what?\n"), 0;

  if(a == "board")
    return enter(a);

  if(sizeof(c=explode(a,",") || ({}) ) > 1)
    return map_array(c, "read", this_object()), 1;

  if(!map) 
    fix_map(allnotes());

  if(atoi(a) < 0)
  {
    b = -atoi(a);
    a = 1;
  } else if(e = sizeof((c = explode(a,"-")) || ({}) ) == 2) {
    a=atoi(c[0]);
    b=atoi(c[1]);
  } else if(a[-1]=='-') {
    a=atoi(a);
    b=m_sizeof(map)/2;
  } else {
    a=b=atoi(a);
  }

  if(!b)
    b=a;

  for(d=a;d<=b;d++)
  {
    a=parse_it(""+d);
    if(a==-1)
      continue;

    add_read(a);
    this_player()->simplemore(LINE);
    this_player()->simplemore(sprintf("%d %s /%d line%s/ %s\n",map[a],
				      my_ctime(made(a)),
	      zoo=sizeof(explode(body(a)||"\n","\n")||({})),zoo!=1?"s":"",
				      author(a)));
    map_array(comments_to(a),"dct", this_object());
    this_player()->simplemore("Title: "+title(a));
    this_player()->simplemore("\n");
    this_player()->simplemore(LINE);
    this_player()->simplemore(body(a));
    this_player()->simplemore(LINE);
  }
  this_player()->flush();

  return 1;
}

int enter(string a)
{
  object board;

  if(!a)
    return notify_fail(capitalize(query_verb())+" what? The board,"+
		       " perhaps?\n"),0;

  if(present(a,environment(this_object())) != this_object()) 
    return notify_fail(capitalize(query_verb())+" what? The board,"+
		       " perhaps?\n"),0;

  board=clone_object(BOARD_OBJ);
  board->set_database(database);
  
  if(!move_object(board,this_player()))
  {
    if(!board)
      write("** Error: The board was destructed upon entering"+
	    " your inventory.\n");
    else
      write("** Error: I cannot move the boardreader ("+
	    file_name(board)+") to you ("+
	    this_player()->query_real_name()+")!\n");
  } else {
    say(this_player()->query_name()+" starts reading from the board.\n");
  }
  return 1;
}

void end_writing()
{
  mapping foo;
  foo=data[this_player()->query_real_name()];

  foo["note"] = get_new_note();
   
  add_comment_to(foo["note"], foo["repl"]);
  add_comment(foo["repl"], foo["note"]);

  set_head(foo["note"], foo["title"], time(), 
	   capitalize(this_player()->query_real_name()), 0);
  set_body(foo["note"],foo["body"]);
  write(LINE);
  data = m_delete( data, this_player()->query_real_name() );
  map=0; /* Empty the cache */
  write("Ok.\n");
}

void input_body(string s)
{
  switch(s)
  {
   case ".":
   case "**":
    end_writing();
    return;

   case "~d":
   case "~r":
    write("----------------------------\n");
    write(data[this_player()->query_real_name()]["body"]);
    write("----------------------------\n");
    break;

   case "~q":
    write("Ok. aborted.\n");
    return;

   default:
    data[this_player()->query_real_name()]["body"] += s+"\n";
  } 
  write(". ");
  input_to("input_body");
}

void input_title(string s)
{
  if(strlen(s))
    data[this_player()->query_real_name()]["title"]=s;
  write("Input the text.\nEnd with \".\" or \"**\", or ~q to abort.\n");
  write("To display the note, use ~r.\n");
  write(LINE);
  input_to("input_body");
  write(". ");
}


void write_note()
{
  mapping dat;
  dat=data[this_player()->query_real_name()];
  if(!mappingp(dat))
    return;
  write("Title: " + dat["title"] + "\n");
  dat["body"]="";
  input_title(dat["title"]);
}

varargs int parse_it(mixed a, status no_0)
{
  init_database(name);
  
  if(!map)
    fix_map(allnotes());

  if(!a)
  {
    write(capitalize(query_verb())+" what? A note, perhaps?\n");
    return -1;
  }
  
  if(!((!no_0 && a=="0") || (a = atoi(a))))
  {
    write("I think you want to "+query_verb()+
	  " a note, like, "+query_verb()+" 1\n");
    return -1;
  }

  if(!((a=map[""+a]) && !deleted(a)))
  { 
    write("There is no such note on this board.\n");
    return -1;
  }
  return a;
}

int reply(mixed a)
{
  int zoo;
  int new_note;

  a=parse_it(a, 1);
  if(a==-1)
    return notify_fail("Reply to which note?\n"), 0;

  
  write(LINE);
  write("Replying to the note\n");
  display_one_head(a);
  write(LINE);
  data[this_player()->query_real_name()]=
    ([ 
      "title":title(a),
      "body":"",
      "repl":a,
      ]);
  write_note();
  return 1;
}

int note(mixed a)
{
  int zoo;
  int new_note;
  init_database(name);

  add_comment_to(new_note, 0);
  add_comment(0, new_note);

  write(LINE);
  data[this_player()->query_real_name()]=
    ([ 
      "title":(a?a:""),
      "body":"",
      ]);
  write_note();
  return 1;
}

int remove(mixed a)
{
  string tmp;
  init_database(name);

  a=parse_it(a,1);
  if(a==-1)
    return 0;
  
  if(delete_note(a))
    write("That note is locked!\n");
  else
    write("Ok.\n");
  
  fix_map(allnotes());
  return 1;
}

int do_lock(mixed a)
{
  int zoo;
  init_database(name);

  a=parse_it(a, 1);
  if(a==-1)
    return -1;
  
  if(zoo=((query_verb() == "lock")?(lock(a)):(unlock(a))))
  {
    if(zoo==3)
      write("There is no such note.\n");
    else
      write("The note is locked by someone with a higher level than you.\n");
    return 1;
  }
  write("Ok.\n");
  return 1;
}

varargs string number(int i, string f00)
{
  if(i<0)
    return "no";
  return (string)"/obj/support"->number_to_string(i);
}

short() 
{ 
  int i, j;
  string s;
  init_database(name);
  s=("a bulletin board with "+number(i=(sizeof(allnotes())-1))+
     " note"+(i==1?"":"s")+" on it. ");
  
  if(i<1)
    return "an empty bulletin board";

  j=sizeof(filter_array(allnotes(), "unreadp", this_object()));
  if(!j)
    s += "You have read them all before";
  else if(j == i)
    s += "They are all new to you";
  else if(j==1)
    s += capitalize(number(j)) + " is new to you";
  else
    s += capitalize(number(j)) + " are new to you";
  
  return s;
}

long()  { head(); }

get()   { return 0; }

id(s)   { return s=="board"; }



