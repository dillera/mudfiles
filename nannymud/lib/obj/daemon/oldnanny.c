#define OSHADOW       "/local/oldnanny_hlp"
#define HOST          "130.236.254.200 2000"

#define SOCK_STREAM   1
#define SOCK_CONN     8
#define SOCK_WROK     4

object who, obj;
string mess;
int fd, flags;

status query_prevent_shadow() { return 1; }

status get() { return 1; }

nomask reset(status flag)
{
   if(flag)
      return;
   who = 0;
   mess = "";
   fd = -1;
   flags = 0;
}

void init()
{
   if(!_isclone(this_object()) || !this_player() || !interactive(this_player()))
      return;
   add_action("user_open", "connect");
   add_action("user_write", "send");
   add_action("user_close", "hangup");
}

private static nomask void die()
{
   socket_close(fd);
   fd = -1;
}

nomask void receive_data(int fd, string str)
{
   if(!who)
      die();
   else
      tell_object(who, str);
}

nomask void write_data(int fd)
{
   int err;
   flags |= SOCK_WROK;
   if(!who)
   {
      die();
      return;
   }
   if(mess == "")
      return;
   err = socket_write(fd, mess, 0);
   mess = "";
   if(err >= 0)
      return;
   tell_object(who, "Error writing: " + socket_error(err) + "\n");
   die();
}

nomask void broken_conn(int fd)
{
   if(who)
      tell_object(who, "Connection closed by foreign host.\n");
   if(obj)
      destruct(obj);
   fd = -1;
}

nomask user_open(string where)
{
   int err;
   if(!_isclone(this_object()) || !this_player() || !interactive(this_player()))
      return 0;
   if((fd = socket_create(SOCK_STREAM, "receive_data", "broken_conn")) < 0 ||
      (err = socket_connect(fd, HOST, "receive_data","write_data")) < 0)
   {
      write("Error connecting.\n");
      die();
   }
   else
   {
      who = this_player();
      flags = SOCK_CONN;
      mess = "";
      if(catch(obj = clone_object(OSHADOW)) ||
	 (int) obj->activate(who, !!where) == -1)
      {
	 write("Could not filter output.\n");
      }
      write("Connected.\n");
   }
   return 1;
}

nomask status user_write(string tosend)
{
   string *tmp;
   int err;
   if(fd == -1)
      return notify_fail("Not connected.\n"), 0;
   else if(!this_player() || who != this_player())
      return notify_fail("Permission denied.\n"), 0;
   else if(flags & SOCK_WROK)
   {
      if(!tosend || !(tmp = explode(tosend, "\\n")))
	 tosend = "\n";
      else
	 tosend = implode(tmp, "\n") + "\n";
      if((err = socket_write(fd, tosend, 0)) >= 0)
	 write("Ok.\n");
      else
	 write("Write error.\n");
   }
   else
   {
      mess += tosend;
      write("Queuing message.\n");
   }
   return 1;
}

nomask status user_close(int fd)
{
   if(fd == -1)
      return notify_fail("Not connected.\n"), 0;
   else if(!this_player() || who != this_player())
      return notify_fail("Permission denied.\n"), 0;
   if(obj)
      destruct(obj);
   die();
   write("Connection closed.\n");
   return 1;
}
