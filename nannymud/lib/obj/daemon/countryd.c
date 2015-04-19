/*  A country daemon to get the country name from an iphostname.  */  

#define COUNTRY_FILE  "/etc/country.data"

mapping c_list;    /* A mapping with all country data. */

void load_country_info()
{
   string *info, ipname, cname, file;
   int    i;
   if(this_player())
   {
      enable_commands();
      disable_commands();
   }
   c_list = ([]);
   file = (string) "/obj/master"->daemon_read(COUNTRY_FILE);
   info = explode((file ? file : ""), "\n");
   for(i = 0; info && i < sizeof(info); i++)
   {
      if(info[i][0] == '#')  /* Skip all comments. */
	 continue;
      if(sscanf(info[i], "%s  %s", ipname, cname) != 2)
	 continue;
      c_list[ipname] = cname;
   }
}

void dump_country_info() 
{
  write(sprintf("%O\n", c_list));
}

void reset(int arg) 
{
  if(arg) return;
  move_object(this_object(),"/obj/daemon/daemonroom");
  load_country_info();
}

long() { write("When you look at the daemon, you see the world!\n"); }
short() { return "a small copy of the world is floating here"; } 

id(a)
{
  return (a == "world" || a == "world daemon" || a == "countryd" ||
	  a == "country daemon" || a == "globe");
}

string query_country_name(string ipname)
{
  string name;


  if(ipname[0]>='0' && ipname[0]<='9')
    ipname="/obj/daemon/dnsd"->query_hostname(ipname);

  if(c_list && ipname && stringp(ipname))
    name   = c_list[ lower_case( explode(ipname + ".", ".")[ -1 ] ) ];

  return (name) ? name : "Unknown";
}
