
init()
{
  if(environment()!=this_player()) return;
  if(this_player()->query_level() < 23) 
  {
    destruct(this_object());
    return;
  }
      
  add_action("bind","bind");
  add_action("send","send");
}

int sock=-1;

static int bind()
{
  if(sock<0)
  {
    sock=socket_create(2,"read_call_back","write_call_back");
    if(sock < 0)
    {
      write("Failed to create socket.\n");
      return 1;
    }
  }
  if(socket_bind(sock,0)<0)
  {
    write("Couldn't bind local address.\n");
    socket_close(sock);
    sock=-1;
  }
  return 1;
}

void read_call_back(int fd,string b, string c)
{
  tell_object(environment(),"GOT: "+b+" from "+c+"\n");
}

void write_call_back(int fd,string b, string c, string d)
{
  tell_object(environment(),"write_callback\n");
}

static int send(string s)
{
  string addr,port,data;
  sscanf(s,"%s %s %s",addr,port,data);
  write(socket_write(sock, data, addr+" "+port)+"\n");
  return 1;
}


int get() { return 1; }
