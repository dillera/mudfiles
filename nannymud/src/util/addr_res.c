
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stropts.h>

#define PORT 2001

main(int argc, char *argv[]) 
{
  int sd;
  char buff[512];
  struct sockaddr_in my_addr, reply_addr;
  extern struct hostent *res_gethostbyaddr();

  if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket");
    exit(1);
  }
  memset((char *) &my_addr, sizeof(my_addr), 0);
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  my_addr.sin_port = htons(PORT);
  if(bind(sd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0)
  {
    perror("bind");
    exit(1);
  }
  while(1)
  {
    int n, len;
    char *p;
    unsigned long addr;
    struct hostent *hp;
    
    len = sizeof(reply_addr);
    if((n = recvfrom(sd, buff, sizeof(buff), 0, 
		     (struct sockaddr *) &reply_addr, &len)) < 0)
    {
      perror("recvfrom");
      exit(1);
    }
    buff[n] = '\0';
    if(p = strchr(buff, '\n'))
      *p = '\0';
    addr = inet_addr(buff);
    if (addr != -1) 
    {
      hp = res_gethostbyaddr(&addr, 4, AF_INET);
      if (!hp) 
      {
	sleep(5);
	hp = res_gethostbyaddr(&addr, 4, AF_INET);
      }
      if (hp) 
      {
	sprintf(buff + strlen(buff), " %s", hp->h_name);
	if((sendto(sd, buff, strlen(buff), 0, &reply_addr, len)) < 0)
	{
	  perror("sendto");
	  abort();
	}
      }
    }
  }
}
    
