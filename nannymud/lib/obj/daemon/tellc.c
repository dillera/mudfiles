#define PORTNO   "mud.lysator.liu.se 2020"

reset()
{
   int fd, err;
   if((fd = socket_create(2, "receive_data")) < 0)
   {
      write("Error creating socket: " + socket_error(fd) + "\n");
      return;
   }
   err = socket_write(fd, "Gwenda@NannyMUD:gwendolyn:Hej hopp", PORTNO);
   socket_close(fd);
   if(err < 0)
      write("Error sending data: " + socket_error(err) + "\n");
}
