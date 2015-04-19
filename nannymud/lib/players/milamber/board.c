#include "/players/milamber/board.h"

#define NOCOMM 0
#define W_HEAD 1
#define W_BODY 2

string name;
int *m_notes=({ 0 });
int state,c_note,qread=1,a,display_all,q;
int *r_notes;
string dbn="none";

string my_line_break(string s, int len)
{
  s=replace(s,"\n\n", "<p>");
  s=replace(replace(s,"<p>",  "\n\n"),"<br>", "\n");
  s=replace(replace(s,"<f>",  "\n      /"),"&lt;", "<");
  s=replace(replace(replace(s,"&ae;", "�"),"&aa;", "�"),"&gt;", ">");
  s=replace(replace(replace(s,"&Ae;", "�"),"&Aa;", "�"),"&oe;", "�");

  return sprintf("%-=*s\n",len,replace(s,"&Oe;", "�"));
}


mapping months=(mapping)"players/milamber/board"->query_months();
mapping query_months()
{
  return months || 
    (months=
     ([
       "jan":1, "feb":2,  "mar":3,  "apr":4,  "may":5,  "jun":6,
       "jul":7, "aug":8,  "oct":10,  "sep":9, "nov":11, "dec":12,
       ]));
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

string short() { return "a boardreader"; }
object query_database() { return database; }
status id(string s) { return (s=="board") || (s=="boardreader"); }

void long()
{
  database->long();
  write(LINE);
}

void save_and_exit() 
{
  if(database)
    catch(database->set_read(r_notes & allnotes(), name));
  destruct(this_object());
}

string rhead(int a)
{
  string res;
  int zoo;
  res="";
  if(deleted(a))
    return "That note is deleted, or there is no such note.\n";
  res+=sprintf("%d %s /%d line%s/ %s\n",a,my_ctime(made(a)),
	       zoo=sizeof(explode(body(a)||"\n","\n")||({})),zoo!=1?"s":"",
	       author( a ));
  res+=implode(map_array(comments_to(c_note),"dct", this_object()),"\n");
  res+="Title: "+title( a );
  return res+"\n";
}

void dhead(int a)  { write(rhead(a)); }

void dshead(int a) 
{
  if(!deleted(a) && a!=0)
  {
    write(sprintf("%d: %s (%s %s) ",a,title(a),author(a),
		  my_ctime(made( a ))[5..9]));
    if(locked(a))
      write("*");
    write("\n");
  } else if(a==0) {
    write(sprintf("%s, Created by %s %s\n%s",title(0),author(0),
		  my_ctime(made(0))[0..9],LINE));
  }
}

string *more_text=({});

varargs int *deep_comments(int start_note)
{
  int j,k;
  int *res,*c;
  res=({ });
  j=sizeof(c=comments(start_note));
  for(k=0;k<j;k++)
  {
    if( c[ k ] != start_note )
      res += deep_comments(c[k]);
    else
      database -> error(c[k]+" is linked to "+c[k]);
  }
  if( member_array( start_note, res ) != -1 )
    return ({ }); /* Auch. */
  return ({ start_note })  + res;
}

varargs object set_database(object o)
{ 
  database = o;
  dbn=(string)o->query_name();
  name=capitalize((string)this_player()->query_real_name());
  r_notes=database->query_read(name) + ({ 0 });
  r_notes = r_notes & r_notes;
  m_notes=deep_comments(0) - r_notes - ({ 0 });
}


varargs int f_next_note(int nomod);

string _title, *_body;
int _note;
int _editing;

void edit(string titl, string bod, int note)
{
  sscanf(titl, "%s <Edited>", titl);
  sscanf(titl, "%s <Unifinished>", titl);
  
  _title=titl;
  _body=explode(bod,"\n")||({});
  _note=note;
  _editing=-1;
  input_to("_edit");
  write(LINE);
  write("Edit. Press return for the default (In []'s)\n");
  write("'.' or '**' to end. `q' to quit.\n");
  write("'i' will insert a line before the current one. (y)\n");
  write("'d' will delete the current line.              (k)\n");
  write("'s/from/to will replace all from with to.      (r)\n");
  write("'g{num}' will move you to num. (num must be less than 10000).\n");
  write(LINE);
  write("Title: ["+_title+"]\n");
}

void prompt();
string _buff;

void _edit(string s)
{
  if(!database)
    database=METADATABASE->query_database( dbn );

  if(s=="**" || s==".")
  {
    write(LINE);
    while(sizeof(_body) && _body[-1]=="")
      _body=(_body[0..sizeof(_body)-2]);
    write("Ok.\n");
    set_body(_note, implode(_body,"\n")+"\n");
    set_title(_note, _title);
    input_to("receive");
    if( _note )
      r_notes -= ({ _note }); /* Lets read it again */
    prompt();
    _note=_body=_title=_editing=0;
    return;
  } else if(s=="q" || s=="~q") {
    write(LINE);
    write("Ok. Change cancelled.\n");
    prompt();
    _note=_body=_title=_editing=0;
    input_to("receive");
    return;
  } else if(s[0]=='s') {
    string a,*b;
    s=s[1..10000];
    a=s[0..0];
    b=explode(s,a);
    b+=({"",""});
    if(_editing!=-1)
      _body[_editing]=replace(_body[_editing],b[0],b[1]);
    else
      _title=replace(_title,b[0],b[1]);
    write(sprintf("Ok. \"%s\" replaced with \"%s\"\n",b[0],b[1]));
    _editing--;
  } else if(s=="d" || s=="k") {
    if(_editing == -1)
    {
      _editing--;
      write("You cannot erase the title!\n");
    } else {
      write("Ok. Line deleted.\n");
      _buff=_body[_editing];
      _body=_body[0.._editing-1]+_body[_editing+1..10000];
      while(sizeof(_body) <= _editing)
	_editing--;
      _editing--;
    }
  } else if (s=="i" || s=="y") {
    if(_editing == -1)
    {
      write("The title may only have one line.\n");
      _editing--;
    } else {
      _body=_body[0.._editing-1]+ ({ _buff||"" }) +_body[_editing..10000];
      write("Ok. Line inserted before this one.\n");
      _editing--;
    }
  } else if (sscanf(s,"g%d",_editing)) {
    write("Ok. Going to line "+_editing+".\n");
    while(sizeof(_body)<_editing)
      _body += ({ "" });
    _editing--;
  } else if(strlen(s)) {
    if(_editing < 0)
      _title=s;
     else 
      _body[ _editing ] = s;
  }  
  _editing++;
  if(_editing == -1) /* Yes, it's possible */
  {
    write("Title: " + _title + "\n");
    input_to("_edit");
    return;
  }
  if(!_editing)
    write(LINE);

  if(sizeof(_body)==_editing)
    _body += ({ "" });
  write(sprintf("%3d: [%s]\n",1+_editing,_body[_editing]));
  write(sprintf("%3d:  ",_editing+1));
  input_to("_edit");
}

mixed prompt()
{
  int comm,*xc,f;

  comm=f_next_note(1);
  if(comm==-1)
    return write("Read from the next board -> "),0;
  if(!intp(comm))
  {
    write(sprintf("** Error: Wrong return from f_next_note: %O\n",comm));
    write("Quited.\n");
    save_and_exit();
    return 1;
  }
  xc=comments_to(comm);
  if(xc)
    if(sizeof(xc))
      f=xc[-1];
  if(f==0)
    if(comm==0)
      write("Read from the next board -> "),0;
    else
      write("Read the next note ("+comm+") -> "),0;
  else
    write("Read the next comment to note "+f+" ("+comm+") -> "),0;
}


void more(string s)
{
  if(s=="q")
  {
    more_text=({""});
    prompt();
    input_to("receive");
    return;
  }
  write(implode(more_text[0..18],"\n")+"\n");
  more_text=more_text[19..1000000];
  if(sizeof(more_text))
  {
    write("** More (q to quit) **");
    input_to("more");
  } else {
    prompt();
    input_to("receive");
  }
}

void dcomm(int a,int i)
{
  int h;
  mixed *c;
  c=comments(a);

  for(h=0;h<sizeof(c);h++) 
    if(sizeof(comments_to(c[h])))
      if(!deleted(c[h]))
      {
	if(i) write("............>"[1..i]);
	dshead(c[h]);
	dcomm(c[h],i+1);
      }
}

void dct(int num)
{
  if(num)
    return "Comment to note "+num+" by "+author(num)+".\n";
}

void dc(int num)
{
  return "Comment in note "+num+" by "+author(num)+".\n";
}

static int cur_line;

#define w(x) buff+=x;
varargs mixed read_current(int no_more, int no_dump) 
{
  string buff, tmp;
  buff="";

  r_notes -= ({ c_note });
  r_notes += ({ c_note });

  if(deleted(c_note))
    return 
      write("The note has been deleted, or there never was no such note\n"),0;
  w("\n");
  w(rhead(c_note));
  if(locked(c_note))
    w(sprintf("Locked by %s(%O)\n",locked(c_note)[1], locked(c_note)[0]));
  w(LINE);
  tmp = body(c_note);
  w(strlen(tmp) > 9999 ? tmp : my_line_break(tmp, strlen(LINE)-1));
  w(LINE);
  if(c_note != 0)
    w(implode(map_array(comments(c_note),"dc",this_object()),""));

  if( no_dump )
    return buff;

  if(!no_more && sizeof(more_text=explode(buff,"\n"))>20)
    return more(buff),1;
  else
    return write(buff),0;
}
#undef w


varargs mixed f_read_note(int note, int no_dump)
{
  c_note = note;
  return read_current(1, no_dump);
}

varargs mixed read_note(int note) 
{
  c_note = note;
  return read_current(1, 1);
}

int new_note;

void f_help(string s)
{
  string tmp;
  sscanf(s,"%*s %s",tmp);
  if(s=read_file("players/milamber/boardhelp/"+lower_case(""+tmp)))
  {
    write(LINE);
    write(s);
    if(s[-1]!='\n')
      write("\n");
    write(LINE);
  } else {
    write("There is no help for that.\n");
  }
}

void f_lock(string s)
{
  mixed tmp;
  sscanf(s,"%*s %d",c_note);
  if(tmp=lock(c_note))
  {
    if(tmp==3)
      write("There is no such note.\n");
    else
      write("The note is already locked by "+
	    "someone with a higher level.\n");
    return;
  }
  write("Ok.\n");
}

void f_unlock(string s)
{
  mixed tmp;
  sscanf(s,"%*s %d",c_note);
  if(tmp=unlock(c_note))
  {
    if(tmp==3)
      write("There is no such note.\n");
    else
      write("The note is locked by someone with a higher level.\n");
    return;
  }
  write("Ok.\n");
}

string f_reply(string s)
{
  sscanf(s,"%*s %d",c_note);
  if(c_note!=0)
  {
    if(deleted(c_note))
     return "That note has been deleted, or there is no such note\n";
    write("Ok, Writing note in reply to \n");
    dshead(c_note);
  }
  write(LINE);      
  new_note=get_new_note();
  add_comment_to(new_note,c_note);
  add_comment(c_note,new_note);

  state=W_HEAD;
  if(c_note)
    return (q=2),"Title (Default="+title(c_note)+")\n: ";
  else
    return (q=1),"Title: ";
}


varargs int f_next_note(int nomodify)
{
  int comm;
  comm=-1;
  m_notes -= ({ 0 });
  if(sizeof(m_notes))
    comm=m_notes[0];

  if(comm==-1)
  {
    m_notes=deep_comments()-r_notes;
    if(sizeof(m_notes))
      return f_next_note(nomodify); /* Yiha! */
    else
      return 0;
  }
  if(nomodify)
    return comm;
  
  c_note=comm;
  m_notes=m_notes[1..100000];
  return c_note;
}

void f_previous_note()
{
  mixed *f;
  if(pointerp(comments_to(c_note)) && sizeof(comments_to(c_note)))
    c_note=comments_to(c_note)[0];
  else
    while(deleted(c_note) && c_note>=0) /* Note '0' is _never_ deleted. */
      c_note--;
  if(c_note<0)
    c_note=num_notes()-1;
  f=m_notes;
  /* Probably nothing at all. */
  m_notes= deep_comments(c_note) - r_notes - ({ 0 }); 
  m_notes += f - m_notes;
}

void f_display_heads()
{
  int a;
  write(LINE);      
  dshead(0);
  if(display_all)
  {
    for(a=1;a<num_notes();a++)
      if(!deleted(a))
	dshead(a);
  } else {
    dcomm(0,0);
  }
  write(LINE);      
}

void f_delete(string s)
{
  int d,tmp;
  sscanf(s,"D %d",d);
  if(!d)
    return write("Syntax: D <note_number>\n");
  if(tmp=delete_note(d))
  {
    switch(tmp)
    {
     case 3:
      return write("There is no such note! ["+d+"]\n");
     default:
      return write("That note is locked! ["+d+"]\n");
    }
  } else {
    write("Ok.\n");
  }
}

void rec_delete(int note)
{
  mixed tmp;
  if(deleted(note))
  {
    write("There is no such note to zap.\n");
    return;
  }
  if(note)
    map_array(comments(note),"rec_delete",this_object()); /* Ta-dum.. */

  if(tmp=delete_note(note))
  {
    switch(tmp)
    {
     case 3:
      return write("There is no such note! ["+note+"]\n");
     case 1:
      return write("That note is locked! ["+note+"]\n");
    }
  } else {
    write("Ok.\n");
  }
}

drop() { return 1; }

string check_palidrome(string s)
{
  string day,month;
  int mon,date,hour,minute,second,year;
  sscanf(s,"%s %s %d %d:%d:%d %d",day,month,date,hour,minute,second,year);
  mon=months[lower_case(month)];

  return sprintf("%d-%02d-%02d %02d:%02d:%02d%s",year,mon,date,hour,
		 minute,second,
		 (sprintf("%c%c%c%c%c%c",
			  second%10+1,second/10+1,minute%10+1,minute/10+1,
			  hour%10+1,hour/10+1) == 
		  sprintf("%c%c%c%c%c%c",hour/10+1,hour%10+1,minute/10+1,
			  minute%10+1,second/10+1,second%10+1))?
		 " (A palindrome!)":"");
}

int query_num_notes()
{
  if(!database)
    database=METADATABASE->query_database( dbn );

  return sizeof(allnotes());
}

void _exit() { save_and_exit(); }

mixed receive(string s)
{
  string res;
  mixed tmp;


  if(!database)
    database=METADATABASE->query_database( dbn );
  
  res=""; 

  switch(state)
  {
   case NOCOMM:
    switch(s[0])
    {
     case 'L': /* Lock note */
      f_lock(s);
      break;

     case 'U': /* Unlock note */
      f_unlock(s);
      break;

     case 'm':
      write("Ok. All notes on this board are marked 'unread'\n");
      r_notes = ({ 0 });
      m_notes = deep_comments(0) - r_notes;
      break;

     case 'M':
      write("Ok. ALL notes on this board are marked 'read'\n");
      r_notes = allnotes();
      m_notes -= r_notes + ({ 0 });
      break;

     case 's':
     case 'S':
      write("Ok. ALL comments to note " + c_note + " are marked 'read'\n");
      r_notes += deep_comments(c_note);
      r_notes = r_notes & r_notes;
      m_notes -= r_notes;
      break;

     case 'i':
     case 'N': /* New note */
      s="R 0";

     case 'k':
     case 'R': /* Reply note */
      input_to("receive");
      return write( f_reply(s) );

     case 'E': /* Edit note */
      sscanf(s,"%*s %d",c_note);
      if(deleted(c_note))
      {
	write("There is no such note as "+c_note+".\n");
	break;
      }
      if(name!=author(c_note) && strlen(author(c_note)) && 
	 author(c_note)[0]!='<')
      {
	write("You cannot edit someone elses note!\n");
	break;
      }
      if(!strlen(author(c_note)) || author(c_note)=="<Somebody>")
      {
	write("The note had no author, setting the author to "+name+"\n");
	write("and the creation time to "+my_ctime(time())+".\n");
	if(c_note==0)
	{
	  write("(You should use the P[rotect] command if you should want\n"+
		"to set the protection level of the board to something else\n"+
		"than 0.)\n");
	}
	set_head(c_note,title(c_note), time(), name, 0);
      }
      edit(title(c_note),body(c_note),c_note);
      return;

     case 'P':
      tmp=0;
      sscanf(s,"%*s %s",tmp);
      if(author(0) && strlen(author(0)) && author(0)!="<Somebody>" &&
	 name != author(0))
      {
	write("Only the creator of a board may set the protection!\n");
	break;
      }
      if(!tmp)
      {
	write("Syntax:\n"+
	" P[rotect] <number>      -- You must be level number or higher\n"+
        "                            to use the board.\n"+
	" P[rotect] <string>      -- <string> must be present in the players\n"
       +"                            inventory to use the board.\n");
	break;
      }
      if(atoi(tmp) || tmp=="0")
	tmp=atoi(tmp);
      set_head(0, title(0), made(0), author(0), ({ tmp, author(0) })); 
      break;

     case 'n': /* Go to (and read) next note  */
      if(s[1]=='b')
      {
	object obase;
	if(database)
	  database->set_read(r_notes & allnotes(), name);
	if(!this_player()->next_board())
	{
	  write("You have no more boards to read.\n");
	  prompt();
	}
	input_to("receive");
	return;
      }
      if(!f_next_note())
      {
	write("The time is "+check_palidrome(ctime(time()))
	      +" (according to the server)\n");
	return receive("nb");
	return;
      }
      if(qread) tmp=read_current(); else dhead(c_note);
      if(tmp) return;
      break;
      
     case 'p': /* Go to (and read) previous note  */
     case ']': /* Unless you use lyskom, you wont get theese two.. */
     case '}': 
      f_previous_note();
      if(qread) tmp=read_current(); else dhead(c_note);
      if(tmp)
	return;
      break;

     case 'g': /* Go to (and read) note */
      sscanf(s,"%*s %d",c_note);
      if(c_note > num_notes()-1)
	c_note=num_notes()-1;
      if(c_note<0)
	c_note=0;
      if(qread)
	tmp=read_current();
      else
	dhead(c_note);

      m_notes= deep_comments(c_note) - r_notes -({ 0 });
      if(f_next_note(1)==c_note)
	f_next_note();
      if(tmp)
	return;
      break;

     case 'Q':
      write("Quick-read "+(qread?"off":"on")+"\n");
      qread=!qread;
      break;

     case 'l':
      write("Linear display mode "+(display_all?"off":"on")+"\n");
      display_all=!display_all;
      break;

     case 'h': /* Heads */
      f_display_heads();
      break;

     case 'r': /* Read current (or note) Will not modify 'm_notes'*/
      sscanf(s, "%*s %d", c_note);
      if(read_current())
	return;
      break;

     case 'd':
      sscanf(s, "%*s %d", c_note);
      dshead(c_note);
      dcomm(c_note,0);
      break;

     case 'q': /* Quit */
      write("Bye.\n");
      save_and_exit();
      return;

     case 'H': f_help(s); break;
     case 'D': f_delete(s); break;

     case 'Z':
      sscanf(s,"%*s %d",c_note);
      rec_delete(c_note);
      break;

     case '?': f_help("H ?"); break;

     case 'A': /* Archive the current note on disk.. */
      if(this_player()->query_level() > 21)
      {
	if(!sscanf(s, "%*s %s", s))
	{
	  write("Syntax: 'A filename'");
	  break;
	} 
	if(sscanf(s, "%*s/%*s"))
	{
	  write("Due to security resons, all files must be saved in your\n"+
		"directory, also, the file name may not contain '/'.\n");
	  break;
	}
	if(catch(write_file("players/"+lower_case(name)+"/"+s, 
			    read_note(c_note))))
	  write("I failed to save the file.\n");
	else
	  write("Ok, note saved to /players/"+lower_case(name)+"/"+s+".\n");
	break;
      }

     case ' ':
      s="";

     default:
      if(!strlen(s))
	return receive("n");

      if(sscanf(s,"%*d"))
	return receive("g "+s);
      else
	write("{r,h,g,n,d,H,q,l,R,N,D,Z,m,M,S,L,U,Q,?} ");
    }
    break;

   case W_BODY:
    switch(s[0..1])
    {
      object o;
     case "**":
     case ".":
      state=NOCOMM;
      write("Ok.\n");
      add_body( new_note, 0 ); /* done editing... */
      break;

     case "~q":
      state=NOCOMM;
      add_body( new_note, 0 ); /* done editing... */
      f_delete("D "+new_note);
      write("Aborted.\n");
      break;

     case "~i":
      if(o = present("wizsoul", environment()))
      {
	string f;
	s = replace(s, " ", "");
	if(f = read_file((string)o -> mk_path(s[2..1000])))
	{
	  add_body(new_note, f);
	  cur_line += 1+(tmp = sizeof(explode(f, "\n") || ({ })));
	  write(sprintf("%d lines read.\n", tmp));	 
	}
	else
	  write("No such file.\n");
	write(sprintf("%3d: ", cur_line));
	input_to("receive");
	return;
      }
     case "~r":
     case "~d":
      tmp = c_note;
      f_read_note(new_note);
      c_note = tmp;
      write(sprintf("%3d: ", cur_line));
      input_to("receive");
      return;

     default:
      add_body(new_note,s + "\n");
      write(sprintf("%3d: ",++cur_line));
      input_to("receive");
      return;
    }
    break;

   case W_HEAD:
    if(!strlen(s) && q == 2)
    {
      q=0;
      s=title(c_note);
    } else if(!strlen(s)) {
      write(": ");
      input_to("receive");
      return;
    }

    set_head(new_note, s, time(), name, 0 );

    state=W_BODY;
    write(LINE);
    cur_line=0;
    write("End with '.' or '**'. ~q to abort, ~d to display the note.\n");
    if(this_player()->query_level() > 21)
      write(" ~i filename to include a file.\n");
    write("  0: ");
    add_body(new_note, ""); /* Clear the body.. */
    input_to("receive");
    return;
  }
  prompt();
  input_to("receive");
  write( res );
}

int atoi(string s) { return efun::atoi(s); }

int query_current_note() { return current_note(); }

int *query_unread() { return allnotes() - r_notes; }

void init(status no_real_init)
{ 
  if(!dbn)
    return;

  no_real_init=no_real_init==42;

  while(present("boardreader 2"))
    destruct(present("boardreader 2"));
  if(!no_real_init && this_player()!=environment())
    return;
 
  if(!database || !database->security())
  {
    write("Security violation.\n");
    destruct(this_object());
    return;
  }

  if(!no_real_init)
  {
    name=capitalize((string)this_player()->query_real_name());
    write(LINE+
	  "Welcome to MUDKom v1.2\n"+
	  "This is the '"+capitalize(dbn)+"' board.\n"+
	  LINE);
    input_to("receive"); 
    prompt();
  }
}

get() { return 1; }

