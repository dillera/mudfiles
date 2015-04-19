
int port = 2000;

run() {
  int fd;

  write(fd = socket_create(1, "read", "close"));
  write(socket_bind(fd, ++port));
  write(socket_listen(fd, "accept"));
}

read(int fd, mixed str)
{
  write("Got: " + fd + " -> " + str + "\n");
  socket_write(fd, str);
}

close(int fd)
{
  write("Close: " + fd + "\n");
}

writee(int fd)
{
  write("Writee got: " + fd + "\n");
}

accept(int fd)
{
  int newfd;

  newfd = socket_accept(fd, "read", "writee");

  write("New connection: " + newfd + "\n");
}
