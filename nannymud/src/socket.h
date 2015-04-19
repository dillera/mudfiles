

#define T_SOCK_NONE  0
#define T_SOCK_USER  1
#define T_SOCK_UNIX  2
#define T_SOCK_LPC   3


struct lp_socket {
  int  fd;
  int  type;
  int  state;
  int  (*read_handler) (int);
  int  (*write_handler)(int);
};

