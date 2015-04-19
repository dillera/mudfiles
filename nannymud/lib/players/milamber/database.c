#define NOTE(x) data[x]
#define HEAD(x) data[x][0]

#define BODY(x) body[x]

#define COMM(x) data[x][2]
#define COTO(x) data[x][3]
#define FLAGS(x) data[x][4]

#define TITL(x) HEAD(x)[0]
#define MADE(x) HEAD(x)[1]
#define AUTH(x) HEAD(x)[2]
#define LOCK(x) HEAD(x)[3]

#define NOTE_SIZE 5
#define HEAD_SIZE 4

#define FLAG_UNFINISHED 1
#define FLAG_EDITED 2

#define OWNER(note) \
(!(AUTH(note) && strlen(AUTH(note)) && AUTH(note)[0]!='<' &&\
   this_player(1)->query_real_name()!=lower_case(AUTH(note)||"")))

#define DIR   "players/milamber/boards/"
#define BOARD "players/milamber/board"

#define array(X) X *

static mapping read=([]);
static string name="unknown";

status no_remote_read = 0 ;
mapping data          = ([ 0:({ ({"",0,"",0}), "", ({}), ({}) }) ]);
int current_note      = 1;

mapping body =     ([]);
static mapping modified = ([]);

status no_remote_read() { return no_remote_read; }

void set_no_remote_read(status foo) 
{
  no_remote_read = foo; 
}

save_me()    
{ 
  remove_call_out("really_save_me"); 
  call_out("really_save_me",30);
}

void error(string s)
{
  log_file("boardsystem.errors", s + " <" + name + ">.\n");
}

void restore_body(int note)
{
  string foo;

  if(BODY(note))
    return; 

  if( data[ note ][ 1 ] )
  {
    body[ note ] = data[ note ][ 1 ];
    data[ note ][ 1 ] = 0;
    modified[ note ] = 1;
    save_me();
    return;
  }

  if(foo = read_file(DIR + name + "/" + note))
  {
    error("Debug: Restored "+strlen(foo)+" bytes from "+ DIR + name 
	  + "/"  + note);
    body[ note ] = foo;
  } else  {
    body[ note ] = "";
    modified[ note ] = 1;
    error("No body for note "+note);
    this_object() -> DeleteNote( note );
    save_me();
  }
}

int is_on_the_board(int start, int end)
{
  int i;
  for(i=start; i<end; i++)
    if(data[i])
      return 1;
}

string encode_read( array(int) what)
{
  int i, j, k, l, m;
  string res;
  res="";
  what = what & what; /* Magic */

  if(intp(what[0]))
    j=k=what[0];


  for(i=1; i<sizeof(what); i++)
  {
    if(intp(what[i]) && (what[ i ] > k+1))
    {
      if(is_on_the_board(k+1, what[i]))
      {
  	if(j==k)   
          res += j + "*";
	else 	     
	  res += j+"-"+k+"*";
	j=k=what[i]; /* Next range. */
      }
      else
	k=what[i];
    } else {
      k++;
    }
  }
  if(j==what[-1])
    res += j + "*";
  else if(intp(what[-1]))
    res += j + "-" + what[-1] + "*";
  else
    res += j + "*";
  return res;
}


array (int) decode_read( mixed what )
{
  string a, b;
  array(int) res;
  int i, i1, i2, cs;

  if(pointerp(what))
    return what;
  if(!stringp(what))
    return ({ 0 });
  
  res = allocate(100);
  cs = 100;
  
  while(strlen(what))
  {
    if(sscanf(what, "%s*%s", a, what) != 2) what = "";
    if(a && strlen(a))
    {
      if(sscanf(a, "%d-%d", i1, i2) == 2)
      {
	int needed_size, tmp;
	needed_size = i2-i1+1;
	if(i+needed_size > cs)
	{
	  res += allocate(needed_size);
	  cs += needed_size;
	}
	for(tmp=i1; tmp<=i2; tmp++)
	  res[i++]=tmp;
      } else {
	if(i+1 > cs)
	{
	  res += allocate(10);
	  cs += 10;
	}
	res[i++] = atoi(a);
      }
    }
  }
  return res & m_indices(data);
}

mixed query_read(string na)
{
  mixed foo;
  int i;
  object pl;
  pl = find_player(lower_case(na));

  if(!pl)
  {
    write("I cannot find the player "+na+"\n");
    return;
  }

  if(pointerp(read[pl]))
    return read[pl];

  read[ pl ] = decode_read( pl -> getenv( "read:"+name ) );
  return read[ pl ];
}


void set_read(int *re, string na)
{
  object pl;
  pl = find_player(lower_case(na));
  if(!pl)
  {
    write("BoardDatabase: Setting read in non existant player.\n");
    return;
  }
  read[ pl ] = re&re;
  pl->setenv("read:"+name, encode_read( read[pl] ));
}

mixed secret(mixed tmp) 
{
  if(pointerp(tmp)) 
    return tmp+({});
  else if(mappingp(tmp)) 
    return (tmp)+([]);
  else
    return tmp;
}

mixed security()
{
  if(!this_player(1))
    return 0;

  if(LOCK(0))
  {
    if(intp(LOCK(0)[0]) && 
       (LOCK(0)[0] > this_player(1)->query_level()))
    {
      if(!OWNER(0))
      {
	write("Only players of level "+LOCK(0)[0]+
	      " or higher may read from this board.\n");
	return 0;
      } else {
	write("** Notice: You have set the protectionlevel on this board\n"+
	      "           to "+LOCK(0)[0]+", which really should make it\n"
	      +"           impossible for YOU to read it.\n");
      }
    } else if(stringp(LOCK(0)[0])) {
      if(!present(LOCK(0)[0],this_player(1)) && 
	 ((int)this_player(1)->query_level() < 23))
	if(!OWNER(0))
	{
	  write("You may not read from this board.\n");
	  return 0;
	}
	else
	  write("** Notice: You have set the protectionlevel on this board\n"+
		"           to '"+LOCK(0)[0]+"', which really should make it\n"
		+"           impossible for YOU to read it.\n");
    }
    if(this_player(1)->query_real_name() == "guest")
    {
      write("Guests may not read from this board.\n");
      return 0;
    }
  }
  return 1;
}


int CurrentNote() { return current_note; }

really_save_me()    
{ 
  int i;
  int *foo;
  string f;

  save_object( DIR + name ); 

  if(sizeof(foo=m_indices( modified )))
  {
    mkdir( DIR + name );
    for(i=0; i<sizeof( foo ); i++)
    {
      f= DIR + name + "/" + foo[ i ];
      if(body[ foo[i] ])
      {
	rm(f);
	write_file( f, body[ foo[i] ] );
      }
      modified = m_delete( modified, foo[i] );
    }
  }
}


restore_me() 
{ 
  int i;
  int *notes;
  mixed *vals;

  restore_object( DIR + name );
  notes=m_indices(data);
  vals=m_values(data);
  i=sizeof(notes);
  while(i--)
    if( sizeof(vals[i]) < NOTE_SIZE )
      vals[i] += ({ 0,0,0,0,0 })[ 1 .. NOTE_SIZE - sizeof(vals[i]) ];
  data=mkmapping( notes, vals );
}

void SetName(string n, string short, string description, string me, int t,
	     int lock)
{
  if(name!="unknown")
  {
    error("** The name is already set!");
    return;
  }
  name=lower_case(n);
  catch(restore_me());
  if(short)
    data[0]=({ ({ short, t, me, lock && ({ lock , me }) }),
	       description, COMM(0), COTO(0) });
}

string query_name() 
{ 
  if(!security())
    return 0;
  return name; 
}

int *AllNotes() 
{ 
  if(!security())
    return ({});
  return m_indices(data) & m_indices(data); 
}

int GetNewNote() 
{
  if(!security())
    return -1;

  data = m_delete( data, current_note );
  data[current_note]=({ ({ "", time(), "", 0 }),
			  "",({}),({}), FLAG_UNFINISHED });
  return current_note++; 
}

int NumNotes(/*void*/)    
{ 
  if(!security())
    return 0;
  return current_note;  
}

void long()
{
  if(!security())
    return;
  write ("Name           : "+name+".\n"+
	 "Current note   : "+current_note+".\n"+
	 "Number of notes: "+sizeof(m_indices(data))+".\n"+
	 "Owner          : "+AUTH(0)+".\n"+
	 "Title          : "+TITL(0)+".\n");
}

status Deleted(int note)   
{ 
  if(!intp(note))
    return 1;
  return !data[note]; 
}

int Flags(int note)
{
  if(!Deleted(note))
    return FLAGS(note);
}


void UnFlag(int note)
{
  if(!Deleted(note))
    FLAGS(note) = 0 ;
}

string Title(int note)     
{
  if(!security())
    return "Bad luck.";
  if(Deleted(note))
    return "This note has gone to met its creator.";
  return (TITL(note) + 
/*	  (FLAGS(note) & FLAG_EDITED     ?  " <Edited>"  : "") + */
	  (FLAGS(note) & FLAG_UNFINISHED ? " <Unfinished>" : ""));
}

int Made(int note)         
{ 
  if(!security())
    return -0x7ffff;
  if(!Deleted(note))
    return MADE(note); 
  else
    error("Made on non-note. [" + note + "]");
  return -0x7ffff;
}

string Author(int note)    
{
  if(!security())
    return "Nobody";
  if(!Deleted(note))
    return AUTH(note); 
  else
    error("Author on non-note. ["+note+"]");
  return "";
}

string Body(int note)      
{ 
  if(!security())
    return "Empty";
  if(Deleted(note))
    return "This note is no more. It has gone to meet it's creator.";

  restore_body(note);
  return BODY(note); 
}

int *Comments(int note)    
{
  if(!security())
    return ({});
  if(!Deleted(note))
  {
    COMM(note) -= ({ note });
    return secret( COMM(note) );
  } else {
    error("Comments on non-note. ["+note+"]");
  }
  return ({});
}

int *CommentsTo(int note)  
{ 
  if(!security())
    return ({});
  if(!Deleted(note))
  {
    COTO(note) -= ({ note });
    return secret(COTO(note)); 
  }
  else
    error("CommentsTo on non-note. ["+note+"]");
  return ({});
}

status Locked(int note)
{
  if(!security())
    return 1;
  if(!Deleted(note))
    return secret(LOCK(note));
  else
    error("Locked on non-note. ["+note+"]");
}

void AddBody(int note,string s)
{
  if(!security())
    return;
  if(LOCK(note))
    return error("AddBody to locked note. ["+note+"]");
  if(!OWNER(note))
    return error("AddBody to another persons note. ["+note+"]");
  
  if(!Deleted(note))
  {
    if(!s)
      FLAGS(note)&=~FLAG_UNFINISHED;
    else
    {
      if(BODY(note))
      {
	modified[ note ] = 1;
	BODY(note) += s;
      } else {
	modified[ note ] = 1;
	restore_body(note);
	BODY(note) += s;
      }
      save_me();
    }
  } else {
    write("The note is deleted.\n");
    error("AddBody to non-note. ["+note+"]");
  }
}

void SetBody(int note,string s)
{
  if(!(intp(note) && stringp(s)) || !security())
    return;

  if(LOCK(note))
    return error("SetBody on locked note. ["+note+"]"); 

  if(!OWNER(note))
    return error("SetBody to another persons note. ["+note+"]");


  if(!Deleted(note))
  {
    modified[ note ] = 1;
    BODY(note) = s;
    FLAGS(note) &= ~FLAG_UNFINISHED;
    save_me();
  } else {
    error("SetBody on non-note. ["+note+"]");
  }
}

void SetTitle(int note,string s)
{
  if(!(intp(note) && stringp(s)) ||!security())
    return;
  if(LOCK(note))
    return error("SetTitle on locked note. ["+note+"]");
  if(!OWNER(note))
    return error("SetTitle to another persons note. ["+ note+"]");

  if(!Deleted(note))
  {
    FLAGS(note) |= FLAG_EDITED;
    FLAGS(note) &= ~FLAG_UNFINISHED;
    TITL(note)=s;
    modified[ note ] = 1;
    save_me();
  } else {
    error("SetTitle on non-note. ["+note+"]");
  }
}

void AddComment(int note, int com)
{
  if(!(intp(note) && intp(com) && security()))
    return;
  if(note == com)
  {
    error( "AddComment would make a circular thread!\n" );
    return;
  }

  if(!Deleted(note) && !Deleted(com))
  {
    COMM(note)-=({ com });
    COMM(note)+=({ com });
    save_me();
  } else {
    error("AddComment to or from non-note. ["+note+" -> "+com+"]");
  }
}

void AddCommentTo(int note, int com)
{
  if(!security())
    return;

  if(!OWNER(note))
    return error("AddCommentTo to another persons note. ["+note+"]");

  if(note<current_note && data[note])
  {
    COTO(note) -= ({ com });
    COTO(note) += ({ com });
    save_me();
  } else {
    error("AddCommentTo to or from non-note. ["+note+" <- "+com+"]");
  }
}

void SetHead(int note, string title, int made, string author, mixed locked)
{
  if(!(stringp(title) && intp(made) && stringp(author) 
       && security()))
    return;

  if(!OWNER(note))
    return error("SetHead to another persons note. ["+note+"]");

  if(locked && !pointerp(locked))
    locked = ({ locked, capitalize(this_player(1)->query_real_name()) });

  if(!Deleted(note))
  {
    HEAD(note)=({ title, made, author, locked });
    save_me();
  } else {
    error("SetHead on non-note. ["+note+"]");
  }
}

status convert_board(object board)
{
  int i,j;
  mixed note;
  data=([ 0:({ ({"",0,"",0}), "", ({}), ({}) }) ]);
  current_note=1;
  while(note=(mixed *)board->display_note(++i))
  { 
    int nn,ct;
    string head;
    nn=GetNewNote();
    ct=0;

    if(sscanf(note[0],"Re:%s",head))
    {
      note[0]=head;
      for(j=0;j<NumNotes();j++)
	if(TITL(m_indices(data)[j]) == head)
	{
	  ct=m_indices(data)[j];
	  break;
	}
    }
    write("Adding the note named '"+note[0]+"'...");
    AddComment(ct,nn);
    AddCommentTo(nn,ct);
    SetHead(nn,note[0],time(),note[1],note[3]);
    AddBody(nn,note[2]);
    write("Done.\n");
  }
}


void RemoveComment(int note, int com)
{
  if(!(intp(note) && intp(com)) || !security())
    return;
  if(note<current_note && data[note])
  {
    COMM(note) -= ({ com });
    save_me();
  } else {
    error("RemoveComment to non-note. ["+note+"]");
  }
}

int Lock(int note)
{
  if(!intp(note) || !security())
    return 1;
  if(!note)
    return 3;
  if(note<current_note && data[note])
  {
    if(LOCK(note))
    {
      if(this_player()->query_level() < LOCK(note)[0]) /* Lock Level */
	return 1; /* Security violation */
    } 
    LOCK(note)=({ this_player()->query_level(), 
		  capitalize((string)this_player()->query_real_name()) });
    save_me();
    return 0;
  }
  return 3;
}

int Unlock(int note)
{
  if(!note)
    return 3;
  if(!Deleted(note))
  {
    if(LOCK(note))
    {
      if(!stringp(LOCK(note)[1]))
      {
	LOCK(note)[1]="";
	LOCK(note)[0]=0;
      }
      if(security() && (LOCK(note)[0]<this_player()->query_level() 
			|| (lower_case(LOCK(note)[1]) ==
			    this_player()->query_real_name())
			|| OWNER(note)))
      {
	LOCK(note)=0;
	save_me();
	return 0;
      }
      return 1;			/* Security violation */
    }
    return 2;			/* Already unlocked. */
  }
  return 3;			/* No such note */
}

status DeleteNote(int note)
{ 
  int i;

  if(!security())
    return 1;

  if(Deleted( note ))
    return 3; /* No such note */

  if(LOCK(note))
    return 1; /* Security violation */


  mkdir( DIR + name + ".deleted/" );
  BOARD->set_database( this_object() );
    write_file( DIR + name + ".deleted/" + note, 
	       BOARD->read_note( note, 1 ));

  for(i=0;i<current_note;i++)
    if(data[i])
      COMM(i) -= ({ note });

  if(sizeof(COMM(note)))
  {
    COMM(COTO(note)[0]) += COMM(note);
    for(i=0;i<sizeof(COMM(note));i++)
    {
      if(data[COMM(note)[i]])
      {
	COTO(COMM(note)[i]) += ({ COTO(note)[0] });
	COTO(COMM(note)[i]) -= ({ note });
      }
    }
  }

  data=m_delete(data, note);
  body=m_delete(body, note);
  rm( DIR + name + "/" + note );
  save_me();
}

static int invalidated;
void invalidate() { invalidated=1; }

void _exit() 
{ 
  if(invalidated || !m_sizeof(data))
    catch(rm(DIR + name + ".o"));
  else
    really_save_me(); 
}
