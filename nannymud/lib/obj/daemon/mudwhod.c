/************************************
 *  The ghastly mudwhodaemon        *
 *  Sends info to our mudwhodaemon  *
 *  about logged in persons         *
 ************************************/

#define NAME     "NannyMUD"
#define PASS     "auharregud"
#define SERVER   "130.236.254.159 6888"
#define MASTER   "/obj/master"
#define COUNTRY  "/etc/country.data"
#define ERRFILE  "MUDWHOERROR"
#define DELAY    300

mapping countries;
int fd;

void load_country_info()
{
   string *info, ipname, cname, file;
   int i;
   if(this_player())
      enable_commands(), disable_commands();
   countries = ([]);
   if(!(file = (string) MASTER->daemon_read(COUNTRY)) || !(info = explode(file, "\n")))
      return;
   for(i = 0; i < sizeof(info); i++)
   {
      if(info[i][0] == '#')
	 continue;
      if(sscanf(info[i], "%s  %s", ipname, cname) != 2)
	 continue;
      countries[ipname] = cname;
   }
}

string query_country_name(string ipname)
{
   string name;
   if(countries && stringp(ipname))
      name   = countries[lower_case(explode(ipname, ".")[-1])];
   return (name ? name : "Unknown");
}

open_socket()
{
   string mess;
   int err;
   if((fd = socket_create(2, "receive_data")) < 0)
   {
      log_file(ERRFILE, "Error creating socket: " + socket_error(fd) + "\n");
      return -1;
   }
   mess = sprintf("U\t%s\t%s\t%s\t%d\t0\t%s", NAME, PASS, NAME, time(), version());
   if((err = socket_write(fd, mess, SERVER)) >= 0)
      return 0;
   socket_close(fd);
   log_file(ERRFILE, "Error sending upmess: " + socket_error(err) + "\n");
   return -1;
}

void receive_data(int fd, string str)
{
   log_file("MUDWHORECEIVE", "Got: " + str + "\n");
}

send_error(string errmess)
{
   string mess;
   log_file(ERRFILE, errmess);
   socket_close(fd);
   if((fd = socket_create(2, "receive_data")) < 0)
      return;
   mess = sprintf("M\t%s\t%s\t%s\t%d\t0\t%s", NAME, PASS, NAME, time(), version());
   socket_write(fd, mess, SERVER);
}

void send_mudwhodata()
{
   object *all;
   string mess;
   int err, i;
   if(find_call_out("send_mudwhodata") == -1)
      call_out("send_mudwhodata", DELAY);
   mess = sprintf("M\t%s\t%s\t%s\t%d\t0\t%s", NAME, PASS, NAME, time(), version());
   if((err = socket_write(fd, mess, SERVER)) < 0)
   {
      send_error("Error sending pingalive: " + socket_error(err) + "\n");
      return;
   }
   for(all = users(), i = 0; i < sizeof(all); i++)
   {
      if(!(all[i]->short()) && (int) all[i]->query_level() > 21)
	 continue;
      mess = sprintf("A\t%s\t%s\t%s\t%s\t%d\t0\t%s", NAME, PASS, NAME,
		     capitalize((string) all[i]->query_real_name()), time(),
		     query_country_name(query_ip_name(all[i])));
      if((err = socket_write(fd, mess, SERVER)) < 0)
      {
	 send_error("Error sending player: " + socket_error(err) + "\n");
	 break;
      }
   }
}

void send_login(object obj)
{
   string mess;
   int err;
   if(!obj || !interactive(obj) || !previous_object() ||
      extract(file_name(previous_object()), 0, 7) == "players/" ||
      (!obj->short() && (int) obj->query_level() > 21))
   {
      return;
   }
   mess = sprintf("A\t%s\t%s\t%s\t%s\t%d\t0\t%s", NAME, PASS, NAME,
		  capitalize((string) obj->query_real_name()), time(),
		  query_country_name(query_ip_name(obj)));
   if((err = socket_write(fd, mess, SERVER)) < 0)
      send_error("Error sending login: " + socket_error(err) + "\n");
}

void send_logout(object obj)
{
   string mess;
   int err;
   if(!obj || !interactive(obj) || !previous_object() ||
      extract(file_name(previous_object()), 0, 7) == "players/")
   {
      return;
   }
   mess = sprintf("Z\t%s\t%s\t%s\t%s", NAME, PASS, NAME,
		  capitalize((string) obj->query_real_name()));
   if((err = socket_write(fd, mess, SERVER)) < 0)
      send_error("Error sending logout: " + socket_error(err) + "\n");
}

void reset(status flag)
{
   if(flag || (int) open_socket() == -1)
      return;
   load_country_info();
   send_mudwhodata();
}

void _exit()
{
   socket_close(fd);
}
int query_prevent_shadow() { return 1; }
