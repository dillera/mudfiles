#define OSHADOW       "/local/oldnanny_hlp"
#define HOST          "130.236.254.200 2000"

#define SOCK_STREAM   1
#define SOCK_CONN     8
#define SOCK_WROK     4

object who, obj;
string mess;
int fd, flags;

status query_prevent_shadow() { return 1; }

private static nomask void die()
{
   socket_close(fd);
   fd = -1;
   if(obj)
      destruct(obj);
   destruct(this_object());
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
   tell_object(who, "Ooops, you fell through a time trapdoor.\n");
   die();
}

nomask void broken_conn(int fd)
{
   if(who)
   {
      tell_object(who, "Everything goes black for a second...\n");
      tell_object(who, "You wake up in a familiar place.\n");
      if(environment(who))
      {
	 tell_room(environment(who), (string) who->query_name() +
		   " suddenly returns from past times.\n", ({ who }));
      }
   }
   fd = -1;
   if(obj)
      destruct(obj);
   destruct(this_object());
}

nomask user_open(status quiet)
{
   int err;
   if(!_isclone(this_object()) || !this_player() || !interactive(this_player()))
      return 0;
   if((fd = socket_create(SOCK_STREAM, "receive_data", "broken_conn")) < 0 ||
      (err = socket_connect(fd, HOST, "receive_data", "write_data")) < 0)
   {
      die();
      return 0;
   }
   who = this_player();
   flags = SOCK_CONN;
   mess = "";
   input_to("user_write");
   if(catch(obj = clone_object(OSHADOW)) || (int) obj->activate(who, quiet) == -1)
   {
      write("Wow! You still have a channel through time attached to you.\n");
      write("Too bad it's a oneway channel...\n");
   }
   return 1;
}

nomask void user_write(string tosend)
{
   string *tmp;
   int err;
   if(fd == -1)
      return notify_fail("Not connected.\n");
   else if(!this_player() || who != this_player())
      return notify_fail("Permission denied.\n");
   input_to("user_write");
   if(flags & SOCK_WROK)
   {
      if(!tosend || !(tmp = explode(tosend, "\\n")))
	 tosend = "\n";
      else
	 tosend = implode(tmp, "\n") + "\n";
      if((err = socket_write(fd, tosend, 0)) < 0)
      {
	 write("Ooops, you fell through a time trapdoor.\n");
	 die();
      }
/*
      else
	 write("Ok.\n");
*/
   }
   else
   {
      mess += tosend;
      write("Queuing message.\n");
   }
}
