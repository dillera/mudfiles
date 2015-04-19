#define SAVE_FILE "etc/dns_info"
#define SAVE_TIME 3600
#define DNSSERVER "130.236.254.159"
#define PORT      "2001"

#define UPDATE_TIME 86400
#define RETRY_TIME  3600
#define EXPIRE      3600000

#define MAX_QUERY   24

#define IP_NAME   0
#define TIMESTAMP 1
#define NUM_QUERY 2

static int requests_sent;
static int requests_received;

/* dns_info == ([ IP_NUMBER : ({ IP_NAME, TIMESTAMP, NUM_QUERY }) ]) */

mapping dns_info;
static int sd;

reset(status flag)
{
   if(flag)
      return;
   move_object(this_object(),"/obj/daemon/daemonroom");
   restore_object(SAVE_FILE);
   if(!dns_info)
      dns_info = ([ /* empty */ ]);
   if((sd = socket_create(2, "read_call_back", "read_call_back")) < 0)
   {
      _syslog("dnsd: Couldn't create socket: " + sd);
      destruct(this_object());
   }
   else if(socket_bind(sd, 0) < 0)
   {
      _syslog("dnsd: Couldn't bind local adress");
      destruct(this_object());
   }
   else
      call_out("save_dns_info", SAVE_TIME);

}

int is_valid(mixed *info)
{
  return info[TIMESTAMP]+EXPIRE > time();
}
  
void save_dns_info()
{
  int entries;
  entries=m_sizeof(dns_info);
  dns_info=filter_mapping(dns_info,"is_valid",this_object());
  entries-=m_sizeof(dns_info);
  tell_room(environment(),"The dns daemon burps "+entries+" time "+(entries==1?"":"s")+".\n");
  save_object(SAVE_FILE);
  remove_call_out("save_dns_info");
  call_out("save_dns_info", SAVE_TIME);
}

void send_query(string str)
{
   int ret;
   tell_room(environment(),"The dns daemon says: Checking "+str+".\n");
   ++requests_sent;
   if((ret = socket_write(sd, str, DNSSERVER + " " + PORT)) < 0)
      _syslog("dnsd: socket_write failed: " + ret);
}
  
string query_hostname(string str)
{
   mixed *hostinfo;
   string hostname;
   if(!stringp(str))
      return 0;
   hostinfo = dns_info[str];
   if(!pointerp(hostinfo) || sizeof(hostinfo) != 3)
   {
      hostinfo = allocate(3);
      hostinfo[TIMESTAMP] = time();
      hostinfo[NUM_QUERY] = 1;
      hostinfo[IP_NAME]   = 0;
      dns_info[str] = hostinfo;
      send_query(str);
      return str;
   }

   if((hostinfo[TIMESTAMP] + UPDATE_TIME) < time())
   {
     hostinfo[TIMESTAMP] = time();
     hostinfo[NUM_QUERY] ++;
     send_query(str);
     return hostinfo[IP_NAME] || str;
   }

   if(hostinfo[IP_NAME]) return hostinfo[IP_NAME];

   if((hostinfo[NUM_QUERY] >= MAX_QUERY) || 
      ((hostinfo[TIMESTAMP] + RETRY_TIME) > time()))
   {
      return str;
   }
   hostinfo[NUM_QUERY] ++;
   hostinfo[TIMESTAMP] = time();
   send_query(str);
   return str;
}

void read_call_back(int fd, string data, string peer)
{
   mixed *hinfo;
   string ip, name;
   _syslog("Got: " + data + " From: " + peer);
   if(sscanf(data, "%s %s", ip, name) == 2)
   {
      hinfo = dns_info[ip];
      if(!pointerp(hinfo) || sizeof(hinfo) != 3)
      {
	 _syslog("Bad entry for " + ip);
	 hinfo = allocate(3);
	 dns_info[ip] = hinfo;
      }
      hinfo[IP_NAME]   = name;
      hinfo[TIMESTAMP] = time();
      tell_room(environment(),"The dns daemon declares: "+ip+" equals "+name+"\n");
      ++requests_received;
   }
}

void init()
{
  add_action("nslookup","nslookup");
}

int nslookup(string s)
{
  mixed hostinfo;
  if(!s)
  {
    notify_fail("usage: nslookup [-f/i] <ip number>\n");
    return 0;
  }

  if(sscanf(s,"-f %s",s))
  {
    hostinfo = dns_info[s];
    if(pointerp(hostinfo) && sizeof(hostinfo) == 3)
    {
      hostinfo[NUM_QUERY]=1;
    }
  }

  if(sscanf(s,"-i %s",s))
  {
    hostinfo = dns_info[s];
    if(pointerp(hostinfo) && sizeof(hostinfo) == 3)
    {
      write("Name: "+hostinfo[IP_NAME]+".\n");
      write("Num tries "+hostinfo[NUM_QUERY]+".\n");
      write("Last retry/answer "+ctime(hostinfo[TIMESTAMP])+".\n");
    }else{
      write("No such entry in cache.\n");
    }
    return 1;
  }

  write("Server:  "+query_hostname(DNSSERVER)+"\n");
  write("Address:  "+DNSSERVER+"\n\n");
  write("Name:    "+query_hostname(s)+"\n");
  write("Address:  "+s+"\n\n");
  return 1;
}

mapping query_dns_info()
{
   return dns_info;
}

string short()
{
  return "a dns daemon";
}

int id(string s)
{
  return s=="dnsd" || s==short() || s=="dns daemon";
}

void long()
{
  write("This jolly daemon keeps track of the machine names,\n"+
	"so that people doesn't have to read numbers all the time.\n");
  write("In its right hand it is holding an infinite amount of requests\n"+
	"to send to other machines, of which he has used "+requests_sent+".\n");
  write("In it's left hand it is holding the "+requests_received+" requests\n"+
	"he has gotten in return.\n");
}

int query_prevent_shadow() { return 1; }
