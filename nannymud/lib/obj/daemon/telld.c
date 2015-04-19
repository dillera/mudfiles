/************************************
 *  The ghastly mudwhodaemon        *
 *  Sends info to our mudwhodaemon  *
 *  about logged in persons         *
 ************************************/

#define PORTNO   2020
#define ERRFILE  "TELLDERROR"

int fd;

void receive_data(int fd, string str)
{
   object obj;
   string from, to, mess;
   if(sscanf(str, "%s:%s:%s", from, to, mess) != 3)
      log_file(ERRFILE, "Malformed input: " + str + "\n");
   else if(!(obj = find_player(to)))
      log_file(ERRFILE, "Player '" + to + "' not found.\n");
   else
      tell_object(obj, from + " tells you: " + mess + "\n");
}

void send_rtell(string host, string from, string to, string mess)
{
   int err;
   if((err = socket_write(fd, sprintf("%s:%s:%s", from, to, mess), host)) < 0)
      log_file(ERRFILE, "Error sending rtell: " + socket_error(err) + "\n");
}

reset(status flag)
{
   int err;
   if(flag)
      return;
   if((fd = socket_create(2, "receive_data")) < 0)
      log_file(ERRFILE, "Error creating socket: " + socket_error(fd) + "\n");
   else if((err = socket_bind(fd, PORTNO)) < 0)
   {
      socket_close(fd);
      log_file(ERRFILE, "Error binding socket: " + socket_error(err) + "\n");
   }
}
