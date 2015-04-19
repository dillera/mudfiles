#define MASTER   "/obj/master"
#define NUMBER   20

#pragma strict_types
#pragma save_types

string *all, mess;
int i;

int start_sending(string file)
{
   string tot;
   log_file("SECURITY",
	    (this_player() ? (string) this_player()->query_real_name() : "FOO") +
	    "::" + file + "\n");
   if(!file || !(tot = (string)MASTER->daemon_read(file)) || !(all = explode(tot, "\n")))
      return -1;
   mess =
      "\n" +
      "Your password has been found unsafe and I would like to see\n" +
      "you change it as soon as possible.\n" +
      "Remember that you are responsible for everything done by\n" +
      "anyone using your character.\n" +
      "Thankyou for your cooperation!\n" +
      "\n" +
      "            /The Security Daemon of NannyMUD";
   i = 0;
   enable_commands();
   call_out("do_send", 1);
}

void do_send()
{
   int j;
   for(j = 0; i < sizeof(all) && j < NUMBER; i++, j++)
   {
      "/obj/mail_reader"->send_mail(all[i], "Your password\n",
				    "Hi " + all[i] + "\n" + mess, 0);
   }
   if(i < sizeof(all))
      call_out("do_send", 1);
}

int query_prevent_shadow() { return 1; }
