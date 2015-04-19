/* (c) Milamber & Slater 1994 */


#define WEBSTERHOST "129.240.76.3"
NF(){}

int s, busy;
string str = "", function;
mixed *stack = ({});

short()
{
  return "a webster daemon, reading from a large book";
}

id(s)
{
  return s=="daemon" || s=="webster" || s=="book" || s=="webster daemon";
}

long(s)
{
  switch(s)
  {
   case "daemon":
   case "webster":
   case "webster daemon":
    write("This small daemon is reading from a huge book.\n");
    break;

   case "book":
    write("It's really huge, and seems to contain all words "+
	  "known to mankind.\n");
    break;
  }
}

timeout()
{
  socket_close(s);
  reset(0);
  got_in(-1,"WEBSTERD Timeout.\nRetry\n"+sprintf("%c",-128));
}

varargs public int send_request(int request, string st, string fun, object o,
				object tp, int i) 
{ 
  string q;
  if(!st)
    return 0;
    
  if(i)
    stack = ({ ({ request, st, fun || "got_reply", o || previous_object(), 
		  tp || this_player(1)}) }) + stack;
  else
    stack += ({ ({ request, st, fun || "got_reply", o || previous_object(), 
		   tp || this_player(1)}) });
  
  if(!i && (q=read_file("/webster/"+lower_case(implode(explode(st," "),"-")))))
  {
    str="";
    return this_object()->got_in(-1,sprintf("DEFINITION 0\n%s%c",q,-128)),1;
  }
  
  if(busy) 
    return 1;
  else 
  {
    call_out("timeout",10);
    st += "\n";
    str = "";
    switch(request)
    {
     case 0:
      socket_write(s,"SPELL "+st);
      break;
     case 1:
      socket_write(s,"ENDINGS "+st);
      break;
     default:
      socket_write(s,"DEFINE "+st);
    }
  }
  busy = 1;
  return 1;
}

#define req  data[0]
#define word data[1]
#define who  data[4]
#define obj  data[3]
#define fun  data[2]

got_in(int fd, string s)
{
  string q;
  mixed res;
  mixed *data;
  if(!sizeof(stack))
    return;
  data=stack[0];
  s = replace(s,"","");
  remove_call_out("timeout");
  call_out("timeout",5);
  
  str += s;
  if(str && sscanf(str,"SPELLING %*d%*s"))
    str+=sprintf("%c",-128);

  if(str[-1] == -128)
  {
    str=str[0..strlen(str)-2];
    remove_call_out("timeout");
    switch(req)
    {
     case 0: /* SPELL */
      sscanf(str,"SPELLING %d",res);
      break;
      
     case 1: /* ENDINGS */
      sscanf(str,"MATCHS\n%s",str);
      res=expode(str,"\n");
      break;
      
     default: /* DEFINE */
      if(sscanf(str,"DEFINITION %*d\n%s",res))
	break;
      sscanf(str,"SPELLING\n%s",str);
      res=explode(str,"\n");
      break;
    }
    q=catch(call_other(obj,fun, res, req, word, who));
    if(q)
      write(sprintf("Websterd: %O->%s(%O...,%O,%O,%O)\n"+
		    "         : %s\n",obj, fun, implode(explode(str[0..20],
								"\n"),"\\n"),
		    req, word, who, q));
    
    busy = str = 0;
    stack = stack[1..1000];
    if(sizeof(stack))
    {
      data = stack[0];
      stack = stack[1..1000];
      send_request(req, word, fun, obj, who, 1); 
    }
  }
}

#undef req
#undef word
#undef who
#undef obj
#undef fun

reset(arg)
{
  int e;
  if(arg) return;
  move_object(this_object(), "/obj/daemon/daemonroom");
  s = socket_create(1, "reset" ,"reset");
  if(s < 0 || (e = socket_connect(s, WEBSTERHOST + " 2627", 
				  "got_in", "NF")) < 0)
  {
    busy = 1;
    remove_call_out("reset");
    return call_out("reset", 60);
  } 
  busy = 0;
}

