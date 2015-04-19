#define HOST          "130.236.254.156 6666"

#define SOCK_STREAM   1
#define SOCK_CONN     8
#define SOCK_WROK     4

int out_port;
int flags;
mixed buff,tmp,err;

status query_prevent_shadow() { return 1; }

private mixed *filter_channels(string channel)
{
  return map_array(users(),"channel_fun",this_object(),channel);
}

private void tell_users(string message,string channel)
{
  map_array(filter_channels(channel),"tell_fun",this_object(),message);
}

query_level()
{
  return 666;
}

object channel_fun(object ob, string channel)
{
  object res;
  enable_commands();
  res=(object)ob->query_wiz_line(channel);
  disable_commands();
  return res;
}

void tell_fun(object ob,string message){ if(ob)tell_object(ob,message); }

wiz(s,c) { tell_users("Wiz["+c+"] "+s+"\n",c); }

private static nomask void die()
{
   socket_close(out_port);
   out_port = -1;
}

nomask void receive_data(int fd, string str)
{
  string rest;
  rest=str;
  while(rest)
  {
    if(sscanf(rest,"bcast	%s\n%s",str,rest))
    {
      wiz(str,"iwiz");
    } else if(sscanf(rest,"send	%s	%s\n%s",str,str,rest))  {
      string who,what,from,where;
      sscanf(str,"to	%s	%s",who,what);   
      if(find_player(lower_case(who)))
      {
	tell_object(find_player(lower_case(who)),"InterWizTell: "+what+"\n");
      } else {
	sscanf(what,"%s@%s: %s",from,where,what);
	this_object()->user_write("send	"+from+"	to	"+
				  from+"	Deamon@NannyMUD: "+who+
				  ": No such player playing at the moment.\n");
      }
    }
  }
}

nomask void write_data(int fd)
{
  int err;
  flags |= SOCK_WROK;
  if(buff == "")
    return;
  err = socket_write(fd, buff, 0);
  buff = "";
  if(err >= 0)
    return;
}

nomask void broken_conn(int fd)
{
   out_port = -1;
}

nomask reset(string where)
{
  int err;
  if(where)
    return;
  if((out_port = socket_create(SOCK_STREAM, "receive_data", "broken_conn")) < 0 ||
     (err = socket_connect(out_port, HOST, "receive_data","write_data")) < 0)
  {
    die();
  } else  {
    flags = SOCK_CONN;
    buff = "";
  }
  buff="namea	NannyMUD\nnamea	Nanny\n";
  return 1;
}

nomask status user_write(string tosend)
{
  if(flags & SOCK_WROK)
  {
    if(!tosend || !(tmp = explode(tosend, "\\n")))
      tosend = "\n";
    else
      tosend = implode(tmp, "\n") + "\n";
    err = socket_write(out_port, tosend, 0);
  } else {
    buff += tosend;
  }
  return 1;
}
