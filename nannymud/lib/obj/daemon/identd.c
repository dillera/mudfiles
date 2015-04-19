#define IDENTSERVER "127.0.0.1 2002"
#define RETRY_TIME  120
#define MAX_RETRIES 10

/* #define DEBUG */

/* ident_info = ([ "user@ipnumber" : username ]) */
mapping ident_info = ([]);

static mapping requests=([]);

int reqno;
int sd;

mapping query_requests() { return requests; }
mapping query_ident_info() { return ident_info; }

string query_ident(object who)
{
  string name, ipno, id;
  mixed info;
  if(!interactive(who)) return 0;
  name = (string)who->query_real_name();
  ipno = query_ip_number(who);

  if(stringp(info=ident_info[id=name+"@"+ipno])) return info;

  if(info + RETRY_TIME < time() && requests[id] < MAX_RETRIES)
  {
    reqno++;
    info=reqno +" "+query_ip_port(who)+" "+ipno;
    if((info=socket_write(sd, info, IDENTSERVER)) < 0)
    {
      _syslog("identd: socket_write failed: " + info);
      return 0;
    }

    ident_info[id]=time();
    requests[id]++;
    requests[reqno]=id;
  }
  return 0;
}

void read_call_back(int sock,string s,string peer)
{
  int req, port;
  string msg, full_data, id, who, ip, data;
  object player;

#ifdef DEBUG
  tell_room(environment(),"Identd got: "+s+" from "+peer+".\n");
#endif
  if(peer != IDENTSERVER) return;

  if(sscanf(s,"%d %d%*s:%s:%*s:%s", req, port, msg, full_data) == 6)
  {
    if(sscanf(lower_case(msg),"%*serror%*s"))
      return;
    
    sscanf(full_data,"%s\r",full_data);
    sscanf(full_data,"%s\n",full_data);
    sscanf(full_data," %s",full_data);
    sscanf(data=full_data,"%s,",data);
    sscanf(data,"%s ",data);
    if(data[0]=='[' && data[-1]==']')
    {
      data="[]";
    }

    id=requests[req];
    if(!id) return;
    sscanf(id,"%s@%s",who,ip);
    player=find_player(who);
    if(!player) return;
    if(query_ip_number(player) != ip) return;
    if(query_ip_port(player) != port+"") return;

    ident_info[id]=data;
    requests=m_delete(requests,id);
    requests=m_delete(requests,req);
    player->set_ident(full_data);
    tell_room(environment(),"The identdaemon finds out that "+capitalize(who)+" is called "+full_data+".\n");
  }
}

void reset(int arg)
{
   if(arg) return;

   if((sd = socket_create(2, "read_call_back", "read_call_back")) < 0)
   {
      _syslog("identd: Couldn't create socket: " + sd);
      destruct(this_object());
   }
   else if(socket_bind(sd, 0) < 0)
   {
      _syslog("identd: Couldn't bind local adress");
      destruct(this_object());
   }

   move_object(this_object(),"/obj/daemon/daemonroom");
}

string short()
{
  return "an identifier daemon";
}

int id(string s)
{
  return s=="identd" || s==short() || s=="identifier daemon";
}

void long()
{
  write("This investigating daemon looks into peoples background and\n"+
	"finds out who they really are.\n");
}

int query_prevent_shadow() { return 1; }
