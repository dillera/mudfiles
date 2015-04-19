int fd;

void reset()
{
  int ret;

  if((fd = socket_create(1, "read_call", "close_call")) < 0)
  {
    write(sprintf("ERROR:socket_create: %d\n", fd));
    return;
  }
  if((ret = socket_connect(fd, "130.236.254.1 13", "read_call", "write_call")) 
     < 0)
  {
    write(sprintf("ERROR:socket_connect: %d\n", ret));
    return;
  }
  write("Write ret: " + socket_write(fd, "kalle") + "\n");
  write("Ok\n");
}

read_call(fd, mess)
{
  write("Read call from: " + fd + "\n");
  write(mess);
}

write_call(fd)
{
  write("Write call from: " + fd + "\n");
}
    
close_call(fd)
{
  write("Close call from: " + fd + "\n");
}
