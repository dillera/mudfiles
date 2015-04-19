#define BASE_DIR     "players/milamber/"
#define METADATABASE (BASE_DIR+"metadatabase")
#define BOARD        (BASE_DIR+"board")

public status display_boards_when_entering_the_game=0;

private static object my_board;
private static string *boards;
private static int pos, num_notes, num_unread;

string slurifax(string zonker) 
{
  return lower_case(zonker[5..1000]); 
}

void display_board_data(string board)
{
  int a,b,c;
  object db;

  db=METADATABASE->query_database(board);

  write(sprintf("%-20s%1s %4d   %4d\n",capitalize(replace(board,"_"," ")),
		db->no_remote_read()?"*":" ",
		a=sizeof((int *)db->AllNotes()-(int *)db->
			 query_read(this_player()->query_real_name())),
		b=sizeof((int *)db->AllNotes())-1));
  num_notes+=b;
  num_unread+=a;
}

static int current_pos;

public status next_board()
{
  int a,b,c;
  if(!my_board)
    my_board=previous_object();

  if(!boards)
    boards=sort_array(map_array(regexp((string *)this_player()->envv(),
			       "^read:"),"slurifax",this_object()),"foo",
		      this_object());

  b=sizeof(boards);

  for(a=current_pos;a<(b+current_pos);a++)
  {
    object db;
    db=METADATABASE->query_database(boards[a%b]); /* hmm */
    if(sizeof((int *)db->AllNotes()-(int *)db->
	      query_read(this_player()->query_real_name())))
    {
      my_board->set_database( db ); 
      current_pos = a%b;
      move_object(my_board, this_object()); /* Re-init it. */
      return 1;
    }
  }
}

status read_next_board()
{
  if(my_board)
    destruct(my_board);
  my_board = clone_object(BOARD);
  return next_board();
}

int foo(string a, string b) { return !!(a>b); }

public string fix(string s)
{
  object o;

  if(lower_case(s)!=s)
    return 0;

  if(strlen((o=(object)METADATABASE->query_database(s))->Author(0)) &&
     o->Author(0)[0] != '<')
    return s;
  o->invalidate();
  destruct(o);
  return 0;
}

public status display_boards(string arg)
{
  mixed *foo;

  if(arg)
    arg=lower_case(arg);
  if(!my_board)
    my_board=clone_object(BOARD);
  num_notes=num_unread=0;

  if(!boards)
    boards=sort_array(map_array(regexp((string *)this_player()->envv(),
		      "^read:"),"slurifax",this_object()),"foo",
		      this_object());
  if(!arg)
  {
    write("----------------------------------\n");
    write("Board name          Unread  Notes \n");
    write("----------------------------------\n");

    map_array(boards,"display_board_data",this_object());

    write("----------------------------------\n");
    write(sprintf("Total:                %4d   %4d\n",num_unread, num_notes));
    write("----------------------------------\n");
    write("Boards marked with a '*' is not readable from afar.\n");
    return 1;
  } else {
    if(arg=="all" && this_player()->query_level()>19)
    {
      write("Boards you read:\n"+implode(boards,", ")+"\n");
      write("Boards you don't read:\n");
      write(implode((map_array(get_dir("players/milamber/boards/*.o"),
			       "fix",this_object())-boards)-({ 0 }),", "));
      write("\n");
      return 1;
    } else if(sscanf(arg,"join %s",arg)) {
      object db;
      mixed *lo;
      if(this_player()->getenv("read:"+arg))
      {
	write("But, you already read the "+arg+" board!\n");
	return 1;
      }
      db=METADATABASE->query_database(arg);
      if(!strlen(db->Author(0)))
      {
	if(this_player()->query_level()<21)
	{
	  write("There is no such board.\n");
	  db->invalidate();
	  destruct(db);
	  return 1;
	} else {
	  write("There is no such board.\n");
	  write("Do 'boards create "+arg+"' to create it.\n");
	  db->invalidate();
	  destruct(db);
	  return 1;
	}
	if(!db->security())
	{
	  write("You can't read from that board.\n");
	  return 1;
	}
      }
      this_player()->setenv("read:"+arg,({}));
      boards=sort_array(map_array(regexp((string *)this_player()->envv(),
			    "^read:"),"slurifax",this_object()),"foo",
		      this_object());
      write("Ok.\n");
      return 1;
    } else if(sscanf(arg, "create %s", arg)) {
      if(this_player()->query_level()<20)
      {
	write("Only wizards may create boards.\n");
	return 1;
      }
      db=METADATABASE->query_database(arg);
      if(strlen(db->Author(0)))
      {
	write("There is already a board named '"+arg+"'.\n");
	return 1;
      }
      write("There was no board named '"+arg+"', but there is now.\n");
      write("Do 'boards "+arg+"' and 'E 0' to set the info in it.\n");
      this_player()->setenv("read:"+arg,({}));
      boards=sort_array(map_array(regexp((string *)this_player()->envv(),
			    "^read:"),"slurifax",this_object()),"foo",
		      this_object());
      write("Ok.\n");
      return 1;
    } else if(sscanf(arg,"leave %s",arg)) {
      if(this_player()->unsetenv("read:"+arg))
      {
	boards=sort_array(map_array(regexp((string *)this_player()->envv(),
		          "^read:"),"slurifax",this_object()),"foo",
			  this_object());
	write("Ok.\n");
      } else {
	write("You are not subscribing to any such board.\n");
      }
      return 1;
    } else if(arg=="on") {
      display_boards_when_entering_the_game=1;
      write("Ok. The boards will be displayed when you enter the game.\n");
      return 1;
    } else if(arg=="off") {
      display_boards_when_entering_the_game=0;
      write("Ok. The boards won't be displayed when you enter the game.\n");
      return 1;
    } else if(arg=="next") {
      if(!read_next_board())
	write("You have no more unread notes.\n");
      return 1;
    }
    

    arg=replace(arg," ","_");

    if(sizeof(foo=regexp(boards, "^"+arg)))
      arg=foo[0];
    else
      return notify_fail("You are not reading from that board,\n"+
			 "if you are sure that it exists, try 'boards join"+
			 " "+arg+"'.\n"),0;
    current_pos=member_array(arg,boards);
    if(((object)METADATABASE->query_database(arg))->no_remote_read() &&
       this_player()->query_level()<19)
    {
      write("Due to general complaints, mortals are no longer able to read\n"+
	    "this board from afar. I am very sorry, but thats the way it is."+
	    "\n                 /Milamber\n");
      return 1;
    }

    my_board->set_database(METADATABASE->query_database(arg));
    move_object(my_board,this_object());
  }
  return 1;
}

void dontlookhere(string s)
{
  string f;
  f=this_player()->getenv(s);
  this_player()->unsetenv(s);
  this_player()->setenv(lower_case(s),f);
}

public void reset()
{
  if(this_player())
  {
    map_array(regexp((string *)this_player()->envv()||({}),
		     "^read:"),"dontlookhere",this_object());
    
    if(display_boards_when_entering_the_game)
      display_boards(0);
  }
}
