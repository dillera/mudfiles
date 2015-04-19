static string morestring="";
static string call_when_finished;
static string call_where;
simplemore(str) { morestring+=str; }

flush(str)
{
  int e;
  string a;
  int rows;

  if (this_player()->query_rows() > 0)
      rows = this_player()->query_rows() - 2;
  else
      rows = 22;
  
  if(str=="q")
  {
    morestring="";
  }else{
    if(rows <0 || rows>10000) /* No more */
    {
      write(morestring);
      morestring="";
    }else{
      for(e=0;e<rows;e++)
      {
	if(sscanf(morestring,"%s\n%s",a,morestring))
	{
	  write(a+"\n");
	}else{
	  write(morestring);
	  e=0x7ffffff0;
	  morestring="";
	}
      }
    }
  }

  if(strlen(morestring))
  {
    write("*Press return for more or q to end. >");
    input_to("flush");
  }else{
    if(call_when_finished)
    {
      call_other(call_where,call_when_finished);
      call_when_finished=0;
    }
  }
}


set_call(fun)
{
  if(fun)
  {
    call_when_finished=fun;
    call_where=file_name(previous_object());
  }else{
    call_when_finished=0;
  }
}

