#define HOSTPORT    "130.236.254.159 25"
#define MASTER      "/obj/master"
#define PLAYERDIR   "/players/"
#define MAILDIR     "/mail/testmail/"
#define ERRFILE     "MAILERR"
#define LOGFILE     "MAILLOG"
#define DELIM       "\n---------- FOO ----------\n\n"
#define SEND_OK      0
#define SEND_FAIL   -1

#undef  DEBUG

nomask static mapping mudlist, who;
nomask static string toline, ccline;
nomask static int mudlisttime;

void broken_conn(int fd)
{
   who = m_delete(who, fd);
#ifdef DEBUG
   log_file(LOGFILE, "Connection closed by foreign host.\n");
#endif
}

void write_data(int fd)
{
   string request;
   int err;
   if(!who[fd])
      return;
   if((err = socket_write(fd, who[fd][1], 0)) < 0)
   {
      socket_close(fd);
      who = m_delete(who, fd);
      log_file(ERRFILE, "Write: " + socket_error(err) + "\n");
   }
}

void receive_data(int fd, string str)
{
#ifdef DEBUG
   log_file(LOGFILE, "Receive: " + str + "\n");
#endif
}

external_mail(string to, string from, string mess)
{
   int fd, err;
   mess =
      "RCPT TO: " + to + "\n" +
      "MAIL FROM: " + from + "@nannymud.lysator.liu.se\n" +
      "DATA\n" +
/*      "From: " + from + "@nannymud.lysator.liu.se\n" +  */
      "X-Mud" + toline + "X-Mud" + ccline + mess + "\n.\n" +
      "QUIT\n";
   if((fd = socket_create(1, "receive_data", "broken_conn")) < 0)
   {
      log_file(ERRFILE, "Create: " + socket_error(fd) + "\n");
      write("Mail failed.\n");
      return SEND_FAIL;
   }
   if((err = socket_connect(fd,HOSTPORT,"receive_data","write_data")) < 0)
   {
      socket_close(fd);
      log_file(ERRFILE, "Connect: " + socket_error(fd) + "\n");
      write("Mail failed.\n");
      return SEND_FAIL;
   }
   if(!who)
      who = ([]);
   who[fd] = ({ this_player(), mess });
   write("Sending external mail to " + to + "\n");
   return SEND_OK;
}

status intermud_mail(string to, string from, string mess)
{
   mess =
      toline +
      "From: " + from + "\n" +
      "Date: " + ctime(time()) + "\n" +
      ccline +
      mess;
   write("Sending intermud mail to " + to + "\n");
   return SEND_FAIL;
}

status internal_mail(string to, string from, string mess)
{
   if((int) MASTER->daemon_file_size(PLAYERDIR + to + ".o") == -1)
      return SEND_FAIL;
   mess =
      DELIM +
      "Date: " + ctime(time()) + "\n" +
      "From: " + from + "\n" +
      toline +
      ccline +
      mess;
   MASTER->daemon_write(MAILDIR + to, mess);
   return SEND_OK;
}

status intermud_address(string addr)
{
   return 0;
}

status send_one_mail(string to, string from, string mess)
{
   string dummy;
   status result;
   if(sscanf(to, "%*s@%s", dummy) != 2)
      result = internal_mail(to, from, mess);
   else if(intermud_address(dummy))
      intermud_mail(to, from, mess);
   else
      result = (int) external_mail(to, from, mess);
   if(result == SEND_OK)
      write("Mail sent to " + to + ".\n");
   else
      write("Failed to send to " + to + ".\n");
   return result;
}

int send_mail(string *tolist,string *cclist,string *bcclist,string subject,string body)
{
   string *sendto, sender, message;
   int i, tot;
   sendto = (tolist - cclist) + cclist;
   sendto -= bcclist;
   sendto += bcclist;
   toline = "To: " + implode(tolist, " ") + "\n";
   ccline = (sizeof(cclist) > 0 ? "Cc: "+implode(cclist, " ")+"\n" : "");
   message = "Subject: " + subject + "\n\n" + body;
   sender = (string) this_player()->query_real_name();
   for(tot = 0, i = 0; i < sizeof(sendto); i++)
   {
      if(send_one_mail(sendto[i], sender, message) == SEND_OK)
	 tot++;
   }
   return tot;
}

mixed *get_mail(string player)
{
   mixed *ret;
   string all, *mails, from;
   int i;
   if(!(all = (string) MASTER->daemon_read(MAILDIR + player)) ||
      !(mails = explode(all, DELIM)))
   {
      return ({});
   }
   for(mails -= ({ "" }), ret = ({}), i = 0; i < sizeof(mails); i++)
   {
      if(sscanf(mails[i], "%*sFrom: %s\n%*s", from) < 2)
	 from = "";
      ret += ({ ({ from, mails[i] }) });
   }
   return ret;
}

int query_prevent_shadow() { return 1; }
