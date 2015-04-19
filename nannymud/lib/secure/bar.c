/***********************************************************
 * Master listening socket descriptor:                     *
 *  mfd                                                    *
 * Incoming connections:                                   *
 *  socks   -> cfd : addr, user, access, flags, dfd, cwd   *
 * Data channels:                                          *
 *  datafds -> dfd : addr, cfd, access, flags, data        *
 * ------------------------------------------------------- *
 *  cfd     - cmd socket descriptor                        *
 *  addr    - remote address of connection                 *
 *  user    - wizardname                                   *
 *  access  - time of last command                         *
 *  flags   - flags for this connection                    *
 *  dfd     - data socket descriptor                       *
 *  data    - data to send on the socket                   *
 ***********************************************************/

/* Stream type socket... */
#define SOCK_STREAM     1

/* Default timeout value */
#define TIMEOUT       900

/* Descriptor flags */
#define SOCK_WROK     0x01
#define SOCK_AUTH     0x02
#define SOCK_ASCII    0x04

/* Index in values for connections descriptors */
#define CON_ADDR      0
#define CON_USER      1
#define CON_ACCESS    2
#define CON_FLAGS     3
#define CON_DATA      4
#define CON_CWD       5

/* Index in values for data descriptors */
#define DATA_ADDR     0
#define DATA_CONN     1
#define DATA_ACCESS   2
#define DATA_FLAGS    3
#define DATA_DATA     4

/* Globals - the master socket and two descriptor mappings */
private static nomask int mfd = -1;
private static nomask mapping socks = ([]), datafds = ([]);

/* Debug - I want to know everything */
private static nomask void syslog(string str)
{
   object obj;
   if(!(obj = find_player("gwendolyn")))
      return;
   tell_object(obj, "Ftpd [" + ctime(time()) + "]: " + str + "\n");
}

/* Setup the master socket */
private nomask void create()
{
   int err;
   if(mfd != -1)
      return;
   if((mfd = socket_create(SOCK_STREAM, "dummy_fun", "broken_conn")) < 0)
   {
      mfd = -1;
      syslog("Error creating socket: " + socket_error(mfd));
      return;
   }
   if((err = socket_bind(mfd, 1024)) < 0)
   {
      socket_close(mfd);
      mfd = -1;
      syslog("Error binding socket: " + socket_error(err));
      return;
   }
   if((err = socket_listen(mfd, "accept_conn")) < 0)
   {
      socket_close(mfd);
      mfd = -1;
      syslog("Error listening on socket: " + socket_error(err));
      return;
   }
   syslog("Starting...");
}

/* Standard reset() function */
nomask void reset(status flag)
{
   if(!flag)
      create();
}

/* Verify a password for a player (wizard) */
private nomask status passwd_ok(string user, string pwd)
{
   string whole, *lines, *res, pass;
   if(!(whole = read_file("/players/" + user + ".o")) ||
      !(lines = explode(whole, "\n")))
   {
      return 0;
   }
   if(sizeof(res = regexp(lines, "^password \".............\"$")) != 1)
      return 0;
   if(sscanf(res[0], "password \"%s\"", pass) != 1)
      return 0;
   if(crypt(pwd, pass) != pass)
      return 0;
   return 1;
}

private nomask status valid_read_path(string user, string path)
{
   if(user == "gwenda" || user == "qqqq")
      return 1;
   return 0;
}

/* Close a socket - with an optional message to syslog */
private static nomask void zap_connection(int fd, string mess)
{
   int num;
   num = fd < 0 ? -fd : fd;
   if(mess)
      syslog(mess);
   if(socks[num])
   {
      if(socks[num][CON_DATA] != -1)
	 zap_connection(socks[num][CON_DATA], 0);
      socks = m_delete(socks, num);
   }
   else
      datafds = m_delete(datafds, num);
   if(fd >= 0)
      socket_close(fd);
}

/* Dummy function - it should never get called */
nomask void dummy_fun(int fd)
{
   zap_connection(fd, "Huh, dummy_fun called...");
}

/* Write to a command socket - close it if write fails */
private nomask void do_write(int fd, string output)
{
   int err;
   if((err = socket_write(fd, output, 0)) < 0)
      zap_connection(fd, "Error writing: " + socket_error(err) + "\n");
}

/* Write to a data socket - special care has to be taken */
private nomask void do_data_write(int fd)
{
   string output;
   int err;
   if(!datafds[fd])
   {
      syslog("Bad fd to do_data_write");
      return;
   }
   if(!(datafds[fd][DATA_FLAGS] & SOCK_WROK))
      return;
   output = datafds[fd][DATA_DATA];
   datafds[fd][DATA_DATA] = "";
   if((err = socket_write(fd, output, 0)) < 0)
   {
      if(socks[datafds[fd][DATA_CONN]])
	 do_write(datafds[fd][DATA_CONN], "500 Could not write data.\r\n");
      zap_connection(fd, "Error writing: " + socket_error(err) + "\n");
      return;
   }
   if(socks[datafds[fd][DATA_CONN]])
   {
      do_write(datafds[fd][DATA_CONN], "226 " +
	       (socks[datafds[fd][DATA_CONN]][CON_FLAGS] & SOCK_ASCII ? "ASCII" : "BINARY") +
	       " Transfer complete.\r\n");
   }
   zap_connection(fd, "Data connection closed");
   return;
}

/* Handle new connections coming in */
nomask void accept_conn(int fd)
{
   int newfd;
   if(fd != mfd)
   {
      zap_connection(fd, "accept_conn called but not for master fd...");
      return;
   }
   if((newfd = socket_accept(fd, "receive_data", "dummy_fun")) < 0)
   {
      syslog("Error accepting on socket: " + socket_error(newfd));
      return;
   }
   socks[newfd] = ({ socket_address(newfd), 0, time(), SOCK_WROK, -1, "/" });
   syslog("Accepted fd [" + newfd + "] from " + socks[newfd][CON_ADDR]);
   do_write(newfd, "220 NannyMUD LPC ftp server 0.02 ready.\r\n");
}

/* A socket got closed */
nomask void broken_conn(int fd)
{
   if(fd == mfd)
      syslog("Master socket abnormally closed [" + fd + "].\n");
   else if(socks[fd])
      zap_connection(-fd, "Connection broken [" + fd + "]");
   else if(datafds[fd])
   {
      if(socks[datafds[fd][DATA_CONN]])
	 socks[datafds[fd][DATA_CONN]][CON_DATA] = -1;
      datafds = m_delete(datafds, fd);
      syslog("Data connection broken [" + fd + "]");
   }
   else
      syslog("Unknown socket in broken_conn [" + fd + "].\n");
}

/* A socket connection succeeded */
nomask void connected(int fd)
{
   if(datafds[fd])
   {
      datafds[fd][DATA_FLAGS] |= SOCK_WROK;
      if(datafds[fd][DATA_DATA] != "")
	 do_data_write(fd);
      syslog("Data write callback called [" + fd + "]");
   }
   else if(socks[fd])
   {
      socks[fd][CON_FLAGS] |= SOCK_WROK;
      syslog("Hmm, connection write callback called [" + fd + "]");
   }
   else
   {
      socket_close(fd);
      syslog("Spurious call to connected [" + fd + "]");
   }
}


/*  Command functions  */
/* =================== */

private nomask void sys_quit(int fd, string *args)
{
   do_write(fd, "221 Goodbye.\r\n");
   socks[fd][CON_DATA] = -1;   /* Leave possible data connection open */
   zap_connection(fd, "Connection closed [" + fd + "]");
}

private nomask void sys_help(int fd, string *args)
{
   do_write(fd, "214-The following commands are recognized:\r\n");
   do_write(fd, "   HELP      QUIT      NOOP      USER\r\n");
   do_write(fd, "   TYPE      ABOR      SYST      PASS\r\n");
   do_write(fd, "   NLST      LIST      PWD\r\n");
   do_write(fd, "214 (Will grow :)\r\n");
}

private nomask void sys_noop(int fd, string *args)
{
   do_write(fd, "200 '" + args[0] + "' command successful.\r\n");
}

private nomask void sys_user(int fd, string *args)
{
   if(sizeof(args) < 2)
   {
      do_write(fd, "500 '" + args[0] + "': Missing wizardname.\r\n");
      return;
   }
   if(file_size("/players/" + args[1] + ".o") < 0 ||
      file_size("/players/" + args[1]) != -2)
   {
      do_write(fd, "530 User '" + args[1] + "' access denied.\r\n");
      return;
   }
   socks[fd][3] &= ~SOCK_AUTH;
   socks[fd][1] = args[1];
   do_write(fd, "331 Password required for '" + args[1] + "'.\r\n");
}

private nomask void sys_pass(int fd, string *args)
{
   if(!socks[fd][CON_USER])
   {
      do_write(fd, "503 Login with USER first.\r\n");
      return;
   }
   if((socks[fd][CON_FLAGS] & SOCK_AUTH))
   {
      do_write(fd, "503 Already logged in.\r\n");
      return;
   }
   if(sizeof(args) < 2)
   {
      do_write(fd, "500 '" + args[0] + "': Missing password.\r\n");
      return;
   }
   if(!passwd_ok(socks[fd][CON_USER], args[1]))
   {
      do_write(fd, "530 Login incorrect.\r\n");
      return;
   }
   socks[fd][CON_FLAGS] |= SOCK_AUTH;
   do_write(fd, "230 User '" + socks[fd][CON_USER] + "' logged in.\r\n");
   syslog("Wizard " + socks[fd][CON_USER] + " logged on");
}

private nomask void sys_type(int fd, string *args)
{
   if(sizeof(args) < 2)
   {
      do_write(fd, "500 '" + args[0] + "': Missing type.\r\n");
      return;
   }
   if(args[1] == "i")
      socks[fd][3] &= ~SOCK_ASCII;
   else if(args[1] == "a")
      socks[fd][3] |= SOCK_ASCII;
   else
   {
      do_write(fd, "500 '" + args[0] + "': Bad type.\r\n");
      return;
   }
   do_write(fd, "200 Type set to " + args[1] + ".\r\n");
}

private nomask void sys_abor(int fd, string *args)
{
   do_write(fd, "225 '" + args[0] + "' command successful.\r\n");
}

private nomask void sys_port(int fd, string *args)
{
   string addr;
   int a,b,c,d,x,y, err, j;
   if(sscanf(args[1], "%d,%d,%d,%d,%d,%d", a, b, c, d, x, y) != 6)
   {
      do_write(fd, "500 '" + args[0] + "' Bad argument.\r\n");
      return;
   }
   if(!(socks[fd][CON_FLAGS] & SOCK_AUTH))
   {
      do_write(fd, "500 Login with USER and PASS first.\r\n");
      return;
   }
   if((j = socket_create(SOCK_STREAM, "dummy_fun", "broken_conn")) < 0)
   {
      do_write(fd, "500 System error.\r\n");
      return;
   }
   addr = a + "." + b + "." + c + "." + d + " " + (x * 256 + y);
   if((err = socket_connect(j, addr, "dummy_fun", "connected")) < 0)
   {
      socket_close(j);
      do_write(fd, "500 Error connecting.\r\n");
      return;
   }
   datafds[j] = ({ socket_address(j), fd, time(), 0, "" });
   socks[fd][CON_DATA] = j;
   do_write(fd, "225 '" + args[0] + "' command successful.\r\n");
   syslog("Connected to: " + addr + " [" + datafds[j][DATA_ADDR] + "]");
}

private nomask void sys_syst(int fd, string *args)
{
   do_write(fd, "215 UNIX Type\r\n");
}

private nomask void sys_nlst(int fd, string *args)
{
   string *files, mess;
   int err;
   if(!(socks[fd][CON_FLAGS] & SOCK_AUTH))
   {
      do_write(fd, "500 Login with USER and PASS first.\r\n");
      return;
   }
   if(socks[fd][CON_DATA] == -1 || !datafds[socks[fd][CON_DATA]])
   {
      /* Should try to open a data connection here maybe */
      do_write(fd, "500 No data connection.\r\n");
      return;
   }
   do_write(fd, "150 " +
	    (socks[fd][CON_FLAGS] & SOCK_ASCII ? "ASCII" : "BINARY") +
	    " data connection.\r\n");
   if(!(files = get_dir(socks[fd][CON_CWD])))
      mess = "\r\n";
   else
      mess = implode(files, "\r\n") + "\r\n";
   datafds[socks[fd][CON_DATA]][DATA_DATA] += mess;
   do_data_write(socks[fd][CON_DATA]);
}

private nomask void sys_list(int fd, string *args)
{
   sys_nlst(fd, args);
}

private nomask void sys_pwd(int fd, string *args)
{
   if(!(socks[fd][CON_FLAGS] & SOCK_AUTH))
   {
      do_write(fd, "500 Login with USER and PASS first.\r\n");
      return;
   }
   do_write(fd, "257 \"" + socks[fd][CON_CWD] + "\" is current directory.\r\n");
}

private nomask void sys_cwd(int fd, string *args)
{
   string *parts, path;
   int i;
   if(!(socks[fd][CON_FLAGS] & SOCK_AUTH))
   {
      do_write(fd, "500 Login with USER and PASS first.\r\n");
      return;
   }
   if(sizeof(args) > 1)
   {
      if(args[1][0] == '/')
	 path = args[1];
      else
	 path = socks[fd][CON_CWD] + "/" + args[1];
   }
   else
      path = "/players/" + socks[fd][CON_USER];
   syslog("Path: " + path);
   if(!(parts = explode(path, "/")) || !sizeof(parts -= ({ "", "." })))
      path = "/";
   else
   {
      while((i = member_array("..", parts)) != -1)
	 parts = parts[0..i-2] + parts[i+1..sizeof(parts)];
      path = "/" + implode(parts, "/");
   }
   syslog("Transformed to: " + path);
   if(!valid_read_path(socks[fd][CON_USER], path))
   {
      do_write(fd, "500 Permission denied.\r\n");
      return;
   }
   if(file_size(path) != -2)
   {
      do_write(fd, "500 Directory doesn't exist.\r\n");
      return;
   }
   socks[fd][CON_CWD] = path;
   do_write(fd, "200 Successful.\r\n");
}

private nomask void sys_cdup(int fd, string *args)
{
   if(!(socks[fd][CON_FLAGS] & SOCK_AUTH))
   {
      do_write(fd, "500 Login with USER and PASS first.\r\n");
      return;
   }
   if(sizeof(args) > 1)
   {
      do_write(fd, "500 Too many args to '" + args[0] + "'.\r\n");
      return;
   }
   args += ({ ".." });
   sys_cwd(fd, args);
}

private nomask void sys_retr(int fd, string *args)
{
   string *parts, path, mess;
   int i;
   if(!(socks[fd][CON_FLAGS] & SOCK_AUTH))
   {
      do_write(fd, "500 Login with USER and PASS first.\r\n");
      return;
   }
   if(socks[fd][CON_DATA] == -1 || !datafds[socks[fd][CON_DATA]])
   {
      /* Should try to open a data connection here maybe */
      do_write(fd, "500 No data connection.\r\n");
      return;
   }
   if(sizeof(args) < 2)
   {
      do_write(fd, "500 Missing argument.\r\n");
      return;
   }
   if(args[1][0] == '/')
      path = args[1];
   else
      path = socks[fd][CON_CWD] + "/" + args[1];
   syslog("Path: " + path);
   if(!(parts = explode(path, "/")) || !sizeof(parts -= ({ "", "." })))
      path = "/";
   else
   {
      while((i = member_array("..", parts)) != -1)
	 parts = parts[0..i-2] + parts[i+1..sizeof(parts)];
      path = "/" + implode(parts, "/");
   }
   syslog("Transformed to: " + path);
   if(!valid_read_path(socks[fd][CON_USER], path))
   {
      do_write(fd, "500 Permission denied.\r\n");
      return;
   }
   if(file_size(path) < 0)
   {
      do_write(fd, "500 File doesn't exist.\r\n");
      return;
   }
   if(!(mess = read_file(path)))
   {
      do_write(fd, "500 Failed to read file.\r\n");
      return;
   }
   do_write(fd, "150 " +
	    (socks[fd][CON_FLAGS] & SOCK_ASCII ? "ASCII" : "BINARY") +
	    " data connection.\r\n");
   datafds[socks[fd][CON_DATA]][DATA_DATA] += mess;
   do_data_write(socks[fd][CON_DATA]);
}

/* We got a command on one of our connections */
nomask void receive_data(int fd, string str)
{
   string *args;
   int i;
   if(!socks[fd])
   {
      zap_connection(fd, "Bad fd to receive_data");
      return;
   }
   socks[fd][CON_ACCESS] = time();
   /* Don't log passwords... */
   if(lower_case(str[0..3]) == "pass")
      syslog("Received [" + fd + "]: " + str[0..3] + " ...");
   else
      syslog("Received [" + fd + "]: " + str);
   for(i = strlen(str) - 1; str[i] == 10 || str[i] == 13; i--)
      ;
   if(i < 0 || !(args = explode(lower_case(str[0..i]), " ")) || !sizeof(args))
      return;
   switch(args[0])
   {
      case "quit":
	 sys_quit(fd, args);
	 break;
      case "help":
	 sys_help(fd, args);
	 break;
      case "noop":
	 sys_noop(fd, args);
	 break;
      case "user":
	 sys_user(fd, args);
	 break;
      case "pass":
	 sys_pass(fd, args);
	 break;
      case "type":
	 sys_type(fd, args);
	 break;
      case "abor":
	 sys_abor(fd, args);
	 break;
      case "syst":
	 sys_syst(fd, args);
	 break;
      case "port":
	 sys_port(fd, args);
	 break;
      case "nlst":
	 sys_nlst(fd, args);
	 break;
      case "list":
	 sys_list(fd, args);
	 break;
      case "cwd":
	 sys_cwd(fd, args);
	 break;
      case "cdup":
	 sys_cdup(fd, args);
	 break;
      case "pwd":
	 sys_pwd(fd, args);
	 break;
      case "retr":
	 sys_retr(fd, args);
	 break;
      case "stor":
	 /* No thanks */
      case "appe":
	 /* No thanks */
      case "dele":
	 /* No thanks */
      case "mkd":
	 /* No thanks */
      case "rmd":
	 /* No thanks */
      default:
	 do_write(fd, "500 '" + args[0] + "': Command not understood.\r\n");
	 break;
   }
}
