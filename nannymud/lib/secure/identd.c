#define DEBUG
#define TIMEOUT 20

object *queue = ({});
int last_time;
int socket = -1;

static void call()
{
   if(!sizeof(queue))
      return;
   if(find_call_out("do_get_ident") == -1)
   {
      remove_call_out("do_get_ident");
      call_out("do_get_ident", 1);
   }
}

void get_ident(object ob)
{
   if(!objectp(ob) || !interactive(ob))
      return;
#ifdef DEBUG
   _syslog("Looking up identity of " + (string) ob->query_real_name());
#endif
   if(member_array(ob, queue) == -1)
      queue += ({ ob });
   call();
}

static void close()
{
   if(socket >= 0)
   {
      socket_close(socket);
      socket = -1;
   }
   if(sizeof(queue))
   {
      queue = queue[1..100000];
      call();
   }
}

void do_get_ident()
{
   string ip_num;
   if(socket >= 0 && time() - last_time < TIMEOUT)
      return;
   if(socket >= 0)
   {
#ifdef DEBUG
      if(sizeof(queue) && objectp(queue[0]))
      {
	 _syslog("Ident of " + (string) queue[0]->query_real_name() +
		 " timed out");
      }
      else
	 _syslog("Ident request timed out");
#endif
      close();
   }
   while(sizeof(queue) && (!objectp(queue[0]) || !interactive(queue[0])))
      queue = queue[1..100000];
   if(!sizeof(queue))
      return;
   ip_num  = query_ip_number(queue[0]);
   if((socket = socket_create(1, "read_call_back", "close_call_back")) < 0)
   {
#ifdef DEBUG
      _syslog("Couldn't create socket. Errno: " + -socket);
#endif
      call();
      return;
   }
   last_time = time();
#ifdef DEBUG
   _syslog("checking :" + ip_num);
#endif
   if(socket_connect(socket, ip_num + " 113", "read_call_back",
		     "write_call_back") < 0)
   {
#ifdef DEBUG
      _syslog("Couldn't connect to " + ip_num);
#endif
      queue[0]->set_ident(-1);
      close();
   }
}

void write_call_back(int sd)
{
#ifdef DEBUG
   _syslog("write_call_back: " + sd);
#endif
   if(!objectp(queue[0]) || !interactive(queue[0]))
      close();
   else if(socket_write(sd, query_ip_port(queue[0]) + ", 2000\n") < 0)
   {
#ifdef DEBUG
      _syslog("Write failed");
#endif
      queue[0]->set_ident(-1);
      close();
   }
   else
      last_time = time();
}

void read_call_back(int sd, string data)
{
   string msg;
#ifdef DEBUG
   _syslog("read_call_back: " + sd + ": " + data);
#endif
   if(objectp(queue[0]) &&
      sscanf(data, "%*s:%s:%*s: %s", msg, data) == 4 && msg != " ERROR ")
   {
      sscanf(data, "%s\r", data);
      sscanf(data, "%s\n", data);
      _syslog((string) queue[0]->query_real_name() + ": " + data);
      queue[0]->set_ident(data);
   }
   close();
}

void close_call_back(int sd)
{
#ifdef DEBUG
   _syslog("close_call_back: " + sd);
#endif
   close();
}
