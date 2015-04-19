/*********************************************************
 *  A simple mudfinger client for muds with socketefuns  *
 *  Author: Gwendolyn@{Nannymud,Ancient Anguish}         *
 *********************************************************/

#define HOSTPORT  "130.236.254.159 6889"

mapping who;
string request;

status get() { return 1; }
status drop() { return 1; }
void move(object dest) { move_object(this_object(), dest); }

void reset(status flag)
{
   if(flag)
      return;
   who = ([]);
}

void init()
{
   add_action("do_mfinger", "mfinger");
}

void broken_conn(int fd)
{
   if(who[fd])
      tell_object(who[fd], "Connection closed by foreign host.\n");
   who = m_delete(who, fd);
}

void write_data(int fd)
{
   int err;
   if(!who[fd])
      return;
   if((err = socket_write(fd, request + "\n", 0)) < 0)
   {
      socket_close(fd);
#ifdef DEBUG
      tell_object(who[fd], "Error writing: " + socket_error(err) + "\n");
#else
      tell_object(who[fd], "Error in mfinger.\n");
#endif
      return;
   }
#ifdef DEBUG
   tell_object(who[fd], "Request '" + request + "' sent. Waiting for response...\n");
#endif
}

void receive_data(int fd, string str)
{
   if(who[fd] && str)
      tell_object(who[fd], str);
   if(!str)
   {
      if(who[fd])
	 tell_object(who[fd], "That's all.\n");
      who = m_delete(who, fd);
   }
}

do_mfinger(arg)
{
   int fd, err;
   request = (arg ? arg : "");
   if((fd = socket_create(1, "receive_data", "broken_conn")) < 0)
      return write("Error creating socket: " + socket_error(fd) + "\n"), 1;
   if((err = socket_connect(fd, HOSTPORT, "receive_data", "write_data")) < 0)
   {
      socket_close(fd);
      write("Error connecting: " + socket_error(err) + "\n");
      return 1;
   }
   who[fd] = this_player();
   write("Establishing connection...\n");
   return 1;
}

int query_prevent_shadow() { return 1; }
