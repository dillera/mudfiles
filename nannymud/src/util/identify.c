#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stropts.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define DEBUG

#ifndef MAX_FD
#define MAX_FD 1024
#endif

#define PORT         2002
#define MUD_PORT     2000
#define IDENT_PORT    113
#define TIMEOUT     10*60

struct fdsets
{
  fd_set read, write;
};

int do_write(int fd, char *str, int len)
{
   int bytes, tries;
   for(tries = 0; len > 0; )
   {
      if(tries > 4)
	 return 0;
      if((bytes = write(fd, str, len)) <= -1)
      {
	 tries++;
#ifdef EAGAIN
	 if(errno == EAGAIN)
	    continue;
#endif
#ifdef EINTR
	 if(errno == EINTR)
	    continue;
#endif
#ifdef EPIPE
	 if(errno == EPIPE)
	    return -2;
#endif
	 return -1;
      }
#if 0
      else if(bytes > 0)
	 tries = 0;
#endif
      str += bytes;
      len -= bytes;
   }
   return 1;
}

int main(int argc, char *argv[]) 
{
  char buff[513];
  int max_fd, i, e;
  int ports[MAX_FD];
  int connected[MAX_FD];
  int requests[MAX_FD];

  struct fdsets fds;
  int sd,last_clean;
  struct sockaddr_in my_addr, reply_addr;
  struct sockaddr_in reply_addrs[MAX_FD];

  signal(SIGPIPE, SIG_IGN);
  memset((char *) & ports, sizeof(ports), 0);
  memset((char *) & connected, sizeof(connected), 0);

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

  FD_ZERO( &fds.read );
  FD_ZERO( &fds.write );
  FD_SET(sd, & fds.read);
  max_fd=sd;

  last_clean=0;

  while(1)
  {
    int e, n, tmp, len, req, port, new_fd;
    char *p;
    struct sockaddr_in sin;
    struct fdsets selected;

    selected=fds;
    if(select(max_fd+1, & selected.read, &selected.write, 0, 0) < 0)
    {
      perror("select");
      exit(1);
    }
    
    if(FD_ISSET(sd, &selected.read))
    {
      len = sizeof(reply_addr);
      if((n = recvfrom(sd, buff, sizeof(buff), 0, 
		       (struct sockaddr *) &reply_addr, &len)) < 0)
      {
	perror("recvfrom");
	exit(1);
      }

      buff[n] = '\0';
      if(p = strchr(buff, '\n')) *p = '\0';
#ifdef DEBUG
      printf("GOT: %s\n",buff);
#endif
      p=buff;
      req=strtol(p, &p, 10);
      port=strtol(p, &p, 10);
      while(isspace(*p) && *p) p++;
    
      new_fd=socket(AF_INET, SOCK_STREAM, 0);

      if(new_fd > max_fd) max_fd=new_fd;
      if(new_fd == -1)
      {
	perror("socket");
	continue;
      }

      if(new_fd > MAX_FD)
      {
	close(new_fd);
	fprintf(stderr,"Filedescriptor out of range.\n");
	continue;
      }

      if( fcntl(new_fd, F_SETFL, O_NONBLOCK) < 0)
      {
	perror("fcntl");
	close(new_fd);
	continue;
      }

      sin.sin_family = AF_INET;
      sin.sin_port = htons(IDENT_PORT);
      sin.sin_addr.s_addr = inet_addr(p);

      if(connect(new_fd, (struct sockaddr *) &sin, sizeof(sin)) < 0)
      {
	if(errno != EINPROGRESS)
	{
	  perror("connect");
	  close(new_fd);
	  continue;
	}
      }
#ifdef DEBUG
      printf("%d connecting to %s\n",new_fd,p);
#endif
      ports[new_fd]=port;
      connected[new_fd]=(int)time(0);
      reply_addrs[new_fd]=reply_addr;
      requests[new_fd]=req;
      FD_SET(new_fd, & fds.write);
    }

    for(e=0;e<=max_fd;e++)
    {
       if(!ports[e])
	  continue;
       if(FD_ISSET(e, &selected.write))
       {
	  sprintf(buff,"%d , %d\r\n", ports[e], MUD_PORT);
#ifdef DEBUG
	  printf("%d writing request: %s", e, buff);
#endif
	  FD_CLR(e, &fds.write);
	  switch(do_write(e, buff, strlen(buff)))
	  {
	     case -2:
#ifdef DEBUG
	        printf("%d: No connection.\n", e);
#endif
	        close(e);
		continue;
	     case -1:
	        perror("write");
		close(e);
		continue;
	     case 0:
	        fprintf(stderr, "%d: Write failed.\n", e);
		close(e);
		continue;
	     default:
	        break;
	  }
	  FD_SET(e, &fds.read);
       }
    }

    for(e=0;e<=max_fd;e++)
    {
      if(!ports[e]) continue;

      if(FD_ISSET(e, &selected.read))
      {
	char buf2[1025];
	int i, tmp;

	if((i = read(e, buff, 512)) < 0)
	   perror("read");
	else
	{
	   buff[i]=0;
#ifdef DEBUG
	   printf("%d answer:\n%s",e,buff);
#endif
	   if(strchr(buff,'\n'))
	   {
	      sprintf(buf2,"%d %s",requests[e],buff);
	      if((sendto(sd, buf2, strlen(buf2), 0,
			 (struct sockaddr *)&reply_addrs[e], len)) < 0)
	      {
		 perror("sendto");
		 exit(1);
	      }
	   }
	}
	close(e);
	ports[e]=0;
	FD_CLR(e, & fds.read);
	FD_CLR(e, & fds.write);
      }
    }
    
    tmp=(int)time(0);
    if(last_clean + 20 < tmp)
    {
      last_clean=tmp;

      for(e=0;e<max_fd;e++)
      {
	if(ports[e])
	{
	  if(connected[e]+TIMEOUT < tmp)
	  {
	    close(e);
	    ports[e]=0;
	    FD_CLR(e, &fds.read);
	    FD_CLR(e, &fds.write);
	  }
	}
      }
    }
  }
}
    
